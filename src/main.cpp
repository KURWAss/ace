#include "config.h"
#include "window_manager.h"

int main() {
    EnsureConfigExists();

    WindowManager* wm = WindowManager::Create();
    if (wm == nullptr) {
        return 1;
    }
    wm->Run();
    delete wm;
    return 0;
}
