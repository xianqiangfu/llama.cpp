# 多模态文档摘要 (multimodal.md)

## 概述

llama.cpp 通过 `libmtmd` 库支持多模态输入，包括图像和音频输入。目前主要通过两个工具提供此功能：
- `llama-mtmd-cli`：命令行工具
- `llama-server`：通过 OpenAI 兼容的 `/chat/completions` API

**注意**：音频输入功能目前处于实验阶段，质量可能有所限制。

## 多模态输入的处理机制

### 启用方法

有两种主要方式启用多模态功能：

#### 方法 1：使用 `-hf` 选项加载支持的模型

```bash
# 启用多模态（默认）
llama-server -hf ggml-org/gemma-3-4b-it-GGUF

# 禁用多模态
llama-server -hf ggml-org/gemma-3-4b-it-GGUF --no-mmproj

# 使用自定义 mmproj 文件
llama-server -hf ggml-org/gemma-3-4b-it-GGUF --mmproj local_file.gguf
```

#### 方法 2：使用 `-m` 和 `--mmproj` 选项分别指定文本模型和多模态投影器

```bash
llama-server -m gemma-3-4b-it-Q4_K_M.gguf --mmproj mmproj-gemma-3-4b-it-Q4_K_M.gguf
```

### GPU 卸载控制

默认情况下，多模态投影器会卸载到 GPU。如需禁用：

```bash
llama-server -hf ggml-org/gemma-3-4b-it-GGUF --no-mmproj-offload
```

### 处理流程

1. **模型加载**：同时加载主模型和多模态投影器
2. **输入处理**：将图像/音频数据转换为模型可处理的格式
3. **特征提取**：通过投影器提取多模态特征
4. **融合处理**：将多模态特征与文本输入融合
5. **生成输出**：基于融合后的上下文生成响应

## 多模态模型的加载方法

### 预量化模型

llama.cpp 提供了大量预量化的多模态模型，大多数使用 `Q4_K_M` 量化。这些模型可以在 ggml-org 的 Hugging Face 页面找到：
https://huggingface.co/collections/ggml-org/multimodal-ggufs-68244e01ff1f39e5bebeeedc

**重要提示**：某些模型可能需要较大的上下文窗口，例如 `-c 8192`。

### 视觉模型

#### Gemma 3 系列

```bash
# 4B 参数模型
llama-mtmd-cli -hf ggml-org/gemma-3-4b-it-GGUF
llama-server -hf ggml-org/gemma-3-4b-it-GGUF

# 12B 参数模型
llama-mtmd-cli -hf ggml-org/gemma-3-12b-it-GGUF
llama-server -hf ggml-org/gemma-3-12b-it-GGUF

# 27B 参数模型
llama-mtmd-cli -hf ggml-org/gemma-3-27b-it-GGUF
llama-server -hf ggml-org/gemma-3-27b-it-GGUF
```

#### SmolVLM 系列

```bash
# SmolVLM 主模型
llama-mtmd-cli -hf ggml-org/SmolVLM-Instruct-GGUF
llama-mtmd-cli -hf ggml-org/SmolVLM-256M-Instruct-GGUF
llama-mtmd-cli -hf ggml-org/SmolVLM-500M-Instruct-GGUF

# SmolVLM2 系列
llama-mtmd-cli -hf ggml-org/SmolVLM2-2.2B-Instruct-GGUF
llama-mtmd-cli -hf ggml-org/SmolVLM2-256M-Video-Instruct-GGUF
llama-mtmd-cli -hf ggml-org/SmolVLM2-500M-Video-Instruct-GGUF
```

#### Pixtral 系列

```bash
# Pixtral 12B
llama-mtmd-cli -hf ggml-org/pixtral-12b-GGUF
llama-server -hf ggml-org/pixtral-12b-GGUF
```

#### Qwen 2 VL 系列

```bash
# Qwen2-VL-2B
llama-mtmd-cli -hf ggml-org/Qwen2-VL-2B-Instruct-GGUF

# Qwen2-VL-7B
llama-mtmd-cli -hf ggml-org/Qwen2-VL-7B-Instruct-GGUF
```

#### Qwen 2.5 VL 系列

```bash
llama-mtmd-cli -hf ggml-org/Qwen2.5-VL-3B-Instruct-GGUF
llama-mtmd-cli -hf ggml-org/Qwen2.5-VL-7B-Instruct-GGUF
llama-mtmd-cli -hf ggml-org/Qwen2.5-VL-32B-Instruct-GGUF
llama-mtmd-cli -hf ggml-org/Qwen2.5-VL-72B-Instruct-GGUF
```

#### InternVL 系列

```bash
# InternVL 2.5
llama-mtmd-cli -hf ggml-org/InternVL2_5-1B-GGUF
llama-mtmd-cli -hf ggml-org/InternVL2_5-4B-GGUF

# InternVL 3
llama-mtmd-cli -hf ggml-org/InternVL3-1B-Instruct-GGUF
llama-mtmd-cli -hf ggml-org/InternVL3-2B-Instruct-GGUF
llama-mtmd-cli -hf ggml-org/InternVL3-8B-Instruct-GGUF
llama-mtmd-cli -hf ggml-org/InternVL3-14B-Instruct-GGUF
```

#### 其他视觉模型

```bash
# Mistral Small 3.1 24B (IQ2_M 量化)
llama-mtmd-cli -hf ggml-org/Mistral-Small-3.1-24B-Instruct-2503-GGUF

# Llama 4 Scout
llama-mtmd-cli -hf ggml-org/Llama-4-Scout-17B-16E-Instruct-GGUF

# Moondream2 (20250414 版本)
llama-mtmd-cli -hf ggml-org/moondream2-20250414-GGUF

# Gemma 4
llama-mtmd-cli -hf ggml-org/gemma-4-E2B-it-GGUF
llama-mtmd-cli -hf ggml-org/gemma-4-E4B-it-GGUF
llama-mtmd-cli -hf ggml-org/gemma-4-26B-A4B-it-GGUF
llama-mtmd-cli -hf ggml-org/gemma-4-31B-it-GGUF
```

### 音频模型

#### Ultravox 0.5

```bash
# Ultravox-v0.5 Llama 3.2 1B
llama-mtmd-cli -hf ggml-org/ultravox-v0_5-llama-3_2-1b-GGUF

# Ultravox-v0.5 Llama 3.1 8B
llama-mtmd-cli -hf ggml-org/ultravox-v0_5-llama-3_1-8b-GGUF
```

#### Mistral's Voxtral

```bash
# Voxtral-Mini 3B
llama-mtmd-cli -hf ggml-org/Voxtral-Mini-3B-2507-GGUF
```

#### Qwen3-ASR

```bash
# Qwen3-ASR 0.6B
llama-mtmd-cli -hf ggml-org/Qwen3-ASR-0.6B-GGUF

# Qwen3-ASR 1.7B
llama-mtmd-cli -hf ggml-org/Qwen3-ASR-1.7B-GGUF
```

**注意**：Qwen2-Audio 和 SeaLLM-Audio 没有预量化的 GGUF 模型，因为效果较差。

### 混合模态模型（音频 + 视觉）

#### Qwen2.5 Omni

```bash
# Qwen2.5-Omni-3B
llama-mtmd-cli -hf ggml-org/Qwen2.5-Omni-3B-GGUF

# Qwen2.5-Omni-7B
llama-mtmd-cli -hf ggml-org/Qwen2.5-Omni-7B-GGUF
```

#### Qwen3 Omni

```bash
# Qwen3-Omni-30B-A3B-Instruct
llama-mtmd-cli -hf ggml-org/Qwen3-Omni-30B-A3B-Instruct-GGUF

# Qwen3-Omni-30B-A3B-Thinking
llama-mtmd-cli -hf ggml-org/Qwen3-Omni-30B-A3B-Thinking-GGUF
```

#### Gemma 4（音频 + 视觉）

```bash
llama-mtmd-cli -hf ggml-org/gemma-4-E2B-it-GGUF
llama-mtmd-cli -hf ggml-org/gemma-4-E4B-it-GGUF
```

## 多模态使用示例

### 命令行使用

#### 基本用法

```bash
# 使用 HuggingFace 模型
llama-mtmd-cli -hf ggml-org/gemma-3-4b-it-GGUF

# 使用本地文件
llama-mtmd-cli -m gemma-3-4b-it-Q4_K_M.gguf --mmproj mmproj-gemma-3-4b-it-Q4_K_M.gguf

# 禁用 GPU 卸载
llama-mtmd-cli -hf ggml-org/gemma-3-4b-it-GGUF --no-mmproj-offload
```

### 服务器使用

#### 基本用法

```bash
# 使用 HuggingFace 模型
llama-server -hf ggml-org/gemma-3-4b-it-GGUF

# 使用本地文件
llama-server -m gemma-3-4b-it-Q4_K_M.gguf --mmproj mmproj-gemma-3-4b-it-Q4_K_M.gguf

# 禁用 GPU 卸载
llama-server -hf ggml-org/gemma-3-4b-it-GGUF --no-mmproj-offload

# 指定端口和主机
llama-server -hf ggml-org/gemma-3-4b-it-GGUF --port 8080 --host 0.0.0.0
```

#### OpenAI 兼容 API

llama-server 通过 `/chat/completions` API 端点提供 OpenAI 兼容的多模态支持。

### OCR 模型特殊注意事项

OCR 模型使用特定的提示词和输入结构进行训练，使用时需要特别注意：

- **PaddleOCR-VL**：https://github.com/ggml-org/llama.cpp/pull/18825
- **GLM-OCR**：https://github.com/ggml-org/llama.cpp/pull/19677
- **Deepseek-OCR**：https://github.com/ggml-org/llama.cpp/pull/17400
- **Dots.OCR**：https://github.com/ggml-org/llama.cpp/pull/17575
- **HunyuanOCR**：https://github.com/ggml-org/llama.cpp/pull/21395

建议在使用这些 OCR 模型前，先阅读相关的 PR 讨论以了解正确的使用方法。

## 模型选择指南

### 视觉任务

| 用途 | 推荐模型 | 特点 |
|------|---------|------|
| 通用图像理解 | Gemma 3-4B/12B | 平衡性能和速度 |
| 高端图像理解 | Gemma 3-27B | 最佳质量 |
| 轻量级应用 | SmolVLM-256M/500M | 最小资源占用 |
| 视频理解 | SmolVLM2-Video | 支持视频输入 |
| 文档理解 | InternVL 系列 | 专业 OCR 能力 |

### 音频任务

| 用途 | 推荐模型 | 特点 |
|------|---------|------|
| 语音识别 | Qwen3-ASR | 专业的 ASR 能力 |
| 音频理解 | Ultravox | 音频-文本融合 |
| 高端音频任务 | Voxtral | 最强音频处理 |

### 混合模态任务

| 用途 | 推荐模型 | 特点 |
|------|---------|------|
| 多模态对话 | Qwen2.5-Omni | 音频+视觉+文本 |
| 复杂推理 | Qwen3-Omni | 30B 参数，强大能力 |
| 通用多模态 | Gemma 4 | 音频+视觉支持 |

## 性能优化建议

### 上下文窗口配置

```bash
# 大型模型可能需要更大的上下文
llama-server -hf ggml-org/gemma-3-27b-it-GGUF -c 8192
```

### GPU 卸载策略

- **默认启用**：多模态投影器默认卸载到 GPU
- **显存受限**：使用 `--no-mmproj-offload` 限制 GPU 使用
- **分层卸载**：使用 `-ngl` 参数控制卸载层数

### 量化选择

- **Q4_K_M**：默认推荐，平衡质量和速度
- **IQ2_M**：极端压缩，适合内存受限场景
- **Q8_0**：最高质量，适合性能优先场景

## 故障排除

### 常见问题

1. **模型加载失败**
   - 检查模型和 mmproj 文件是否匹配
   - 确保有足够的内存
   - 验证文件完整性

2. **性能不佳**
   - 启用 GPU 卸载
   - 调整上下文大小
   - 使用合适的量化级别

3. **输出质量低**
   - 尝试更大的模型
   - 检查输入格式
   - 调整提示词

## 查找更多模型

Hugging Face 上支持视觉功能的 GGUF 模型：
https://huggingface.co/models?pipeline_tag=image-text-to-text&sort=trending&search=gguf

## 结论

llama.cpp 的多模态支持提供了丰富的图像和音频处理能力。通过选择合适的模型、配置正确的参数，可以在各种应用场景中实现高效的多模态推理。建议根据具体需求和资源限制选择最适合的模型和配置方案。