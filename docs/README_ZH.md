# 文档目录总览

本目录包含 llama.cpp 项目的各种文档，涵盖安装、使用、开发和运维等方面。

## 目录结构

```
docs/
├── android/          # Android 平台相关文档
├── backend/          # 各硬件后端文档
├── development/      # 开发相关文档
├── multimodal/       # 多模态支持文档
├── ops/              # 运维相关文档
└── *.md              # 各种主题的独立文档
```

## 文档分类

### 平台相关

#### [Android](./android/README_ZH.md)
Android 平台集成指南
- 在 Android Studio 中集成 llama.cpp
- 移动端部署和优化

详细说明请参见 [Android 文档](./android/README_ZH.md)。

### 后端文档

#### [后端总览](./backend/README_ZH.md)
各硬件后端的使用说明：

- **CUDA** - NVIDIA GPU 支持
- **Metal** - Apple Metal 支持
- **Vulkan** - 跨平台 GPU 支持
- **OpenCL** - OpenCL 支持
- **WebGPU** - 浏览器/Web 支持
- **SYCL** - Intel oneAPI 支持
- **BLIS** - BLIS 库集成
- **CANN** - 华为昇腾 NPU 支持
- **ZenDNN** - AMD ZenDNN 支持
- **VirtGPU** - 虚拟 GPU 支持

详细说明请参见 [后端文档](./backend/README_ZH.md)。

### 开发文档

#### [开发总览](./development/README_ZH.md)
为开发者提供的指南：

- 添加新模型支持
- 测试和调试
- 解析器开发
- Token 生成性能优化

详细说明请参见 [开发文档](./development/README_ZH.md)。

### 多模态支持

#### [多模态总览](./multimodal/README_ZH.md)
多模态模型支持文档：

- LLaVA
- MiniCPM-V
- Granite Vision
- GLM-Edge
- MobileVLM
- 以及其他多模态模型

详细说明请参见 [多模态文档](./multimodal/README_ZH.md)。

### 运维文档

#### [运维总览](./ops/README_ZH.md)
运维相关文档：

- 服务器部署
- 性能监控
- 故障排查
- 基准测试

详细说明请参见 [运维文档](./ops/README_ZH.md)。

### 核心文档

#### [build.md](build.md)
构建指南
- 构建选项和配置
- 依赖库安装
- 跨平台构建

#### [build-riscv64-spacemit.md](build-riscv64-spacemit.md)
RISC-V 架构构建说明

#### [build-s390x.md](build-s390x.md)
IBM S/390x 架构构建说明

#### [install.md](install.md)
安装指南

#### [docker.md](docker.md)
Docker 部署指南

#### [multi-gpu.md](multi-gpu.md)
多 GPU 部署指南

### 功能文档

#### [function-calling.md](function-calling.md)
函数调用功能说明

#### [speculative.md](speculative.md)
推测性解码 (Speculative Decoding) 优化

#### [llguidance.md](llguidance.md)
LLM Guidance 语法约束

#### [autoparser.md](autoparser.md)
自动解析器功能

#### [presets.md](preset.md)
预设配置说明

### 其他文档

#### [android.md](android.md)
Android 平台使用说明

#### [multimodal.md](multimodal.md)
多模态功能概述

#### [ops.md](ops.md)
运维详细指南

## 快速导航

### 新手入门
1. 阅读 [install.md](install.md) 了解安装方法
2. 查看 [build.md](build.md) 进行构建
3. 选择合适的 [后端](./backend/README_ZH.md)

### 模型开发者
1. 查看 [开发文档](./development/README_ZH.md)
2. 阅读 [添加模型指南](./development/HOWTO-add-model.md)
3. 了解 [测试和调试](./development/debugging-tests.md)

### 运维人员
1. 参考 [运维文档](./ops/README_ZH.md)
2. 查看 [多 GPU 部署](multi-gpu.md)
3. 学习 [Docker 部署](docker.md)

### 多模态应用
1. 查看 [多模态文档](./multimodal/README_ZH.md)
2. 了解支持的 [多模态模型](./multimodal/)

## 文档贡献

欢迎贡献文档改进：

- 修正错误和遗漏
- 添加示例和说明
- 翻译文档
- 更新过时信息

## 相关链接

- [主项目 README](../README.md)
- [GitHub Issues](https://github.com/ggml-org/llama.cpp/issues)
- [Wiki](https://github.com/ggml-org/llama.cpp/wiki)