#include "workspaces.h"

#include <unordered_map>

namespace workspaces {

namespace {

Display* g_display = nullptr;
Window g_root = None;
int g_current_index = 0;
std::unordered_map<Window, int> g_window_workspace;

}  // namespace

void Initialize(Display* display, Window root) {
    g_display = display;
    g_root = root;
    g_current_index = 0;
    g_window_workspace.clear();
}

void AddWindow(Window window) {
    g_window_workspace[window] = g_current_index;
}

void RemoveWindow(Window window) {
    g_window_workspace.erase(window);
}

void SwitchTo(int index) {
    if (index < 0 || index >= kWorkspaceCount || index == g_current_index) {
        return;
    }

    for (const auto& entry : g_window_workspace) {
        if (entry.second == index) {
            XMapWindow(g_display, entry.first);
        } else if (entry.second == g_current_index) {
            XUnmapWindow(g_display, entry.first);
        }
    }

    g_current_index = index;
}

int GetCurrentIndex() {
    return g_current_index;
}

}  // namespace workspaces
