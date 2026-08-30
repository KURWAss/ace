#include "ewmh.h"

#include <X11/Xatom.h>

namespace ewmh {

namespace {

Window g_check_window = None;

}  // namespace

void Initialize(Display* display, Window root) {
    g_check_window = XCreateSimpleWindow(display, root, -1, -1, 1, 1, 0, 0, 0);

    Atom net_supporting_wm_check = XInternAtom(display, "_NET_SUPPORTING_WM_CHECK", False);
    Atom net_wm_name = XInternAtom(display, "_NET_WM_NAME", False);
    Atom utf8_string = XInternAtom(display, "UTF8_STRING", False);
    Atom net_supported = XInternAtom(display, "_NET_SUPPORTED", False);
    Atom net_client_list = XInternAtom(display, "_NET_CLIENT_LIST", False);
    Atom net_active_window = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);

    XChangeProperty(
        display, root, net_supporting_wm_check, XA_WINDOW, 32,
        PropModeReplace, reinterpret_cast<unsigned char*>(&g_check_window), 1);
    XChangeProperty(
        display, g_check_window, net_supporting_wm_check, XA_WINDOW, 32,
        PropModeReplace, reinterpret_cast<unsigned char*>(&g_check_window), 1);

    const char* wm_name = "ace";
    XChangeProperty(
        display, g_check_window, net_wm_name, utf8_string, 8,
        PropModeReplace, reinterpret_cast<const unsigned char*>(wm_name), 3);

    Atom supported[] = {
        net_supported,
        net_supporting_wm_check,
        net_client_list,
        net_active_window,
        net_wm_name,
    };
    XChangeProperty(
        display, root, net_supported, XA_ATOM, 32,
        PropModeReplace, reinterpret_cast<unsigned char*>(supported),
        sizeof(supported) / sizeof(supported[0]));

    UpdateClientList(display, root, {});
    UpdateActiveWindow(display, root, None);
}

void UpdateClientList(Display* display, Window root, const std::vector<Window>& windows) {
    Atom net_client_list = XInternAtom(display, "_NET_CLIENT_LIST", False);

    XChangeProperty(
        display, root, net_client_list, XA_WINDOW, 32,
        PropModeReplace,
        reinterpret_cast<unsigned char*>(const_cast<Window*>(windows.data())),
        static_cast<int>(windows.size()));
}

void UpdateActiveWindow(Display* display, Window root, Window window) {
    Atom net_active_window = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);

    XChangeProperty(
        display, root, net_active_window, XA_WINDOW, 32,
        PropModeReplace, reinterpret_cast<unsigned char*>(&window), 1);
}

}  // namespace ewmh
