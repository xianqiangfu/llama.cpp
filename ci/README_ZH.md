# CI 配置说明

本目录包含 llama.cpp 的持续集成（CI）配置和脚本，用于在本地和 CI 环境中运行自动化测试。

## 目录结构

```
ci/
└── run.sh               # CI 运行脚本
```

## 概述

CI 系统实现了重型工作流程，运行在自托管的运行器上。这些工作流程的目的是：

1. 覆盖 GitHub 托管运行器不可用的硬件配置
2. 提供比通常可用更多的计算资源
3. 支持各种硬件后端的测试和验证

## 快速开始

### 本地运行 CI

在本地机器上运行完整的 CI 测试是一个好习惯，可以在发布更改前验证代码。

```bash
mkdir tmp

# CPU-only 构建
bash ./ci/run.sh ./tmp/results ./tmp/mnt

# CUDA 支持
GG_BUILD_CUDA=1 bash ./ci/run.sh ./tmp/results ./tmp/mnt

# SYCL 支持
source /opt/intel/oneapi/setvars.sh
GG_BUILD_SYCL=1 bash ./ci/run.sh ./tmp/results ./tmp/mnt

# MUSA 支持
GG_BUILD_MUSA=1 bash ./ci/run.sh ./tmp/results ./tmp/mnt
```

### 参数说明

- `./tmp/results` - 测试结果输出目录
- `./tmp/mnt` - 挂载点目录（用于容器化测试）

## 环境变量

CI 运行脚本支持以下环境变量来控制构建行为：

| 环境变量 | 说明 | 示例 |
|---------|------|------|
| `GG_BUILD_CUDA` | 启用 CUDA 构建 | `GG_BUILD_CUDA=1` |
| `GG_BUILD_METAL` | 启用 Metal 构建 | `GG_BUILD_METAL=1` |
| `GG_BUILD_ROCM` | 启用 ROCm 构建 | `GG_BUILD_ROCM=1` |
| `GG_BUILD_SYCL` | 启用 SYCL 构建 | `GG_BUILD_SYCL=1` |
| `GG_BUILD_VULKAN` | 启用 Vulkan 构建 | `GG_BUILD_VULKAN=1` |
| `GG_BUILD_OPENVINO` | 启用 OpenVINO 构建 | `GG_BUILD_OPENVINO=1` |
| `GG_BUILD_CANN` | 启用 CANN 构建 | `GG_BUILD_CANN=1` |
| `GG_BUILD_MUSA` | 启用 MUSA 构建 | `GG_BUILD_MUSA=1` |

## 添加自托管运行器

### 步骤

1. **添加自托管工作流程**

在 `.github/workflows/build.yml` 中添加一个自托管 `ggml-ci` 工作流程，并指定适当的标签：

```yaml
jobs:
  build-self-hosted:
    runs-on: [self-hosted, linux, x64, cuda]
    steps:
      - uses: actions/checkout@v4
      - name: Build
        run: bash ./ci/run.sh ./results ./mnt
```

2. **请求运行器令牌**

从 `ggml-org` 请求运行器令牌（例如通过 PR 评论或电子邮件）。

3. **设置机器**

使用接收到的令牌设置机器：

```bash
# 下载并配置 runner
cd /path/to/runner
./config.sh --url https://github.com/ggml-org/llama.cpp --token YOUR_TOKEN

# 安装为服务
./svc.sh install
./svc.sh start
```

参考文档：[添加自托管运行器](https://docs.github.com/en/actions/how-tos/manage-runners/self-hosted-runners/add-runners)

4. **更新 CI 脚本**（可选）

如果需要，更新 `ci/run.sh` 以在目标平台上构建和运行，通过 `GG_BUILD_...` 环境变量控制实现。

## 运行脚本详解

`ci/run.sh` 是主要的 CI 运行脚本，负责：

1. **环境检测** - 检测可用的硬件和软件
2. **依赖安装** - 安装必要的构建依赖
3. **配置生成** - 生成 CMake 配置
4. **编译构建** - 编译项目
5. **测试执行** - 运行测试套件
6. **结果收集** - 收集和报告测试结果

## 支持的平台

CI 配置支持以下平台：

| 平台 | 标签 | 说明 |
|------|------|------|
| Linux | `linux` | 通用 Linux |
| Windows | `windows` | Windows 系统 |
| macOS | `macos` | macOS 系统 |
| x64 | `x64` | x86_64 架构 |
| ARM64 | `arm64` | ARM64 架构 |
| RISC-V | `riscv64` | RISC-V 架构 |

## 支持的后端

CI 配置支持以下硬件后端：

| 后端 | 环境变量 | 要求 |
|------|---------|------|
| CUDA | `GG_BUILD_CUDA` | NVIDIA GPU, CUDA Toolkit |
| Metal | `GG_BUILD_METAL` | Apple Silicon, Xcode |
| ROCm | `GG_BUILD_ROCM` | AMD GPU, ROCm |
| SYCL | `GG_BUILD_SYCL` | Intel GPU, oneAPI |
| Vulkan | `GG_BUILD_VULKAN` | Vulkan 驱动, Vulkan SDK |
| OpenVINO | `GG_BUILD_OPENVINO` | Intel CPU/GPU, OpenVINO Toolkit |
| CANN | `GG_BUILD_CANN` | 华为昇腾 NPU, CANN Toolkit |
| MUSA | `GG_BUILD_MUSA` | 摩尔线程 GPU, MUSA Toolkit |

## 测试矩阵

CI 运行多个测试矩阵以确保广泛覆盖：

1. **平台矩阵**
   - Linux (x64, ARM64)
   - Windows (x64)
   - macOS (x64, ARM64)

2. **编译器矩阵**
   - GCC
   - Clang
   - MSVC

3. **后端矩阵**
   - CPU-only
   - CUDA
   - Metal
   - ROCm
   - SYCL
   - Vulkan
   - OpenVINO

4. **构建类型**
   - Debug
   - Release
   - RelWithDebInfo

## 本地测试最佳实践

### 1. 预提交测试

在提交代码前运行相关测试：

```bash
# 快速测试
bash ./ci/run.sh ./tmp/results ./tmp/mnt

# 完整测试（包括所有后端）
GG_BUILD_CUDA=1 GG_BUILD_METAL=1 bash ./ci/run.sh ./tmp/results ./tmp/mnt
```

### 2. 特定后端测试

只测试特定后端：

```bash
# 只测试 CUDA
GG_BUILD_CUDA=1 bash ./ci/run.sh ./tmp/results ./tmp/mnt

# 只测试 SYCL
source /opt/intel/oneapi/setvars.sh
GG_BUILD_SYCL=1 bash ./ci/run.sh ./tmp/results ./tmp/mnt
```

### 3. 调试构建

使用 Debug 构建进行调试：

```bash
CMAKE_BUILD_TYPE=Debug bash ./ci/run.sh ./tmp/results ./tmp/mnt
```

## 结果和报告

### 测试结果位置

测试结果输出到指定的结果目录：

```
./tmp/results/
├── build.log          # 构建日志
├── test.log           # 测试日志
├── performance.txt    # 性能数据
└── summary.txt        # 测试摘要
```

### 查看结果

```bash
# 查看摘要
cat ./tmp/results/summary.txt

# 查看性能数据
cat ./tmp/results/performance.txt
```

## 故障排除

### 问题：依赖安装失败

解决方案：
- 检查网络连接
- 手动安装缺失的依赖
- 使用包管理器（apt, yum, brew）

### 问题：编译失败

解决方案：
- 检查 CMake 版本
- 验证编译器版本
- 查看构建日志获取详细错误

### 问题：测试失败

解决方案：
- 查看测试日志
- 检查环境配置
- 确认硬件可用

### 问题：后端不可用

解决方案：
- 验证驱动安装
- 检查环境变量
- 确认硬件可用

## 相关文档

- [llama.cpp 主 README](../README.md)
- [DevOps 配置](../.devops/README_ZH.md)
- [GitHub Actions 配置](../.github/README_ZH.md)
- [基准测试](../benches/README_ZH.md)
- [GitHub Actions 文档](https://docs.github.com/en/actions)