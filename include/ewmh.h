#ifndef ACE_EWMH_H_
#define ACE_EWMH_H_

#include <X11/Xlib.h>
#include <vector>

namespace ewmh {

void Initialize(Display* display, Window root);
void UpdateClientList(Display* display, Window root, const std::vector<Window>& windows);
void UpdateActiveWindow(Display* display, Window root, Window window);

}  // namespace ewmh

#endif  // ACE_EWMH_H_
