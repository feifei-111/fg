// 定义 window
// window 配置了处理 event 的用户接口
// window 能够调用 sys window 提供的底层 api
// 之所以 window 和 sys window 分开，是因为，希望 event 处理接口能够无视 sys 差异
// 因为 window 可能需要提供一些类似于 draw_circle 之类的接口，这些接口是给用户使用的（在 event 处理接口中）
// 感觉这些逻辑和 windows 的逻辑放在一起不是很好？

namespace fei_window{

class FEI_Window{
    FEI_Window();
    ~FEI_Window();

    void init();
};

}
