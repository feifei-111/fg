// 统一管理 res desc 的工具
// 一份数据可能有几个层次
//   1. path
//   2. cpu 存储数据
//   3. gpu 存储数据
// 在资源紧张的时候切换
// 应该有一个 id，方便序列化 pipeline desc
// id 直接用 path 是不是就行了
