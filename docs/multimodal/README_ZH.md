# 多模态文档总览

本目录包含 llama.cpp 支持的多模态模型的文档。

## 简介

llama.cpp 支持多种多模态大语言模型（Multimodal LLM），这些模型能够处理和生成多种类型的数据，包括文本、图像等。

## 支持的多模态模型

| 模型 | 文档 | 说明 |
|------|------|------|
| LLaVA | [llava.md](./llava.md) | 大语言和视觉助手 |
| MiniCPM-V 系列 | 多个文档 | 多种 MiniCPM-V 模型变体 |
| Granite Vision | [granitevision.md](./granitevision.md) | IBM 的视觉语言模型 |
| GLM-Edge | [glmedge.md](./glmedge.md) | GLM 边缘多模态模型 |
| MobileVLM | [MobileVLM.md](./MobileVLM.md) | 移动端视觉语言模型 |
| Gemma 3 | [gemma3.md](./gemma3.md) | Google Gemma 多模态版本 |

## 模型文档

### LLaVA
[llava.md](./llava.md)

LLaVA (Large Language and Vision Assistant) 是一个开源的多模态模型，结合了视觉编码器和语言模型。

**特点：**
- 强大的视觉理解能力
- 支持图像描述、问答
- 良好的指令跟随能力

**使用方法：**
```bash
./llama-cli \
  --model llava-v1.5-7b-q4.gguf \
  --mmproj llava-v1.5-7b-mmproj.gguf \
  --image path/to/image.jpg \
  --prompt "Describe this image"
```

### MiniCPM-V 系列

llama.cpp 支持多个 MiniCPM-V 模型变体：

- **minicpmv2.5.md** - MiniCPM-V 2.5 版本
- **minicpmv2.6.md** - MiniCPM-V 2.6 版本
- **minicpmv4.0.md** - MiniCPM-V 4.0 版本
- **minicpmv4.5.md** - MiniCPM-V 4.5 版本
- **minicpmv4.6.md** - MiniCPM-V 4.6 版本
- **minicpmo2.6.md** - MiniCPM-O 2.6 版本（优化版）
- **minicpmo4.0.md** - MiniCPM-O 4.0 版本（优化版）

**特点：**
- 高效的移动端性能
- 多种尺寸和配置
- 优秀的边缘设备推理能力

### Granite Vision
[granitevision.md](./granitevision.md)

IBM Granite Vision 系列多模态模型。

**特点：**
- 企业级视觉理解
- 文档分析
- 场景理解

### GLM-Edge
[glmedge.md](./glmedge.md)

GLM-Edge 是专为边缘设备设计的多模态模型。

**特点：**
- 低资源消耗
- 快速推理
- 适合移动和边缘部署

### MobileVLM
[MobileVLM.md](./MobileVLM.md)

专为移动设备优化的视觉语言模型。

**特点：**
- 极低的延迟
- 小模型尺寸
- 适合移动应用集成

### Gemma 3
[gemma3.md](./gemma3.md)

Google Gemma 3 多模态版本。

**特点：**
- Google 最新多模态技术
- 优秀的性能质量比
- 广泛的应用场景

## 使用多模态模型

### 1. 准备模型

多模态模型通常需要两个文件：

1. **主模型文件** - 包含语言模型和融合层
2. **视觉投影器** - 处理图像编码

```bash
# 示例：LLaVA 模型
llava-v1.5-7b-q4.gguf          # 主模型
llava-v1.5-7b-mmproj.gguf      # 视觉投影器
```

### 2. 运行推理

使用 CLI 工具：

```bash
./llama-cli \
  --model <model-file>.gguf \
  --mmproj <mmproj-file>.gguf \
  --image path/to/image.jpg \
  --prompt "What do you see in this image?"
```

### 3. 批量处理

处理多张图片：

```bash
./llama-cli \
  --model model.gguf \
  --mmproj mmproj.gguf \
  --image img1.jpg \
  --image img2.jpg \
  --prompt "Analyze these images"
```

## 支持的功能

### 1. 图像理解

- 描述图像内容
- 回答关于图像的问题
- 识别图像中的对象

### 2. 图像对话

- 多轮对话
- 上下文理解
- 图像推理

### 3. 多图像处理

- 图像比较
- 图像序列分析
- 多图像综合分析

## 性能优化

### 1. 模型选择

根据设备和需求选择合适的模型：

- **移动端**：MobileVLM, MiniCPM-V
- **桌面端**：LLaVA, Gemma 3
- **服务器端**：Granite Vision, LLaVA-Large

### 2. 量化

使用量化模型减少内存占用：

```bash
# Q4 量化 - 平衡性能和质量
./llama-cli -m model-q4.gguf --mmproj mmproj.gguf ...

# Q2 量化 - 最低内存占用
./llama-cli -m model-q2.gguf --mmproj mmproj.gguf ...
```

### 3. 硬件加速

使用 GPU 加速图像处理：

```bash
# CUDA
./llama-cli -m model.gguf --mmproj mmproj.gguf -ngl 99 ...

# Metal (macOS)
./llama-cli -m model.gguf --mmproj mmproj.gguf -ngl 99 ...
```

## 常见问题

### Q: 多模态模型需要特殊的硬件吗？

A: 不需要，但 GPU 加速可以显著提升推理速度，特别是图像处理部分。

### Q: 可以使用自己的图像编码器吗？

A: 当前只支持模型自带的视觉投影器，自定义需要修改代码。

### Q: 支持视频输入吗？

A: 目前主要通过提取视频帧来处理，需要将视频转换为图像序列。

### Q: 如何处理高分辨率图像？

A: 高分辨率图像会被缩放到模型支持的尺寸，建议预处理图像。

## 开发指南

### 添加新多模态模型支持

1. **定义模型结构**
   - 在 `ggml` 中添加新的层类型
   - 实现图像编码接口

2. **实现视觉投影器**
   - 定义投影器架构
   - 实现前向传播

3. **添加模型加载器**
   - 支持 GGUF 格式
   - 加载模型参数

4. **创建文档**
   - 模型使用说明
   - 性能特征
   - 示例代码

### 调试多模态模型

```bash
# 启用详细日志
./llama-cli -m model.gguf --mmproj mmproj.gguf \
  --image test.jpg --log-level debug ...
```

## 相关资源

- [GBNF 语法](../../grammars/README_ZH.md) - 语法约束
- [后端文档](../backend/README_ZH.md) - 硬件加速
- [开发文档](../development/README_ZH.md) - 开发指南
- [文档目录总览](../README_ZH.md)

## 示例应用

### 1. 图像描述生成

```bash
./llama-cli \
  --model llava-v1.5-7b-q4.gguf \
  --mmproj llava-v1.5-7b-mmproj.gguf \
  --image photo.jpg \
  --prompt "Describe this image in detail"
```

### 2. 视觉问答

```bash
./llama-cli \
  --model llava-v1.5-7b-q4.gguf \
  --mmproj llava-v1.5-7b-mmproj.gguf \
  --image chart.png \
  --prompt "What is the trend shown in this chart?"
```

### 3. OCR 任务

```bash
./llama-cli \
  --model llava-v1.5-7b-q4.gguf \
  --mmproj llava-v1.5-7b-mmproj.gguf \
  --image document.png \
  --prompt "Extract all text from this document"
```

## 贡献

欢迎贡献新的多模态模型支持。请：

1. 参考现有模型实现
2. 添加完整文档
3. 提供测试用例
4. 包含性能基准