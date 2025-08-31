// 在这里定义我们要处理的 event
// 还要确定处理 event 的接口的形式
// event 应该有很多种，这样会方便一点
// 我们在基类提供一个默认操作

class Event;

Event* poll_event();