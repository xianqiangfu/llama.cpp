# DevOps 配置说明

本目录包含 DevOps（开发运维）相关的配置文件和工具脚本，用于支持项目的自动化构建、部署和跨平台开发。

## 目录结构

```
.devops/
├── cann.Dockerfile              # CANN (昇腾) Docker 镜像
├── cpu.Dockerfile               # CPU Docker 镜像
├── cuda.Dockerfile              # CUDA Docker 镜像
├── intel.Dockerfile             # Intel Docker 镜像
├── llama-cli-cann.Dockerfile    # llama-cli CANN Docker 镜像
├── llama-cpp-cuda.srpm.spec     # CUDA SRPM 规格文件
├── llama-cpp.srpm.spec          # SRPM 规格文件
├── musa.Dockerfile              # MUSA (摩尔线程) Docker 镜像
├── openvino.Dockerfile          # OpenVINO Docker 镜像
├── rocm.Dockerfile              # ROCm (AMD GPU) Docker 镜像
├── s390x.Dockerfile             # IBM s390x Docker 镜像
├── tools.sh                     # 工具脚本
├── vulkan.Dockerfile            # Vulkan Docker 镜像
└── nix/                         # Nix 包管理配置
```

## Docker 镜像

### CPU 镜像 (cpu.Dockerfile)

纯 CPU 运行的 Docker 镜像，适用于没有 GPU 的环境。

构建命令：
```bash
docker build -f .devops/cpu.Dockerfile -t llama.cpp:cpu .
```

### CUDA 镜像 (cuda.Dockerfile)

支持 NVIDIA CUDA 的 Docker 镜像，包含 CUDA 工具链和 cuBLAS 库。

构建命令：
```bash
docker build -f .devops/cuda.Dockerfile -t llama.cpp:cuda .
```

运行命令：
```bash
docker run --gpus all -it llama.cpp:cuda
```

### ROCm 镜像 (rocm.Dockerfile)

支持 AMD ROCm 的 Docker 镜像，适用于 AMD GPU。

构建命令：
```bash
docker build -f .devops/rocm.Dockerfile -t llama.cpp:rocm .
```

### CANN 镜像 (cann.Dockerfile, llama-cli-cann.Dockerfile)

支持华为昇腾 CANN 的 Docker 镜像。

构建命令：
```bash
docker build -f .devops/cann.Dockerfile -t llama.cpp:cann .
docker build -f .devops/llama-cli-cann.Dockerfile -t llama.cpp:cli-cann .
```

### MUSA 镜像 (musa.Dockerfile)

支持摩尔线程 MUSA 的 Docker 镜像。

构建命令：
```bash
docker build -f .devops/musa.Dockerfile -t llama.cpp:musa .
```

### OpenVINO 镜像 (openvino.Dockerfile)

支持 Intel OpenVINO 的 Docker 镜像。

构建命令：
```bash
docker build -f .devops/openvino.Dockerfile -t llama.cpp:openvino .
```

### Vulkan 镜像 (vulkan.Dockerfile)

支持 Vulkan 的跨平台 Docker 镜像。

构建命令：
```bash
docker build -f .devops/vulkan.Dockerfile -t llama.cpp:vulkan .
```

### Intel 镜像 (intel.Dockerfile)

Intel 优化的 Docker 镜像，可能包含 OpenMP、MKL 等 Intel 库。

构建命令：
```bash
docker build -f .devops/intel.Dockerfile -t llama.cpp:intel .
```

### s390x 镜像 (s390x.Dockerfile)

IBM s390x 架构的 Docker 镜像。

构建命令：
```bash
docker build -f .devops/s390x.Dockerfile -t llama.cpp:s390x .
```

## SRPM 包

### llama-cpp.srpm.spec

标准 SRPM（Source RPM）规格文件，用于构建 RPM 包。

构建 SRPM：
```bash
rpmbuild -bs .devops/llama-cpp.srpm.spec
```

### llama-cpp-cuda.srpm.spec

CUDA 版本的 SRPM 规格文件。

构建 SRPM：
```bash
rpmbuild -bs .devops/llama-cpp-cuda.srpm.spec
```

## 工具脚本 (tools.sh)

便捷工具脚本，提供常用操作的快捷方式。

### 可用命令

| 命令 | 说明 |
|------|------|
| `-c, --convert` | 转换模型为 GGUF 格式 |
| `-q, --quantize` | 量化模型 |
| `-r, --run` | 运行模型（聊天模式） |
| `-l, --run-legacy` | 运行模型（传统完成模式） |
| `-b, --bench` | 运行性能基准测试 |
| `-p, --perplexity` | 计算困惑度 |
| `-a, --all-in-one` | 执行转换和量化 |
| `-s, --server` | 启动服务器 |

### 使用示例

```bash
# 转换模型
bash .devops/tools.sh --convert --outtype f16 /models/7B/

# 量化模型
bash .devops/tools.sh --quantize /models/7B/ggml-model-f16.bin /models/7B/ggml-model-q4_0.bin q4_0

# 运行模型
bash .devops/tools.sh --run -m /models/7B/ggml-model-q4_0.bin

# 运行基准测试
bash .devops/tools.sh --bench -m model.gguf

# 计算困惑度
bash .devops/tools.sh --perplexity -m model.gguf -f file.txt

# 启动服务器
bash .devops/tools.sh --server -m /models/7B/ggml-model-q4_0.bin -c 2048 -ngl 43 --port 8080
```

## Nix 配置 (nix/)

Nix 包管理器配置文件，用于声明式的包管理和环境配置。

### Nix 配置文件

| 文件 | 说明 |
|------|------|
| `apps.nix` | 应用程序定义 |
| `devshells.nix` | 开发环境 shell 配置 |
| `docker.nix` | Docker 镜像定义 |
| `jetson-support.nix` | NVIDIA Jetson 支持配置 |
| `nixpkgs-instances.nix` | nixpkgs 实例配置 |
| `package-gguf-py.nix` | gguf-py 包定义 |
| `package.nix` | 主包定义 |
| `python-scripts.nix` | Python 脚本配置 |
| `scope.nix` | 作用域配置 |
| `sif.nix` | Singularity SIF 镜像配置 |

### 使用 Nix

#### 进入开发环境

```bash
nix develop
```

#### 构建包

```bash
nix build
```

#### 运行应用

```bash
nix run .#llama-cli
```

## 支持的平台

通过 DevOps 配置，llama.cpp 支持以下平台和硬件：

- x86_64 (Intel/AMD)
- ARM64 (Apple Silicon, ARM 服务器)
- RISC-V (Spacemit)
- IBM s390x
- IBM Power

支持的硬件加速：
- NVIDIA GPU (CUDA)
- AMD GPU (ROCm)
- Apple GPU (Metal)
- Intel GPU (SYCL, OpenVINO)
- 华为昇腾 NPU (CANN)
- 摩尔线程 GPU (MUSA)
- Vulkan GPU
- WebGPU

## 相关文档

- [Docker 文档](https://docs.docker.com/)
- [Nix 文档](https://nixos.org/manual/nix/stable/)
- [RPM 打包指南](https://rpm-packaging-guide.github.io/)
- [llama.cpp README](../README.md)
- [CI 文档](../ci/README.md)