#include "window_manager.h"

int main() {
    WindowManager* wm = WindowManager::Create();
    if (wm == nullptr) {
        return 1;
    }
    wm->Run();
    delete wm;
    return 0;
}
