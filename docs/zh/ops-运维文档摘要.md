# GGML 运维文档摘要

## 1. ops.md 文档概述

`docs/ops.md` 文档主要展示了 GGML 操作在各硬件后端上的支持状态表格，而非传统意义上的服务器运维文档。

### 1.1 文档作用

- 记录各种 GGML 操作在不同后端的支持情况
- 为运维人员选择合适的后端提供参考
- 帮助理解不同硬件平台的功能限制

### 1.2 后端支持状态说明

| 状态 | 含义 |
|------|------|
| ✅ | 完全支持 |
| 🟡 | 部分支持 |
| ❌ | 不支持 |

### 1.3 支持的后端

- **BLAS**: 基础线性代数子程序
- **CANN**: 华为昇腾NPU
- **CPU**: 通用CPU
- **CUDA**: NVIDIA GPU
- **MTL**: Apple Metal
- **OpenCL**: 通用GPU计算
- **SYCL**: Intel GPU
- **Vulkan**: 通用GPU API
- **WebGPU**: Web端GPU
- **ZenDNN**: AMD CPU
- **zDNN**: IBM Z & LinuxONE

### 1.4 关键操作支持情况

#### 核心推理操作
- **MUL_MAT**: 所有后端部分支持，这是矩阵乘法的核心操作
- **SOFT_MAX**: 大部分后端部分支持
- **RMS_NORM**: 除BLAS、ZenDNN、zDNN外均支持
- **ROPE**: 除BLAS、ZenDNN、zDNN外均支持

#### 注意力机制
- **FLASH_ATTN_EXT**: 大部分后端部分支持
- **GATED_LINEAR_ATTN**: 仅CANN和CPU支持

#### 激活函数
- **GELU/GELU_ERF/GELU_QUICK**: 除BLAS外均支持
- **SILU/SWIGLU**: 除BLAS外均支持
- **RELU**: 除BLAS外均支持

## 2. 服务器部署要点

### 2.1 部署方式

#### Docker 部署（推荐）

**基础镜像选择：**
- `ghcr.io/ggml-org/llama.cpp:server` - 专用服务器镜像
- `ghcr.io/ggml-org/llama.cpp:full` - 完整功能镜像
- `ghcr.io/ggml-org/llama.cpp:light` - 轻量级镜像

**GPU 加速镜像：**
- `ghcr.io/ggml-org/llama.cpp:server-cuda` - NVIDIA GPU
- `ghcr.io/ggml-org/llama.cpp:server-rocm` - AMD GPU
- `ghcr.io/ggml-org/llama.cpp:server-musa` - 摩尔线程GPU
- `ghcr.io/ggml-org/llama.cpp:server-intel` - Intel GPU (SYCL)
- `ghcr.io/ggml-org/llama.cpp:server-vulkan` - Vulkan GPU
- `ghcr.io/ggml-org/llama.cpp:server-openvino` - OpenVINO

**部署命令示例：**
```bash
# 基础部署
docker run -v /path/to/models:/models -p 8080:8080 \
  ghcr.io/ggml-org/llama.cpp:server -m /models/7B/ggml-model-q4_0.gguf

# GPU 部署
docker run --gpus all -v /path/to/models:/models -p 8080:8080 \
  ghcr.io/ggml-org/llama.cpp:server-cuda -m /models/7B/ggml-model-q4_0.gguf \
  --n-gpu-layers 1

# 多用户并发部署
docker run -v /path/to/models:/models -p 8080:8080 \
  ghcr.io/ggml-org/llama.cpp:server -m /models/7B/ggml-model-q4_0.gguf \
  -c 16384 -np 4
```

### 2.2 支持平台

| 平台 | 架构 | 支持状态 |
|------|------|----------|
| Linux | amd64 | ✅ |
| Linux | arm64 | ✅ |
| Linux | s390x | ✅ |
| macOS | arm64 (Apple Silicon) | ✅ |
| Windows | amd64 | ✅ |

### 2.3 模型文件存储

- **推荐路径**: 创建专用目录存储大模型（如 `/llama/models`）
- **格式要求**: GGUF 格式
- **量化级别**: 支持 1.5-bit 到 8-bit 整数量化

## 3. 监控配置方法

### 3.1 性能监控

#### 使用 llama-bench 进行基准测试
```bash
llama-bench -m model.gguf
```

**输出指标：**
- 模型大小 (MiB)
- 参数数量 (params)
- 后端类型 (backend)
- 线程数 (threads)
- 测试类型 (test)
- 吞吐量 (t/s)

#### 困惑度测量
```bash
llama-perplexity -m model.gguf -f file.txt
```

### 3.2 服务器监控端点

**WebUI**: `http://localhost:8080`
**Chat completion API**: `http://localhost:8080/v1/chat/completions`
**Embedding API**: `http://localhost:8080/v1/embeddings`
**Reranking API**: `http://localhost:8080/v1/rerankings`

### 3.3 资源监控要点

- **GPU 使用率**: 监控 VRAM 使用情况
- **CPU 线程数**: 根据 CPU 核心数调整
- **内存使用**: 模型大小 + KV Cache
- **网络延迟**: 对于 API 服务特别重要

## 4. 性能优化建议

### 4.1 硬件层面优化

#### GPU 优化
- 使用 `--n-gpu-layers` 参数控制 GPU 层数
- 选择合适的量化级别（Q4_0/Q4_K_M/Q5_K_M/Q6_K）
- 确保显存足够存储模型权重

#### CPU 优化
- 设置适当的线程数（通常设置为物理核心数）
- 启用适当的 SIMD 指令集（AVX2/AVX512）
- 考虑使用 BLAS 后端加速

### 4.2 模型层面优化

#### 量化策略
- **Q4_0/Q4_K_M**: 速度与精度的良好平衡
- **Q5_K_M**: 更高的精度，略微增加模型大小
- **Q8_0**: 接近全精度，但模型大小较大

#### 推测解码
```bash
llama-server -m model.gguf -md draft.gguf
```
- 使用小型 draft 模型加速推理
- 可显著提高生成速度

### 4.3 配置层面优化

#### 上下文长度
- 根据应用场景设置合适的上下文长度
- 较短的上下文可以减少内存使用

#### 批处理大小
- 对于批量推理，可以调整批处理大小
- 注意显存限制

#### KV Cache 优化
- 使用适当的 KV Cache 大小
- 考虑使用 KV Cache 量化

### 4.4 后端选择建议

| 使用场景 | 推荐后端 | 原因 |
|----------|----------|------|
| NVIDIA GPU | CUDA | 成熟稳定，性能最优 |
| Apple Silicon | Metal | 原生支持，性能优秀 |
| AMD GPU | ROCm | 官方支持 |
| Intel GPU | SYCL | Intel 优化 |
| 通用GPU | Vulkan | 跨平台兼容性好 |
| 仅有CPU | BLAS | 纯CPU优化 |

### 4.5 运维最佳实践

1. **资源规划**
   - 提前计算模型内存需求
   - 为 KV Cache 预留足够内存
   - 监控系统资源使用情况

2. **容器化部署**
   - 使用 Docker 简化部署流程
   - 设置资源限制（CPU、内存）
   - 使用适当的存储卷

3. **高可用性**
   - 考虑多实例部署
   - 实现负载均衡
   - 配置健康检查

4. **安全考虑**
   - 限制访问端口
   - 使用 HTTPS 通信
   - 实施认证机制

## 5. 故障排查

### 5.1 常见问题

#### GPU 不被识别
- 检查驱动安装
- 验证 CUDA/ROCm 工具包
- 检查 Docker GPU 访问权限

#### 内存不足
- 减小上下文长度
- 使用更低量化级别
- 减少 GPU 层数

#### 性能不佳
- 检查后端选择是否正确
- 验证线程数设置
- 运行 benchmark 测试

## 6. 参考资源

- [ops.md 原始文档](../ops.md)
- [Docker 部署文档](../docker.md)
- [构建文档](../build.md)
- [多 GPU 使用](../multi-gpu.md)
- [性能优化建议](development/token_generation_performance_tips.md)