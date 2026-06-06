// 如果我们统一了 desc，那么就必须要表达出 shader 这个级别的语义
// 换句话说，我们得有一个 dsl，让 executor 能翻译成不同 backend
// 这个工作有点太 heavy 了，先用一个 shader wrapper 包一下，这里可以搞丑一点
