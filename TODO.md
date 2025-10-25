# code refine
- 设置 const method
- 区分报错场景：
    1. 先让所有报错都用 CHECK
    2. tex load 之类的逻辑可能是可以补救的，不应该直接 CHECK 报错，这里应该检查好