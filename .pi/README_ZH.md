# 性能分析说明

本目录包含 llama.cpp 的性能分析配置和相关文件，用于帮助开发者分析和优化代码性能。

## 目录结构

```
.pi/
├── dgx-spark/          # NVIDIA DGX Spark 性能配置
├── mac-m2-ultra/       # Apple M2 Ultra 性能配置
└── nemotron/           # NVIDIA Nemotron 性能配置
```

## 性能分析工具

llama.cpp 提供多种性能分析工具和方法：

### 1. 基准测试 (llama-bench)

使用 `llama-bench` 工具进行性能基准测试。

基本用法：
```bash
./llama-bench -m model.gguf -p 512 -n 512 -b 512
```

常用选项：
- `-m` - 模型文件路径
- `-p` - 提示词长度
- `-n` - 生成长度
- `-b` - 批处理大小
- `-t` - 线程数
- `-ngl` - GPU 层数
- `-fa` - 启用 Flash Attention

输出示例：
```
main: build = 1234 (12345678)
main: run 0/1
main: run 1/1
main: benchmark completed with 1 runs
main: t_load_ms = 123.45
main: t_prompt_eval_per_token_ms = 12.34
main: t_sample_per_token_ms = 1.23
main: t_gen_per_token_ms = 23.45
```

### 2. 性能计数器

llama.cpp 内置性能计数器，可以跟踪各个阶段的耗时。

启用性能统计：
```bash
./llama-cli -m model.gguf --stats
```

### 3. Profiler 配置

针对不同硬件平台，本目录提供特定的性能配置：

#### NVIDIA DGX Spark (dgx-spark/)

配置 NVIDIA DGX Spark 集群的性能优化。

包含内容：
- GPU 分配策略
- NCCL 通信优化
- 数据并行配置

#### Apple M2 Ultra (mac-m2-ultra/)

配置 Apple M2 Ultra 的性能优化。

包含内容：
- Metal 后端优化
- 统一内存配置
- 神经引擎设置

#### NVIDIA Nemotron (nemotron/)

配置 NVIDIA Nemotron 模型的性能优化。

包含内容：
- 模型特定优化
- 精度配置
- 内存优化

## 性能指标

### 关键指标

| 指标 | 说明 | 优化目标 |
|------|------|----------|
| `t_load_ms` | 模型加载时间 | 越低越好 |
| `t_prompt_eval_per_token_ms` | 提示词评估时间/token | 越低越好 |
| `t_sample_per_token_ms` | 采样时间/token | 越低越好 |
| `t_gen_per_token_ms` | 生成时间/token | 越低越好 |
| `tokens_per_second` | 每秒生成 token 数 | 越高越好 |

### 性能分析步骤

1. **选择模型**
   ```bash
   # 下载或转换模型
   python convert_hf_to_gguf.py /path/to/hf/model
   ```

2. **运行基准测试**
   ```bash
   ./llama-bench -m model.gguf -p 128 -n 128 -b 1
   ```

3. **分析结果**
   - 查看各阶段耗时
   - 识别瓶颈
   - 记录基线性能

4. **优化并测试**
   - 调整参数（线程数、批大小、GPU 层数）
   - 启用优化（Flash Attention、量化）
   - 重新运行基准测试

5. **对比改进**
   - 与基线对比
   - 验证优化效果

## 优化建议

### GPU 加速

1. **增加 GPU 层数**
   ```bash
   ./llama-cli -m model.gguf -ngl 99
   ```

2. **启用 Flash Attention**
   ```bash
   ./llama-cli -m model.gguf -fa
   ```

3. **使用量化模型**
   ```bash
   ./llama-cli -m model-q4_k.gguf
   ```

### CPU 优化

1. **调整线程数**
   ```bash
   ./llama-cli -m model.gguf -t 8
   ```

2. **启用优化指令集**
   ```bash
   # 使用 AVX2/AVX512
   CMAKE_ARGS="-DLLAMA_AVX2=ON" make
   ```

### 内存优化

1. **使用 MMAP**
   ```bash
   ./llama-cli -m model.gguf --mmap
   ```

2. **调整批大小**
   ```bash
   ./llama-cli -m model.gguf -b 512
   ```

### 批处理优化

对于高吞吐量场景，使用批处理：
```bash
./llama-cli -m model.gguf --batch-size 32
```

## 性能配置文件

根据硬件类型，选择合适的配置：

### CPU 配置
```bash
./llama-bench -m model.gguf -t $(nproc) -p 512 -n 512 -b 1
```

### GPU 配置 (CUDA)
```bash
./llama-bench -m model.gguf -ngl 99 -fa -p 512 -n 512 -b 1
```

### GPU 配置 (Metal - Apple Silicon)
```bash
./llama-bench -m model.gguf -ngl 99 -fa -p 512 -n 512 -b 1
```

### 多 GPU 配置
```bash
CUDA_VISIBLE_DEVICES=0,1 ./llama-cli -m model.gguf --n-gpu-layers 99
```

## 性能分析工具链

### 使用 perf (Linux)

```bash
perf record -g ./llama-cli -m model.gguf
perf report
```

### 使用 Instruments (macOS)

使用 Xcode 的 Instruments 工具进行 GPU 和 CPU 分析。

### 使用 Nsight (NVIDIA)

```bash
nsys profile ./llama-cli -m model.gguf
```

### 使用 Intel VTune

```bash
vtune -collect hotspots ./llama-cli -m model.gguf
```

## 相关文档

- [llama.cpp README](../README.md)
- [基准测试文档](../benches/README.md)
- [后端文档](../docs/backend/)