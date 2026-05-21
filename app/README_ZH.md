# 应用程序说明

本目录包含 llama.cpp 的核心应用程序源代码。

## 目录结构

```
app/
├── CMakeLists.txt       # CMake 构建配置
└── llama.cpp            # 主应用程序源代码
```

## 应用程序概述

llama.cpp 提供多种命令行应用程序，用于不同的使用场景：

| 应用程序 | 说明 |
|---------|------|
| `llama-cli` | 命令行交互工具，用于对话和文本生成 |
| `llama-server` | HTTP API 服务器 |
| `llama-bench` | 性能基准测试工具 |
| `llama-perplexity` | 困惑度计算工具 |
| `llama-quantize` | 模型量化工具 |
| `llama-export-lora` | LoRA 适配器导出工具 |
| `llama-gguf` | GGUF 文件操作工具 |

## 主应用程序 (llama.cpp)

主应用程序是 llama.cpp 的核心入口点，提供了完整的推理功能。

### 编译

使用 CMake 编译：
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

编译特定应用：
```bash
cmake --build . --target llama-cli
cmake --build . --target llama-server
```

### 基本用法

#### llama-cli - 交互式聊天

```bash
./llama-cli -m model.gguf
```

常用选项：
- `-m` - 模型文件路径
- `-p` - 提示词
- `-n` - 生成 token 数
- `-c` - 上下文大小
- `-t` - 线程数
- `-ngl` - GPU 层数
- `-fa` - 启用 Flash Attention

示例：
```bash
# 交互模式
./llama-cli -m model.gguf

# 单次生成
./llama-cli -m model.gguf -p "Hello, how are you?" -n 128

# 指定参数
./llama-cli -m model.gguf -c 2048 -t 8 -ngl 99
```

#### llama-server - HTTP API 服务器

```bash
./llama-server -m model.gguf --port 8080
```

常用选项：
- `-m` - 模型文件路径
- `--port` - 服务端口
- `--host` - 监听地址
- `-c` - 上下文大小
- `-ngl` - GPU 层数
- `-fa` - 启用 Flash Attention

API 端点：
- `POST /completion` - 文本补全
- `POST /chat/completions` - 聊天补全（OpenAI 兼容）
- `GET /models` - 列出模型
- `GET /health` - 健康检查

示例：
```bash
# 启动服务器
./llama-server -m model.gguf --port 8080

# 调用 API
curl http://localhost:8080/completion -d '{
  "prompt": "Hello, how are you?",
  "n_predict": 128
}'
```

#### llama-bench - 基准测试

```bash
./llama-bench -m model.gguf -p 512 -n 512 -b 1
```

输出结果：
```
main: build = 1234 (12345678)
main: t_load_ms = 123.45
main: t_prompt_eval_per_token_ms = 12.34
main: t_sample_per_token_ms = 1.23
main: t_gen_per_token_ms = 23.45
```

#### llama-perplexity - 困惑度计算

```bash
./llama-perplexity -m model.gguf -f text.txt
```

输出模型在给定文本上的困惑度分数，分数越低表示模型对该文本的拟合越好。

#### llama-quantize - 模型量化

```bash
./llama-quantize model-f16.gguf model-q4_k.gguf q4_k
```

支持的量化方法：
- `q4_0` - 4-bit 量化（基础）
- `q4_k` - 4-bit K-quant（推荐）
- `q5_k` - 5-bit K-quant
- `q6_k` - 6-bit K-quant
- `q8_0` - 8-bit 量化

#### llama-export-lora - LoRA 导出

```bash
./llama-export-lora -m model.gguf -l lora.gguf -o output.gguf
```

将 LoRA 适配器合并到基础模型中。

#### llama-gguf - GGUF 操作

```bash
# 查看 GGUF 文件信息
./llama-gguf model.gguf

# 提取元数据
./llama-gguf model.gguf --extract metadata
```

## CMake 配置

### 支持的后端

编译时可以启用的后端：

```bash
# CUDA
cmake -DLLAMA_CUDA=ON ..

# Metal (Apple Silicon)
cmake -DLLAMA_METAL=ON ..

# ROCm (AMD GPU)
cmake -DLLAMA_ROCM=ON ..

# Vulkan
cmake -DLLAMA_VULKAN=ON ..

# SYCL (Intel GPU)
cmake -DLLAMA_SYCL=ON ..

# OpenVINO
cmake -DLLAMA_OPENVINO=ON ..
```

### 优化选项

```bash
# 启用所有警告
cmake -DLLAMA_ALL_WARNINGS=ON ..

# 将警告视为错误
cmake -DLLAMA_FATAL_WARNINGS=ON ..

# 启用线程消毒器
cmake -DLLAMA_SANITIZE_THREAD=ON ..

# 启用地址消毒器
cmake -DLLAMA_SANITIZE_ADDRESS=ON ..
```

## 架构概述

主应用程序采用模块化设计：

1. **模型加载** - 从 GGUF 文件加载模型权重和配置
2. **内存管理** - 管理模型权重和 KV Cache
3. **推理引擎** - 执行前向传播
4. **采样器** - 实现各种采样策略
5. **I/O 处理** - 处理用户输入和输出

## 性能优化

### GPU 加速

```bash
./llama-cli -m model.gguf -ngl 99 -fa
```

- `-ngl 99` - 将所有层加载到 GPU
- `-fa` - 启用 Flash Attention

### 线程配置

```bash
./llama-cli -m model.gguf -t 8
```

根据 CPU 核心数调整线程数。

### 内存映射

```bash
./llama-cli -m model.gguf --mmap
```

使用内存映射减少内存占用。

## 错误处理

应用程序包含完善的错误处理机制：

- 无效的模型文件
- 内存不足
- 后端初始化失败
- 无效的参数

错误信息会显示在标准错误输出中。

## 调试

启用详细日志：
```bash
./llama-cli -m model.gguf --verbose
```

启用统计信息：
```bash
./llama-cli -m model.gguf --stats
```

## 相关文档

- [llama.cpp 主 README](../README.md)
- [GGUF 格式文档](../docs/gguf.md)
- [量化指南](../docs/quantization.md)
- [API 文档](../docs/server.md)