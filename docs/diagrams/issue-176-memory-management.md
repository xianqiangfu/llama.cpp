# 内存管理架构图

```mermaid
graph TD
    A[llama_memory_i<br/>内存接口] --> B[内存类型实现]
    B --> C[llama_kv_cache<br/>标准 KV 缓存]
    B --> D[llama_kv_cache_iswa<br/>iSWA 缓存]
    B --> E[llama_memory_hybrid<br/>混合内存]
    B --> F[llama_memory_recurrent<br/>循环内存]
    B --> G[llama_memory_hybrid_iswa<br/>混合 iSWA]

    C --> H[内存上下文接口]
    D --> H
    E --> H
    F --> H
    G --> H

    H --> I[init_batch<br/>批次初始化]
    H --> J[init_full<br/>完整缓存模拟]
    H --> K[init_update<br/>更新准备]
    H --> L[apply<br/>应用更新]

    I --> M[批次分割器]
    M --> N[llama_ubatch<br/>微批次]

    L --> O[内存操作]
    O --> P[seq_rm<br/>删除序列]
    O --> Q[seq_cp<br/>复制序列]
    O --> R[seq_keep<br/>保留序列]
    O --> S[seq_add<br/>添加序列]
    O --> T[seq_div<br/>分割序列]

    subgraph 缓冲区管理
        U[ggml_backend_buffer_type_t<br/>缓冲区类型]
        V[ggml_backend_buffer_t<br/>缓冲区实例]
        W[ggml_allocr_t<br/>分配器]
    end

    C --> U
    D --> U
    E --> U
    F --> U
    G --> U

    U --> V
    V --> W

    subgraph 内存类型
        X[ggml_type_k<br/>Key 类型]
        Y[ggml_type_v<br/>Value 类型]
        Z[swa_full<br/>完整 SWA]
    end

    C --> X
    C --> Y
    C --> Z

    D --> X
    D --> Y
    D --> Z

    E --> X
    E --> Y
    E --> Z

    subgraph 内存状态
        AA[LLAMA_MEMORY_STATUS_SUCCESS]
        AB[LLAMA_MEMORY_STATUS_NO_UPDATE]
        AC[LLAMA_MEMORY_STATUS_FAILED_PREPARE]
        AD[LLAMA_MEMORY_STATUS_FAILED_COMPUTE]
    end

    H --> AA
    H --> AB
    H --> AC
    H --> AD

    subgraph 内存分配策略
        AE[内存池]
        AF[动态分配]
        AG[预分配]
    end

    W --> AE
    W --> AF
    W --> AG
```

## 内存管理说明

### 1. 内存接口
- **llama_memory_i**: 统一的内存管理接口
- 定义所有内存类型必须实现的方法

### 2. 内存类型
- **标准 KV 缓存**: 基础键值缓存
- **iSWA 缓存**: 滑动窗口注意力缓存
- **混合内存**: 组合多种内存类型
- **循环内存**: 支持循环网络结构
- **混合 iSWA**: 混合滑动窗口缓存

### 3. 内存上下文
- **init_batch**: 初始化批次处理
- **init_full**: 模拟完整缓存
- **init_update**: 准备内存更新
- **apply**: 应用内存状态

### 4. 批次处理
- **批次分割器**: 将批次分割为微批次
- **llama_ubatch**: 微批次结构
- 支持多种分割策略

### 5. 序列操作
- **seq_rm**: 删除指定序列
- **seq_cp**: 复制序列数据
- **seq_keep**: 保留序列
- **seq_add**: 添加新序列
- **seq_div**: 分割序列

### 6. 缓冲区管理
- **缓冲区类型**: 定义内存类型
- **缓冲区实例**: 实际内存缓冲区
- **分配器**: 内存分配策略

### 7. 内存状态
- **SUCCESS**: 操作成功
- **NO_UPDATE**: 无需更新
- **FAILED_PREPARE**: 准备失败
- **FAILED_COMPUTE**: 计算失败

### 8. 分配策略
- **内存池**: 预分配内存池
- **动态分配**: 按需分配
- **预分配**: 提前分配所需内存