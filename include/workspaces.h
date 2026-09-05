#ifndef ACE_WORKSPACES_H_
#define ACE_WORKSPACES_H_

#include <X11/Xlib.h>

namespace workspaces {

constexpr int kWorkspaceCount = 10;

void Initialize(Display* display, Window root);
void AddWindow(Window window);
void RemoveWindow(Window window);
void SwitchTo(int index);
int GetCurrentIndex();

}  // namespace workspaces

#endif  // ACE_WORKSPACES_H_
