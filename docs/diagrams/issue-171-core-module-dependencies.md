# 核心模块依赖关系图

```mermaid
graph TD
    A[include/llama.h<br/>公共API接口] --> B[src/llama.cpp<br/>核心接口实现]
    A --> C[src/llama-context.h<br/>上下文管理]
    A --> D[src/llama-model.h<br/>模型管理]
    A --> E[src/llama-sampler.h<br/>采样器]
    A --> F[src/llama-adapter.h<br/>适配器管理]
    A --> G[src/llama-grammar.h<br/>语法约束]

    B --> H[ggml/include/ggml.h<br/>基础计算图]
    B --> I[ggml/include/ggml-backend.h<br/>后端接口]
    B --> J[ggml/include/gguf.h<br/>模型格式]

    C --> K[src/llama-memory.h<br/>内存管理接口]
    C --> L[src/llama-kv-cache.h<br/>KV缓存]
    C --> M[src/llama-batch.h<br/>批次处理]

    D --> N[src/llama-model-loader.h<br/>模型加载器]
    D --> O[src/llama-arch.h<br/>架构支持]
    D --> P[src/llama-vocab.h<br/>词汇表]

    E --> Q[src/llama-sampler.h<br/>采样器实现]

    F --> R[src/llama-adapter.h<br/>LoRA/控制向量]
    F --> S[src/llama-adapter-cvec.h<br/>控制向量实现]

    G --> T[src/llama-grammar.h<br/>GBNF解析]

    K --> U[src/llama-memory.h<br/>内存类型实现]
    L --> V[src/llama-kv-cache.h<br/>KV缓存实现]
    M --> W[src/llama-batch.h<br/>批次分配器]

    H --> X[ggml/src/ggml.c<br/>核心计算]
    I --> Y[ggml/src/ggml-backend.c<br/>后端实现]

    U --> Z[src/llama-memory-hybrid.h<br/>混合内存]
    U --> AA[src/llama-memory-recurrent.h<br/>循环内存]
    U --> AB[src/llama-memory-hybrid-iswa.h<br/>iSWA内存]

    V --> AC[src/llama-kv-cache-iswa.h<br/>iSWA缓存]
```

## 核心模块说明

### 1. 公共接口层
- **llama.h**: C语言公共API接口
- 提供模型加载、推理、采样等功能

### 2. 核心实现层
- **llama.cpp**: 主要接口实现
- **llama-context.h**: 推理上下文管理
- **llama-model.h**: 模型结构和加载

### 3. 功能模块层
- **llama-sampler**: 采样器和采样链
- **llama-adapter**: LoRA 适配器和控制向量
- **llama-grammar**: GBNF 语法约束
- **llama-batch**: 批次处理和分配

### 4. 内存管理层
- **llama-memory**: 内存管理接口
- **llama-kv-cache**: KV 缓存管理
- 支持多种内存类型（混合、循环、iSWA）

### 5. 依赖基础
- **ggml**: 基础计算图和操作
- **ggml-backend**: 后端接口实现
- **gguf**: 模型文件格式