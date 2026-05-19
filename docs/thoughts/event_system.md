# event list 怎么运作

现在的 event system （主要讨论 win32）其实比较简单，核心就是有一个 global 的 event list，窗口系统往里面塞， 我们的应用往外读取

这里需要一点基本的 message loop 知识

windows 每一个窗体对应一个 UI 线程，对应一个消息队列，os 会把消息放到这个队列里面

实际上 global event list 只让一个线程访问，所以不需要锁

于是最重要的两件事是：

1. CollectEvents: 往 event list 塞 event
    通过系统调用 PeekMessage DispatchMessageW 进入 WindowProc （用户给窗体挂的 hook）
    WindowProc 内处理系统 msg，转为 event 塞进 event list
2. PollEvent: 从 event list pop
    直接提供一个接口输出就可以了，毕竟 event list 是我们提供的

我看了 SDL 的写法，它似乎只有一个 Poll 的接口，在 Poll 内可能触发 CollectEvents，我觉得这个语义有点不太自然
所以我改成了，需要手动 CollectEvents，然后在一个帧内通过循环 Poll 处理所有 event，这样才能把 Event 以组为单位进行处理，每组对应一帧中需要的操作

为了顺便支持 Mac，用了 glfw，系统接口不太一样，CollectEvents 的行为必须和系统相关，所以改成了一个 hook

# event 丢失

我觉得 event 丢失是正常的，默认 ring buffer 有 128 个固定 slot，如果满了就会丢弃
相当于一帧最多处理 128 个 event，我觉得这个数值完全足够了


# window state 的维护

还有一套逻辑是关于 window state 的
显然有大量的 mouse move 或者 keyboard 相关的 event，如果每个 event 都要手动记录状态更新，这太蠢了

window state 就是记录一个 window 固有属性的结构，包括当前的按键状态，鼠标位置等

这些状态是在 PollEvent 时自动更新的，所以可以直接检查当前 Window 的 state 获取信息

这一点也是参考了 SDL 的设计得到的

另外，应该保证 event 和 window 是对应的，所以加了 window id 和 register（不过我其实没有考虑多窗口，感觉也没啥多窗口的必要，所以目前是这样，看起来 register 也挺鸡肋的）


# others

另外，这里本来是用一个继承来分发不同 platform 的，但是那样就有一个 vptr，感觉压根没必要，所以改成了持有 WindowImpl 的形式，直接根据宏分支实现