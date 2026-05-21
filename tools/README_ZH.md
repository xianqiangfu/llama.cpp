# llama.cpp 工具集总览

本目录包含 llama.cpp 项目的各种实用工具，涵盖模型量化、性能测试、服务部署等多个方面。

## 工具列表

### 核心工具

| 工具 | 目录 | 功能描述 |
|------|------|----------|
| **llama-cli** | `cli/` | 命令行交互工具，支持聊天模式和交互式对话 |
| **llama-server** | `server/` | HTTP 服务器，提供 REST API 和 Web UI，支持 OpenAI 兼容接口 |
| **llama-quantize** | `quantize/` | 模型量化工具，将高精度模型转换为量化格式以减小体积 |
| **llama-bench** | `llama-bench/` | 性能基准测试工具，用于测量模型推理速度 |

### 模型处理工具

| 工具 | 目录 | 功能描述 |
|------|------|----------|
| **llama-imatrix** | `imatrix/` | 计算重要性矩阵，用于提高量化模型质量 |
| **gguf-split** | `gguf-split/` | GGUF 模型文件分割工具 |
| **export-lora** | `export-lora/` | LoRA 适配器导出工具 |
| **fit-params** | `fit-params/` | 模型参数拟合工具 |

### 测试与分析工具

| 工具 | 目录 | 功能描述 |
|------|------|----------|
| **perplexity** | `perplexity/` | 困惑度计算工具，评估模型质量 |
| **batched-bench** | `batched-bench/` | 批量性能基准测试 |
| **cvector-generator** | `cvector-generator/` | 控制向量生成器 |
| **results** | `results/` | 测试结果收集和分析 |

### 文本处理工具

| 工具 | 目录 | 功能描述 |
|------|------|----------|
| **completion** | `completion/` | 文本补全工具 |
| **tokenize** | `tokenize/` | 文本分词工具 |
| **parser** | `parser/` | 通用解析器 |

### 多模态与音频工具

| 工具 | 目录 | 功能描述 |
|------|------|----------|
| **mtmd** | `mtmd/` | 多模态数据处理工具 |
| **tts** | `tts/` | 文本转语音工具 |

### 其他工具

| 工具 | 目录 | 功能描述 |
|------|------|----------|
| **rpc** | `rpc/` | RPC（远程过程调用）支持 |
| **ui** | `ui/` | Web 用户界面相关文件 |

## 快速开始

### 基础使用

```bash
# 命令行交互
./llama-cli -m model.gguf -p "你好，请介绍一下你自己"

# 启动服务器
./llama-server -m model.gguf --port 8080

# 模型量化
./llama-quantize model-f16.gguf model-q4_k.gguf Q4_K_M

# 性能测试
./llama-bench -m model.gguf -p 512 -n 128

# 计算重要性矩阵（用于优化量化）
./llama-imatrix -m model.gguf -f calibration.txt
```

### 常见工作流程

#### 1. 模型准备和量化

```bash
# 转换 Hugging Face 模型到 GGUF
python3 convert_hf_to_gguf.py ./models/mymodel/

# 计算重要性矩阵（推荐）
./llama-imatrix -m ./models/mymodel/ggml-model-f16.gguf -f calibration.txt -ngl 99

# 使用重要性矩阵进行量化
./llama-quantize --imatrix imatrix.gguf ./models/mymodel/ggml-model-f16.gguf \
    ./models/mymodel/ggml-model-Q4_K_M.gguf Q4_K_M
```

#### 2. 性能测试

```bash
# 测试不同 GPU 层数的性能
./llama-bench -ngl 10,20,30,31,32,33,34,35

# 测试不同批处理大小的性能
./llama-bench -n 0 -p 1024 -b 128,256,512,1024

# 测试不同线程数的性能
./llama-bench -n 0 -n 16 -p 64 -t 1,2,4,8,16,32
```

#### 3. 部署服务

```bash
# 基础服务器
./llama-server -m model.gguf -c 2048 --host 0.0.0.0 --port 8080

# 带 GPU 加速的服务器
./llama-server -m model.gguf -ngl 99 --host 0.0.0.0 --port 8080

# 启用多槽位（并发请求）
./llama-server -m model.gguf -np 4

# 使用 Docker
docker run -p 8080:8080 -v /path/to/models:/models \
    ghcr.io/ggml-org/llama.cpp:server \
    -m models/model.gguf -c 512 --host 0.0.0.0 --port 8080
```

## 详细文档

每个工具都有独立的 README 文档，提供更详细的使用说明：

- [llama-cli 文档](./cli/README.md)
- [llama-server 文档](./server/README.md)
- [llama-quantize 文档](./quantize/README.md)
- [llama-bench 文档](./llama-bench/README.md)
- [llama-imatrix 文档](./imatrix/README.md)
- [mtmd 文档](./mtmd/README.md)

## 命令行参数

大多数工具共享一组通用参数，可以通过 `-h` 或 `--help` 查看：

```bash
./llama-cli --help
./llama-server --help
./llama-quantize --help
```

### 常用通用参数

| 参数 | 说明 |
|------|------|
| `-m, --model` | 模型文件路径 |
| `-t, --threads` | CPU 线程数 |
| `-ngl, --n-gpu-layers` | GPU 层数 |
| `-c, --ctx-size` | 上下文大小 |
| `-b, --batch-size` | 批处理大小 |
| `-fa, --flash-attn` | Flash Attention 支持 |

## 技术支持

如需帮助或报告问题，请访问：
- GitHub Issues: https://github.com/ggml-org/llama.cpp/issues
- Discord 社区: https://discord.gg/llama-cpp

## 许可证

本工具集遵循 llama.cpp 项目的许可证。