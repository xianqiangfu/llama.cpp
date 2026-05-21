# Docker 部署文档摘要 (docker.md)

## 概述

llama.cpp 提供了多种 Docker 镜像，支持不同的硬件加速后端（CUDA、ROCm、MUSA、SYCL、Vulkan、OpenVino）和平台（linux/amd64、linux/arm64、linux/s390x）。通过 Docker 容器化部署，可以简化环境配置，提高部署效率和可移植性。

## 前置条件

### 系统要求

- Docker 已安装并运行
- 创建存储大模型和中间文件的文件夹（例如 `/llama/models`）

### 存储空间

根据模型大小预留足够的磁盘空间：
- 小型模型（7B）：约 8-16 GB
- 中型模型（13B）：约 16-32 GB
- 大型模型（30B+）：约 32-64 GB

## Docker 镜像构建方法

### 基础镜像类型

#### 1. Full 镜像（`ghcr.io/ggml-org/llama.cpp:full`）

**包含内容**：
- `llama-cli` 可执行文件
- `llama-completion` 可执行文件
- 模型转换工具（将 LLaMA 模型转换为 ggml 格式）
- 4 位量化工具

**支持平台**：
- `linux/amd64`
- `linux/arm64`
- `linux/s390x`

**适用场景**：
- 需要完整的模型处理流程
- 首次部署和模型转换
- 开发和测试环境

#### 2. Light 镜像（`ghcr.io/ggml-org/llama.cpp:light`）

**包含内容**：
- `llama-cli` 可执行文件
- `llama-completion` 可执行文件

**支持平台**：
- `linux/amd64`
- `linux/arm64`
- `linux/s390x`

**适用场景**：
- 仅需要推理功能
- 已有量化模型
- 生产环境部署

#### 3. Server 镜像（`ghcr.io/ggml-org/llama.cpp:server`）

**包含内容**：
- `llama-server` 可执行文件

**支持平台**：
- `linux/amd64`
- `linux/arm64`
- `linux/s390x`

**适用场景**：
- API 服务部署
- 多客户端访问
- 云端部署

### 硬件加速镜像

#### CUDA 支持镜像

```bash
# CUDA 12 支持
ghcr.io/ggml-org/llama.cpp:full-cuda
ghcr.io/ggml-org/llama.cpp:light-cuda
ghcr.io/ggml-org/llama.cpp:server-cuda

# CUDA 13 支持
ghcr.io/ggml-org/llama.cpp:full-cuda13
ghcr.io/ggml-org/llama.cpp:light-cuda13
ghcr.io/ggml-org/llama.cpp:server-cuda13
```

**支持平台**：`linux/amd64`、`linux/arm64`

**前提条件**：
- 已安装 [nvidia-container-toolkit](https://github.com/NVIDIA/nvidia-container-toolkit)
- 或使用支持 GPU 的云服务

#### ROCm 支持镜像

```bash
ghcr.io/ggml-org/llama.cpp:full-rocm
ghcr.io/ggml-org/llama.cpp:light-rocm
ghcr.io/ggml-org/llama.cpp:server-rocm
```

**支持平台**：`linux/amd64`（AMD GPU）

#### MUSA 支持镜像

```bash
ghcr.io/ggml-org/llama.cpp:full-musa
ghcr.io/ggml-org/llama.cpp:light-musa
ghcr.io/ggml-org/llama.cpp:server-musa
```

**支持平台**：`linux/amd64`（摩尔线程 GPU）

**前提条件**：
- 已安装 [mt-container-toolkit](https://developer.mthreads.com/musa/native)

#### Intel SYCL 支持镜像

```bash
ghcr.io/ggml-org/llama.cpp:full-intel
ghcr.io/ggml-org/llama.cpp:light-intel
ghcr.io/ggml-org/llama.cpp:server-intel
```

**支持平台**：`linux/amd64`（Intel GPU）

#### Vulkan 支持镜像

```bash
ghcr.io/ggml-org/llama.cpp:full-vulkan
ghcr.io/ggml-org/llama.cpp:light-vulkan
ghcr.io/ggml-org/llama.cpp:server-vulkan
```

**支持平台**：`linux/amd64`、`linux/arm64`

#### OpenVINO 支持镜像

```bash
ghcr.io/ggml-org/llama.cpp:full-openvino
ghcr.io/ggml-org/llama.cpp:light-openvino
ghcr.io/ggml-org/llama.cpp:server-openvino
```

**支持平台**：`linux/amd64`

#### s390x 平台镜像

```bash
ghcr.io/ggml-org/llama.cpp:full-s390x    # full 镜像别名
ghcr.io/ggml-org/llama.cpp:light-s390x   # light 镜像别名
ghcr.io/ggml-org/llama.cpp:server-s390x  # server 镜像别名
```

**支持平台**：`linux/s390x`

## Docker 容器运行配置

### 基础使用

#### Full 镜像 - 模型转换和优化

```bash
# 使用 --all-in-one 命令下载、转换和优化模型
docker run -v /path/to/models:/models ghcr.io/ggml-org/llama.cpp:full --all-in-one "/models/" 7B
```

**说明**：
- `/path/to/models`：替换为实际模型存储路径
- `7B`：模型大小参数
- `--all-in-one`：自动化处理流程

#### Full 镜像 - 运行推理

```bash
# 标准推理
docker run -v /path/to/models:/models ghcr.io/ggml-org/llama.cpp:full --run -m /models/7B/ggml-model-q4_0.gguf

# 传统推理模式
docker run -v /path/to/models:/models ghcr.io/ggml-org/llama.cpp:full --run-legacy \
  -m /models/32B/ggml-model-q8_0.gguf \
  -no-cnv \
  -p "Building a mobile app can be done in 15 steps:" \
  -n 512
```

#### Light 镜像 - CLI 工具

```bash
# llama-cli
docker run -v /path/to/models:/models --entrypoint /app/llama-cli \
  ghcr.io/ggml-org/llama.cpp:light \
  -m /models/7B/ggml-model-q4_0.gguf

# llama-completion
docker run -v /path/to/models:/models --entrypoint /app/llama-completion \
  ghcr.io/ggml-org/llama.cpp:light \
  -m /models/32B/ggml-model-q8_0.gguf \
  -no-cnv \
  -p "Building a mobile app can be done in 15 steps:" \
  -n 512
```

**注意**：`--entrypoint /app/llama-cli` 可以省略，因为它是默认入口点。

#### Server 镜像 - API 服务

```bash
docker run -v /path/to/models:/models -p 8080:8080 \
  ghcr.io/ggml-org/llama.cpp:server \
  -m /models/7B/ggml-model-q4_0.gguf \
  --port 8080 \
  --host 0.0.0.0 \
  -n 512
```

**参数说明**：
- `-v /path/to/models:/models`：挂载模型目录
- `-p 8080:8080`：端口映射（主机:容器）
- `--port 8080`：容器内服务端口
- `--host 0.0.0.0`：监听所有网络接口
- `-n 512`：生成最大 token 数

### CUDA GPU 支持

#### 使用预构建镜像

```bash
# Full 镜像
docker run --gpus all -v /path/to/models:/models \
  ghcr.io/ggml-org/llama.cpp:full-cuda \
  --run -m /models/7B/ggml-model-q4_0.gguf \
  -p "Building a website can be done in 10 simple steps:" \
  -n 512 \
  --n-gpu-layers 1

# Light 镜像
docker run --gpus all -v /path/to/models:/models \
  ghcr.io/ggml-org/llama.cpp:light-cuda \
  -m /models/7B/ggml-model-q4_0.gguf \
  -p "Building a website can be done in 10 simple steps:" \
  -n 512 \
  --n-gpu-layers 1

# Server 镜像
docker run --gpus all -v /path/to/models:/models -p 8080:8080 \
  ghcr.io/ggml-org/llama.cpp:server-cuda \
  -m /models/7B/ggml-model-q4_0.gguf \
  --port 8080 \
  --host 0.0.0.0 \
  -n 512 \
  --n-gpu-layers 1
```

**关键参数**：
- `--gpus all`：启用所有 GPU
- `--n-gpu-layers 1`：GPU 层数配置

#### 本地构建 CUDA 镜像

```bash
# Full 镜像
docker build -t local/llama.cpp:full-cuda --target full -f .devops/cuda.Dockerfile .

# Light 镜像
docker build -t local/llama.cpp:light-cuda --target light -f .devops/cuda.Dockerfile .

# Server 镜像
docker build -t local/llama.cpp:server-cuda --target server -f .devops/cuda.Dockerfile .
```

**构建参数**：
- `CUDA_VERSION`：默认 `12.8.1`
- `CUDA_DOCKER_ARCH`：默认包含所有支持的架构

**自定义构建**：
```bash
# 指定 CUDA 版本
docker build -t local/llama.cpp:full-cuda \
  --build-arg CUDA_VERSION=12.4.0 \
  --target full -f .devops/cuda.Dockerfile .

# 指定 GPU 架构
docker build -t local/llama.cpp:full-cuda \
  --build-arg CUDA_DOCKER_ARCH="80;86" \
  --target full -f .devops/cuda.Dockerfile .
```

### MUSA GPU 支持

#### 本地构建 MUSA 镜像

```bash
# Full 镜像
docker build -t local/llama.cpp:full-musa --target full -f .devops/musa.Dockerfile .

# Light 镜像
docker build -t local/llama.cpp:light-musa --target light -f .devops/musa.Dockerfile .

# Server 镜像
docker build -t local/llama.cpp:server-musa --target server -f .devops/musa.Dockerfile .
```

**构建参数**：
- `MUSA_VERSION`：默认 `rc4.3.0`

#### 配置 MUSA 运行时

```bash
# 设置 MUSA 为默认 Docker 运行时
(cd /usr/bin/musa && sudo ./docker setup $PWD)

# 验证配置
docker info | grep mthreads
```

#### 运行 MUSA 镜像

```bash
# Full 镜像
docker run -v /path/to/models:/models \
  local/llama.cpp:full-musa \
  --run -m /models/7B/ggml-model-q4_0.gguf \
  -p "Building a website can be done in 10 simple steps:" \
  -n 512 \
  --n-gpu-layers 1

# Server 镜像
docker run -v /path/to/models:/models -p 8080:8080 \
  local/llama.cpp:server-musa \
  -m /models/7B/ggml-model-q4_0.gguf \
  --port 8080 \
  --host 0.0.0.0 \
  -n 512 \
  --n-gpu-layers 1
```

## Docker Compose 配置

### 基础服务配置

```yaml
version: '3.8'

services:
  llama-server:
    image: ghcr.io/ggml-org/llama.cpp:server
    container_name: llama-cpp-server
    ports:
      - "8080:8080"
    volumes:
      - ./models:/models
    command: >
      -m /models/7B/ggml-model-q4_0.gguf
      --port 8080
      --host 0.0.0.0
      -n 512
    restart: unless-stopped
```

### GPU 加速服务配置

```yaml
version: '3.8'

services:
  llama-server-gpu:
    image: ghcr.io/ggml-org/llama.cpp:server-cuda
    container_name: llama-cpp-server-gpu
    ports:
      - "8080:8080"
    volumes:
      - ./models:/models
    command: >
      -m /models/7B/ggml-model-q4_0.gguf
      --port 8080
      --host 0.0.0.0
      -n 512
      --n-gpu-layers 1
    deploy:
      resources:
        reservations:
          devices:
            - driver: nvidia
              count: all
              capabilities: [gpu]
    restart: unless-stopped
```

### 多实例负载均衡配置

```yaml
version: '3.8'

services:
  llama-server-1:
    image: ghcr.io/ggml-org/llama.cpp:server
    container_name: llama-cpp-server-1
    ports:
      - "8081:8080"
    volumes:
      - ./models:/models
    command: >
      -m /models/7B/ggml-model-q4_0.gguf
      --port 8080
      --host 0.0.0.0
      -n 512
    restart: unless-stopped

  llama-server-2:
    image: ghcr.io/ggml-org/llama.cpp:server
    container_name: llama-cpp-server-2
    ports:
      - "8082:8080"
    volumes:
      - ./models:/models
    command: >
      -m /models/7B/ggml-model-q4_0.gguf
      --port 8080
      --host 0.0.0.0
      -n 512
    restart: unless-stopped

  nginx:
    image: nginx:alpine
    container_name: nginx-loadbalancer
    ports:
      - "80:80"
    volumes:
      - ./nginx.conf:/etc/nginx/nginx.conf:ro
    depends_on:
      - llama-server-1
      - llama-server-2
    restart: unless-stopped
```

### 监控和日志配置

```yaml
version: '3.8'

services:
  llama-server:
    image: ghcr.io/ggml-org/llama.cpp:server
    container_name: llama-cpp-server
    ports:
      - "8080:8080"
    volumes:
      - ./models:/models
      - ./logs:/var/log/llama
    command: >
      -m /models/7B/ggml-model-q4_0.gguf
      --port 8080
      --host 0.0.0.0
      -n 512
      --log-file /var/log/llama/server.log
    restart: unless-stopped
    logging:
      driver: "json-file"
      options:
        max-size: "10m"
        max-file: "3"

  prometheus:
    image: prom/prometheus:latest
    container_name: prometheus
    ports:
      - "9090:9090"
    volumes:
      - ./prometheus.yml:/etc/prometheus/prometheus.yml:ro
    restart: unless-stopped
```

## 性能优化建议

### 资源限制配置

```bash
# 限制内存使用
docker run -v /path/to/models:/models \
  -m 8g \
  --memory-swap 16g \
  ghcr.io/ggml-org/llama.cpp:server \
  -m /models/7B/ggml-model-q4_0.gguf

# 限制 CPU 使用
docker run -v /path/to/models:/models \
  --cpus="2.0" \
  ghcr.io/ggml-org/llama.cpp:server \
  -m /models/7B/ggml-model-q4_0.gguf
```

### 存储优化

```bash
# 使用 tmpfs 提高模型加载速度
docker run -v /path/to/models:/models \
  --tmpfs /tmp:rw,size=4g \
  ghcr.io/ggml-org/llama.cpp:server \
  -m /models/7B/ggml-model-q4_0.gguf
```

### 网络优化

```bash
# 使用 host 网络模式
docker run --network host \
  -v /path/to/models:/models \
  ghcr.io/ggml-org/llama.cpp:server \
  -m /models/7B/ggml-model-q4_0.gguf
```

## 部署最佳实践

### 1. 镜像选择

| 场景 | 推荐镜像 | 理由 |
|------|---------|------|
| 开发测试 | `full-cuda` | 包含完整工具链 |
| 生产环境 | `light-cuda` | 体积小，启动快 |
| API 服务 | `server-cuda` | 专门的服务器实现 |
| CPU 环境 | `light` | 兼容性好 |
| AMD GPU | `light-rocm` | ROCm 优化 |

### 2. 安全配置

```bash
# 以非 root 用户运行
docker run --user 1000:1000 \
  -v /path/to/models:/models \
  ghcr.io/ggml-org/llama.cpp:server \
  -m /models/7B/ggml-model-q4_0.gguf

# 只读文件系统
docker run --read-only \
  -v /path/to/models:/models:ro \
  --tmpfs /tmp:rw \
  ghcr.io/ggml-org/llama.cpp:server \
  -m /models/7B/ggml-model-q4_0.gguf
```

### 3. 健康检查

```yaml
healthcheck:
  test: ["CMD", "curl", "-f", "http://localhost:8080/health"]
  interval: 30s
  timeout: 10s
  retries: 3
  start_period: 40s
```

### 4. 日志管理

```bash
# 集中日志收集
docker run -v /path/to/models:/models \
  -v /var/log/llama:/var/log/llama \
  --log-driver=journald \
  --log-opt labels=llama.cpp \
  ghcr.io/ggml-org/llama.cpp:server \
  -m /models/7B/ggml-model-q4_0.gguf
```

## 故障排除

### 常见问题

#### 1. GPU 不可用

**症状**：容器无法访问 GPU

**解决方案**：
```bash
# 检查 nvidia-container-toolkit
docker run --rm --gpus all nvidia/cuda:11.6.2-base-ubuntu20.04 nvidia-smi

# 重新安装 nvidia-container-toolkit
sudo apt-get install -y nvidia-container-toolkit
sudo systemctl restart docker
```

#### 2. 模型加载失败

**症状**：容器启动但无法加载模型

**解决方案**：
- 检查模型路径映射
- 验证模型文件完整性
- 检查文件权限

#### 3. 性能问题

**症状**：推理速度比预期慢

**解决方案**：
- 增加 `--n-gpu-layers` 参数
- 使用更小的模型
- 检查 GPU 利用率

#### 4. 内存不足

**症状**：容器被 OOM killer 终止

**解决方案**：
- 使用量化模型
- 增加 Docker 内存限制
- 减小上下文大小

## 总结

llama.cpp 的 Docker 部署提供了灵活、高效的容器化解决方案。通过选择合适的镜像类型、配置适当的运行参数，可以在各种硬件平台上实现高性能的模型推理。建议在生产环境中使用 GPU 加速镜像，配合 Docker Compose 实现可扩展的服务部署，并通过监控和日志管理确保服务稳定性。