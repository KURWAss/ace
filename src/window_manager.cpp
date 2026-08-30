#include "window_manager.h"

#include <X11/XKBlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "config.h"

int WindowManager::OnXError(Display* display, XErrorEvent* e) {
    char buffer[1024];
    XGetErrorText(display, e->error_code, buffer, sizeof(buffer));
    fprintf(stderr, "X error: %s (request code %d, minor code %d)\n",
            buffer, e->request_code, e->minor_code);
    return 0;
}

WindowManager* WindowManager::Create() {
    Display* display = XOpenDisplay(nullptr);
    if (display == nullptr) {
        return nullptr;
    }
    return new WindowManager(display);
}

WindowManager::WindowManager(Display* display)
    : display_(display),
      root_(DefaultRootWindow(display)),
      terminal_command_(GetTerminalCommand()),
      launcher_command_(GetLauncherCommand()) {}

WindowManager::~WindowManager() {
    XCloseDisplay(display_);
}

void WindowManager::Run() {
    XSetErrorHandler(&WindowManager::OnXError);

    XSelectInput(
        display_,
        root_,
        SubstructureRedirectMask | SubstructureNotifyMask);
    XSync(display_, False);

    XGrabButton(
        display_,
        Button1,
        Mod1Mask,
        root_,
        False,
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
        GrabModeAsync,
        GrabModeAsync,
        None,
        None);

    XGrabButton(
        display_,
        Button3,
        Mod1Mask,
        root_,
        False,
        ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
        GrabModeAsync,
        GrabModeAsync,
        None,
        None);

    GrabKeyWithLockVariants(XK_q, Mod1Mask | ShiftMask);
    GrabKeyWithLockVariants(XK_space, Mod4Mask);
    GrabKeyWithLockVariants(XK_q, Mod4Mask);
    GrabKeyWithLockVariants(XK_d, Mod4Mask);

    InitializeEwmh();

    for (const std::string& command : GetAutostartCommands()) {
        LaunchCommand(command);
    }

    for (;;) {
        XEvent e;
        XNextEvent(display_, &e);

        switch (e.type) {
            case MapRequest:
                OnMapRequest(e.xmaprequest);
                break;
            case ConfigureRequest:
                OnConfigureRequest(e.xconfigurerequest);
                break;
            case ButtonPress:
                OnButtonPress(e.xbutton);
                break;
            case ButtonRelease:
                OnButtonRelease(e.xbutton);
                break;
            case MotionNotify:
                while (XCheckTypedWindowEvent(display_, e.xmotion.window, MotionNotify, &e)) {}
                OnMotionNotify(e.xmotion);
                break;
            case KeyPress:
                OnKeyPress(e.xkey);
                break;
            case DestroyNotify:
                OnDestroyNotify(e.xdestroywindow);
                break;
            case EnterNotify:
                OnEnterNotify(e.xcrossing);
                break;
            default:
                break;
        }
    }
}

void WindowManager::OnMapRequest(const XMapRequestEvent& e) {
    XSelectInput(display_, e.window, EnterWindowMask);
    XMapWindow(display_, e.window);
    managed_windows_[e.window] = true;
    UpdateClientList();

    XWindowAttributes attrs;
    XGetWindowAttributes(display_, e.window, &attrs);
    if (attrs.x == 0 && attrs.y == 0) {
        XMoveWindow(display_, e.window, 50, 50);
    }
}

void WindowManager::OnConfigureRequest(const XConfigureRequestEvent& e) {
    XWindowChanges changes;
    changes.x = e.x;
    changes.y = e.y;
    changes.width = e.width;
    changes.height = e.height;
    changes.border_width = e.border_width;
    changes.sibling = e.above;
    changes.stack_mode = e.detail;
    XConfigureWindow(display_, e.window, e.value_mask, &changes);
}

void WindowManager::OnButtonPress(const XButtonEvent& e) {
    if (e.subwindow == None) {
        return;
    }

    XGetWindowAttributes(display_, e.subwindow, &drag_start_attributes_);
    drag_start_button_ = e;
    drag_start_window_ = e.subwindow;

    XRaiseWindow(display_, e.subwindow);
}

void WindowManager::OnButtonRelease(const XButtonEvent&) {
    drag_start_window_ = None;
}

void WindowManager::OnMotionNotify(const XMotionEvent& e) {
    if (drag_start_window_ == None) {
        return;
    }

    int delta_x = e.x_root - drag_start_button_.x_root;
    int delta_y = e.y_root - drag_start_button_.y_root;

    if (drag_start_button_.button == Button1) {
        int dest_x = drag_start_attributes_.x + delta_x;
        int dest_y = drag_start_attributes_.y + delta_y;
        XMoveWindow(display_, drag_start_window_, dest_x, dest_y);
    } else if (drag_start_button_.button == Button3) {
        int dest_width = drag_start_attributes_.width + delta_x;
        int dest_height = drag_start_attributes_.height + delta_y;
        dest_width = dest_width > 20 ? dest_width : 20;
        dest_height = dest_height > 20 ? dest_height : 20;
        XResizeWindow(display_, drag_start_window_, dest_width, dest_height);
    }
}

void WindowManager::OnKeyPress(const XKeyEvent& e) {
    KeySym keysym = XkbKeycodeToKeysym(display_, e.keycode, 0, 0);

    if (keysym == XK_q && (e.state & Mod1Mask) && (e.state & ShiftMask)) {
        exit(0);
    }

    if (keysym == XK_space && (e.state & Mod4Mask)) {
        LaunchCommand(terminal_command_);
    }

    if (keysym == XK_q && (e.state & Mod4Mask)) {
        CloseFocusedWindow();
    }

    if (keysym == XK_d && (e.state & Mod4Mask)) {
        LaunchCommand(launcher_command_);
    }
}

void WindowManager::GrabKeyWithLockVariants(KeySym keysym, unsigned int modifiers) {
    KeyCode keycode = XKeysymToKeycode(display_, keysym);

    const unsigned int lock_variants[] = {
        0,
        LockMask,
        Mod2Mask,
        LockMask | Mod2Mask,
    };

    for (unsigned int lock_mask : lock_variants) {
        XGrabKey(
            display_,
            keycode,
            modifiers | lock_mask,
            root_,
            False,
            GrabModeAsync,
            GrabModeAsync);
    }
}

void WindowManager::LaunchCommand(const std::string& command) {
    std::vector<std::string> tokens;
    std::istringstream stream(command);
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        return;
    }

    pid_t pid = fork();

    if (pid < 0) {
        return;
    }

    if (pid == 0) {
        setsid();

        std::vector<char*> argv;
        for (std::string& t : tokens) {
            argv.push_back(t.data());
        }
        argv.push_back(nullptr);

        execvp(argv[0], argv.data());
        _exit(1);
    }
}

void WindowManager::OnEnterNotify(const XCrossingEvent& e) {
    XSetInputFocus(display_, e.window, RevertToPointerRoot, CurrentTime);
    focused_window_ = e.window;
    UpdateActiveWindow(e.window);
}

void WindowManager::CloseFocusedWindow() {
    if (focused_window_ == None) {
        return;
    }

    Atom wm_protocols = XInternAtom(display_, "WM_PROTOCOLS", False);
    Atom wm_delete_window = XInternAtom(display_, "WM_DELETE_WINDOW", False);

    Atom* supported_protocols = nullptr;
    int protocol_count = 0;
    bool supports_delete = false;

    if (XGetWMProtocols(display_, focused_window_, &supported_protocols, &protocol_count)) {
        for (int i = 0; i < protocol_count; ++i) {
            if (supported_protocols[i] == wm_delete_window) {
                supports_delete = true;
                break;
            }
        }
        XFree(supported_protocols);
    }

    if (supports_delete) {
        XClientMessageEvent message = {};
        message.type = ClientMessage;
        message.window = focused_window_;
        message.message_type = wm_protocols;
        message.format = 32;
        message.data.l[0] = static_cast<long>(wm_delete_window);
        message.data.l[1] = CurrentTime;

        XSendEvent(display_, focused_window_, False, NoEventMask,
                   reinterpret_cast<XEvent*>(&message));
    } else {
        XKillClient(display_, focused_window_);
    }
}

void WindowManager::OnDestroyNotify(const XDestroyWindowEvent& e) {
    managed_windows_.erase(e.window);
    UpdateClientList();

    if (drag_start_window_ == e.window) {
        drag_start_window_ = None;
    }
    if (focused_window_ == e.window) {
        focused_window_ = None;
        UpdateActiveWindow(None);
    }
}

void WindowManager::InitializeEwmh() {
    check_window_ = XCreateSimpleWindow(display_, root_, -1, -1, 1, 1, 0, 0, 0);

    Atom net_supporting_wm_check = XInternAtom(display_, "_NET_SUPPORTING_WM_CHECK", False);
    Atom net_wm_name = XInternAtom(display_, "_NET_WM_NAME", False);
    Atom utf8_string = XInternAtom(display_, "UTF8_STRING", False);
    Atom net_supported = XInternAtom(display_, "_NET_SUPPORTED", False);
    Atom net_client_list = XInternAtom(display_, "_NET_CLIENT_LIST", False);
    Atom net_active_window = XInternAtom(display_, "_NET_ACTIVE_WINDOW", False);

    XChangeProperty(
        display_, root_, net_supporting_wm_check, XA_WINDOW, 32,
        PropModeReplace, reinterpret_cast<unsigned char*>(&check_window_), 1);
    XChangeProperty(
        display_, check_window_, net_supporting_wm_check, XA_WINDOW, 32,
        PropModeReplace, reinterpret_cast<unsigned char*>(&check_window_), 1);

    const char* wm_name = "ace";
    XChangeProperty(
        display_, check_window_, net_wm_name, utf8_string, 8,
        PropModeReplace, reinterpret_cast<const unsigned char*>(wm_name), 3);

    Atom supported[] = {
        net_supported,
        net_supporting_wm_check,
        net_client_list,
        net_active_window,
        net_wm_name,
    };
    XChangeProperty(
        display_, root_, net_supported, XA_ATOM, 32,
        PropModeReplace, reinterpret_cast<unsigned char*>(supported),
        sizeof(supported) / sizeof(supported[0]));

    UpdateClientList();
    UpdateActiveWindow(None);
}

void WindowManager::UpdateClientList() {
    Atom net_client_list = XInternAtom(display_, "_NET_CLIENT_LIST", False);

    std::vector<Window> windows;
    windows.reserve(managed_windows_.size());
    for (const auto& entry : managed_windows_) {
        windows.push_back(entry.first);
    }

    XChangeProperty(
        display_, root_, net_client_list, XA_WINDOW, 32,
        PropModeReplace, reinterpret_cast<unsigned char*>(windows.data()),
        static_cast<int>(windows.size()));
}

void WindowManager::UpdateActiveWindow(Window window) {
    Atom net_active_window = XInternAtom(display_, "_NET_ACTIVE_WINDOW", False);

    XChangeProperty(
        display_, root_, net_active_window, XA_WINDOW, 32,
        PropModeReplace, reinterpret_cast<unsigned char*>(&window), 1);
}
