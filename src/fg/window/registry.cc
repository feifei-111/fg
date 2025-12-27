
#include <unordered_map>

#include "fg/utils/log.h"
#include "fg/window/registry.h"

namespace fg::window {
// 在 poll event 时，需要更新 window state
// 所以 event 必须知道自己来自哪个 window
// 然后我们需要找到这个 window 对应的 state，实际上 window 持有自己的 state
// 我们为每一个 window 分配一个 id，然后在这里注册即可
static std::unordered_map<unsigned int, WindowRegisterInfo> GlobalWindowRegisterMap;

void RegisterWindow(const WindowRegisterInfo& reg_info) {
    GlobalWindowRegisterMap.emplace(reg_info.id, reg_info);
}

void UnregisterWindow(unsigned int window_id) { GlobalWindowRegisterMap.erase(window_id); }

WindowBase* GetRegisteredWindowPtr(unsigned int window_id){
    CHECK(GlobalWindowRegisterMap.find(window_id) !=
            GlobalWindowRegisterMap.end()) << "window_id " << window_id << " not found";
    return GlobalWindowRegisterMap[window_id].window;
}

}