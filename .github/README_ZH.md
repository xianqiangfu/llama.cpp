# GitHub Actions 配置说明

本目录包含 GitHub Actions 自动化配置，用于持续集成（CI）、持续部署（CD）和项目管理自动化。

## 目录结构

```
.github/
├── ISSUE_TEMPLATE/          # Issue 模板
├── actions/                 # 可重用的 GitHub Actions
├── workflows/               # 工作流程定义
├── labeler.yml              # 自动标签配置
└── pull_request_template.md # PR 模板
```

## 工作流程 (workflows/)

主要工作流程文件：

| 工作流程 | 说明 |
|---------|------|
| `build.yml` | 主构建流程，支持多平台和多后端编译 |
| `build-apple.yml` | Apple 平台构建（Metal 后端） |
| `build-sycl.yml` | SYCL 后端构建（Intel GPU） |
| `build-cann.yml` | CANN 后端构建（华为昇腾 NPU） |
| `build-vulkan.yml` | Vulkan 后端构建 |
| `build-openvino.yml` | OpenVINO 后端构建 |
| `build-cross.yml` | 交叉编译构建 |
| `build-self-hosted.yml` | 自托管运行器构建 |
| `server.yml` | Server 应用构建 |
| `server-self-hosted.yml` | Server 自托管运行器构建 |
| `docker.yml` | Docker 镜像构建 |
| `release.yml` | 发布流程 |
| `code-style.yml` | 代码风格检查 |
| `ai-issues.yml` | AI Issue 管理 |

### 构建后端支持

llama.cpp 支持多种硬件加速后端：

- **CUDA** - NVIDIA GPU
- **Metal** - Apple Silicon GPU
- **SYCL** - Intel GPU
- **Vulkan** - 跨平台 GPU
- **ROCm** - AMD GPU
- **CANN** - 华为昇腾 NPU
- **OpenVINO** - Intel CPU/GPU/NPU
- **MUSA** - 摩尔线程 GPU
- **zDNN** - IBM z/Architecture
- **ZenDNN** - AMD ZenDNN
- **WebGPU** - Web 平台

## 自动标签 (labeler.yml)

`labeler.yml` 配置了基于文件变更的自动标签规则。当 PR 创建时，GitHub Actions 会根据变更的文件自动添加相应的标签。

支持的标签分类：

| 标签 | 变更文件模式 |
|------|-------------|
| `Apple Metal` | ggml-metal 相关文件 |
| `SYCL` | ggml-sycl 相关文件 |
| `Nvidia GPU` | ggml-cuda 相关文件 |
| `Vulkan` | ggml-vulkan 相关文件 |
| `IBM zDNN` | ggml-zdnn 相关文件 |
| `AMD ZenDNN` | ggml-zendnn 相关文件 |
| `documentation` | docs/, media/ 目录 |
| `testing` | tests/ 目录 |
| `build` | cmake/, CMakeLists.txt |
| `examples` | examples/, tools/ 目录 |
| `devops` | .devops/, .github/, ci/ 目录 |
| `python` | Python 文件和包 |
| `script` | scripts/ 目录 |
| `android` | Android 相关文件 |
| `server` | server 相关文件 |
| `server/ui` | UI 相关文件 |
| `ggml` | ggml/ 目录 |
| `model` | src/models/ 目录 |
| `nix` | Nix 配置文件 |
| `embedding` | embedding 示例 |
| `Ascend NPU` | CANN 相关文件 |
| `OpenCL` | OpenCL 相关文件 |

## 可重用 Actions (actions/)

包含可重用的 Action 组件：

| Action | 说明 |
|--------|------|
| `get-tag-name` | 获取 Git 标签名称 |
| `install-exe` | 安装可执行文件 |
| `linux-setup-openvino` | Linux OpenVINO 环境设置 |
| `linux-setup-spacemit` | Linux Spacemit 环境设置 |
| `linux-setup-vulkan` | Linux Vulkan 环境设置 |
| `unarchive-tar` | 解压 tar 归档 |
| `windows-setup-cuda` | Windows CUDA 环境设置 |
| `windows-setup-rocm` | Windows ROCm 环境设置 |

## Issue 模板 (ISSUE_TEMPLATE/)

提供标准化的 Issue 创建模板，帮助用户更好地报告问题和提出建议。

## PR 模板 (pull_request_template.md)

提供标准化的 PR 创建模板，确保 PR 包含必要的信息。

## 配置文件说明

### labeler.yml

使用 [actions/labeler](https://github.com/actions/labeler) 自动为 PR 添加标签。

配置示例：
```yaml
Apple Metal:
    - changed-files:
        - any-glob-to-any-file:
            - ggml/include/ggml-metal.h
            - ggml/src/ggml-metal/**
            - README-metal.md
```

## 自定义工作流程

如需添加新的工作流程，请在 `.github/workflows/` 目录下创建 `.yml` 或 `.yaml` 文件。

工作流程文件必须包含：
- `name`: 工作流程名称
- `on`: 触发条件
- `jobs`: 定义执行的任务

示例：
```yaml
name: My Workflow
on: [push, pull_request]
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Build
        run: make
```

## 相关文档

- [GitHub Actions 文档](https://docs.github.com/en/actions)
- [工作流程语法](https://docs.github.com/en/actions/reference/workflow-syntax-for-github-actions)
- [llama.cpp 主 README](../../README.md)