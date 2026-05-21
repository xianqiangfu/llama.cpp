# LoRA 适配器应用逻辑图

```mermaid
flowchart TD
    A[开始 LoRA 适配] --> B[加载 LoRA 文件]
    B --> C{检查模型类型}
    C -->|LLaMA| D[解析 LoRA 权重]
    C -->|其他架构| E[适配特定格式]
    D --> F[创建 LoRA 上下文]
    E --> F
    F --> G{检查层映射}
    G -->|有映射| H[应用 LoRA 到指定层]
    G -->|无映射| I[应用 LoRA 到所有兼容层]
    H --> J[计算缩放因子]
    I --> J
    J --> K[alpha = adapter_scale * alpha / rank]
    K --> L[构建 LoRA 节点]
    L --> M[A 矩阵投影]
    M --> N[B 矩阵投影]
    N --> O[缩放操作]
    O --> P[与原始权重相加]
    P --> Q[更新计算图]
    Q --> R{是否 aLoRA?}
    R -->|是| S[解析调用token]
    R -->|否| T[完成适配]
    S --> U{匹配调用token?}
    U -->|是| V[激活对应 LoRA]
    U -->|否| W[跳过该 LoRA]
    V --> T
    W --> T
    T --> X[结束]

    subgraph LoRA 结构
        Y[LoRA 适配器] --> Z[A 矩阵<br/>d_model × rank]
        Y --> AA[B 矩阵<br/>rank × d_model]
        Y --> AB[alpha 缩放参数]
        Z --> AC[降维投影]
        AA --> AD[升维投影]
        AB --> AE[动态缩放]
    end

    M -.-> Z
    N -.-> AA
    J -.-> AB
```

## 核心组件

### 1. LoRA 适配器结构
- **A 矩阵**: 降维投影 (d_model × rank)
- **B 矩阵**: 升维投影 (rank × d_model)
- **alpha**: 缩放参数

### 2. 应用流程
- **权重加载**: 从 GGUF 文件加载 LoRA 权重
- **层映射**: 将 LoRA 应用到指定层
- **缩放计算**: 根据 alpha 和 rank 动态计算缩放因子

### 3. aLoRA 机制
- 支持 token 级别的 LoRA 激活
- 通过调用 token 动态切换 LoRA
- 实现自适应适配器选择