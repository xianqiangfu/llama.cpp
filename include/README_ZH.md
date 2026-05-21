# 公共头文件目录说明

本目录包含 llama.cpp 的公共 C/C++ API 接口定义。

## 文件说明

### llama.h
主要的公共 C API 接口文件，定义了：

- **核心数据类型**
  - `llama_model` - 模型结构
  - `llama_context` - 执行上下文
  - `llama_vocab` - 词汇表
  - `llama_sampler` - 采样器接口

- **模型管理**
  - 模型加载/保存函数
  - 模型参数查询
  - 多设备支持

- **推理执行**
  - `llama_encode()` - 编码器推理（用于编码器-解码器模型）
  - `llama_decode()` - 解码器推理
  - 批处理接口

- **词表操作**
  - `llama_tokenize()` - 文本转 Token
  - `llama_detokenize()` - Token 转文本
  - 特殊 Token 处理

- **采样 API**
  - 多种采样策略（Top-K、Top-P、Temperature 等）
  - Mirostat 算法
  - 语法约束（GBNF）
  - DRY 重复惩罚

- **状态管理**
  - 会话保存/加载
  - KV 缓存管理

- **适配器支持**
  - LoRA 适配器加载和应用
  - 控制向量支持

### llama-cpp.h
C++ 包装接口，提供更简洁的 C++ API。

## API 版本控制

使用 `LLAMA_SHARED` 宏控制导出/导入符号：
- Windows: `__declspec(dllexport/dllimport)`
- 其他: `__attribute__((visibility("default")))`

## 量化类型

支持的量化格式（通过 `enum llama_ftype`）：
- 全精度（F32、BF16）
- 半精度（F16）
- 各种 Q4 格式（Q4_0、Q4_K、Q4_K_S、Q4_K_M 等）
- 各种 Q5/Q6/Q8 格式
- IQ 系列（IQ1_S、IQ2_XS、IQ3_XXS 等）
- TQ 系列（TQ1_0、TQ2_0）
- NVFP4（NVIDIA 4-bit 浮点）
- MXFP4（混合精度 4-bit 浮点）

## 支持的词表类型

| 类型 | 说明 |
|------|------|
| `LLAMA_VOCAB_TYPE_SPM` | SentencePiece（LLaMA 风格） |
| `LLAMA_VOCAB_TYPE_BPE` | 字节级 BPE（GPT-2 风格） |
| `LLAMA_VOCAB_TYPE_WPM` | WordPiece（BERT 风格） |
| `LLAMA_VOCAB_TYPE_UGM` | Unigram（T5 风格） |
| `LLAMA_VOCAB_TYPE_RWKV` | RWKV 贪婪分词 |
| `LLAMA_VOCAB_TYPE_PLAMO2` | PLaMo-2 动态规划分词 |

## RoPE 缩放类型

| 类型 | 说明 |
|------|------|
| `LLAMA_ROPE_SCALING_TYPE_NONE` | 无缩放 |
| `LLAMA_ROPE_SCALING_TYPE_LINEAR` | 线性缩放 |
| `LLAMA_ROPE_SCALING_TYPE_YARN` | YaRN 缩放 |
| `LLAMA_ROPE_SCALING_TYPE_LONGROPE` | LongRoPE 缩放 |

## Flash Attention

支持多种 Flash Attention 模式：
- `LLAMA_FLASH_ATTN_TYPE_AUTO` - 自动选择
- `LLAMA_FLASH_ATTN_TYPE_DISABLED` - 禁用
- `LLAMA_FLASH_ATTN_TYPE_ENABLED` - 启用

## 相关文档

- [../src/README_ZH.md](../src/README_ZH.md) - 核心源代码说明
- [../AGENTS.md](../AGENTS.md) - Agent 工作流程

## 使用示例

详细的 API 使用示例请参考项目 `examples/` 目录下的示例程序。