#include "window_manager.h"

#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

#include "config.h"

int WindowManager::OnXError(Display* display, XErrorEvent* e) {
    char buffer[1024];
    XGetErrorText(display, e->error_code, buffer, sizeof(buffer));
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
      terminal_command_(GetTerminalCommand()) {}

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

    XGrabKey(
        display_,
        XKeysymToKeycode(display_, XK_Q),
        Mod1Mask | ShiftMask,
        root_,
        False,
        GrabModeAsync,
        GrabModeAsync);

    XGrabKey(
        display_,
        XKeysymToKeycode(display_, XK_space),
        Mod4Mask,
        root_,
        False,
        GrabModeAsync,
        GrabModeAsync);

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

    if (keysym == XK_Q && (e.state & Mod1Mask) && (e.state & ShiftMask)) {
        exit(0);
    }

    if (keysym == XK_space && (e.state & Mod4Mask)) {
        LaunchCommand(terminal_command_);
    }
}

void WindowManager::LaunchCommand(const std::string& command) {
    pid_t pid = fork();

    if (pid < 0) {
        return;
    }

    if (pid == 0) {
        setsid();
        execlp(command.c_str(), command.c_str(), nullptr);
        _exit(1);
    }
}

void WindowManager::OnEnterNotify(const XCrossingEvent& e) {
    XSetInputFocus(display_, e.window, RevertToPointerRoot, CurrentTime);
}

void WindowManager::OnDestroyNotify(const XDestroyWindowEvent& e) {
    managed_windows_.erase(e.window);
    if (drag_start_window_ == e.window) {
        drag_start_window_ = None;
    }
}
