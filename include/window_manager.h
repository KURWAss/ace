#ifndef ACE_WINDOW_MANAGER_H_
#define ACE_WINDOW_MANAGER_H_

#include <X11/Xlib.h>
#include <unordered_map>

class WindowManager {
public:
    static WindowManager* Create();
    ~WindowManager();

    WindowManager(const WindowManager&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;

    void Run();

private:
    explicit WindowManager(Display* display);

    void OnMapRequest(const XMapRequestEvent& e);
    void OnConfigureRequest(const XConfigureRequestEvent& e);
    void OnButtonPress(const XButtonEvent& e);
    void OnButtonRelease(const XButtonEvent& e);
    void OnMotionNotify(const XMotionEvent& e);
    void OnKeyPress(const XKeyEvent& e);
    void OnDestroyNotify(const XDestroyWindowEvent& e);
    void OnEnterNotify(const XCrossingEvent& e);

    static int OnXError(Display* display, XErrorEvent* e);

    Display* display_;
    Window root_;

    Window drag_start_window_ = None;
    XButtonEvent drag_start_button_;
    XWindowAttributes drag_start_attributes_;

    std::unordered_map<Window, bool> managed_windows_;
};

#endif  // ACE_WINDOW_MANAGER_H_
