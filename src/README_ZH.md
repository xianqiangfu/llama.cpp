# 核心源代码目录总览

本目录包含 llama.cpp 的核心源代码实现。

## 目录结构

```
src/
├── llama-*.cpp/h         # 核心实现文件
├── models/               # 模型实现目录（详见 models/README_ZH.md）
└── ...
```

## 核心模块说明

### 架构与模型管理

| 文件 | 说明 |
|------|------|
| `llama-arch.cpp/h` | 模型架构定义和管理，支持 140+ 种不同架构 |
| `llama-context.cpp/h` | 上下文管理，处理模型运行状态和计算图 |
| `llama-graph.cpp/h` | 计算图构建和管理 |
| `llama-hparams.cpp/h` | 超参数管理 |
| `llama-cparams.cpp/h` | 计算参数管理 |

### 批处理与解码

| 文件 | 说明 |
|------|------|
| `llama-batch.cpp/h` | 批处理逻辑，支持多序列并行处理 |
| `llama-grammar.cpp/h` | GBNF 语法约束实现 |

### KV 缓存

| 文件 | 说明 |
|------|------|
| `llama-kv-cache.cpp/h` | KV 键值缓存管理 |
| `llama-kv-cache-iswa.cpp/h` | ISWA (In-Sliding-Window Attention) 缓存优化 |

### 适配器与扩展

| 文件 | 说明 |
|------|------|
| `llama-adapter.cpp/h` | 适配器支持（LoRA、控制向量等） |
| `llama-chat.cpp/h` | 聊天模板和对话格式化 |

### 其他组件

| 文件 | 说明 |
|------|------|
| `llama-impl.cpp/h` | 内部实现辅助 |
| `llama-ext.h` | 扩展接口定义 |
| `llama-io.cpp/h` | I/O 操作 |

## 支持的模型架构

本模块支持 140+ 种不同的模型架构，包括但不限于：

- **Transformer 系列**: LLaMA、Qwen、Gemma、Mistral 等
- **BERT 系列**: BERT、ModernBERT、NomicBERT 等
- **混合架构**: Jamba、Granite、Hunyuan MOE 等
- **循环架构**: Mamba、Mamba2、RWKV 等
- **扩散模型**: LLADA、Dream 等
- **视觉语言模型**: CLIP、Chameleon、CogVLM 等

完整的架构列表请参考 `llama-arch.h` 中的 `llm_arch` 枚举。

## 关键特性

### 多后端支持
- CPU（通过 GGML）
- CUDA（NVIDIA GPU）
- Metal（Apple Silicon）
- ROCm（AMD GPU）
- OpenCL、Vulkan、SYCL 等

### 高级功能
- 量化支持（F16、Q4、Q8 等）
- Flash Attention 优化
- LoRA 适配器
- 语法约束生成（GBNF）
- Lookahead 解码
- 多模态输入处理

## 相关文档

- [models/README_ZH.md](models/README_ZH.md) - 模型实现详细说明
- [../include/llama.h](../include/llama.h) - C API 接口文档
- [../AGENTS.md](../AGENTS.md) - Agent 工作流程