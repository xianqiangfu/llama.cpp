# 基准测试说明

本目录包含 llama.cpp 的性能基准测试工具和脚本。

## 目录结构

```
benches/
├── CMakeLists.txt       # CMake 构建配置
├── llama.cpp            # 基准测试实现
├── run.sh               # 运行脚本
├── README.md            # 英文说明文档
└── README-MUSA.md       # MUSA (摩尔线程) 特定说明
```

## 概述

llama.cpp 提供全面的基准测试工具，用于评估不同配置下的推理性能。基准测试覆盖以下方面：

- 模型加载时间
- 提示词评估速度
- 文本生成速度
- 内存使用
- 不同量化格式的性能差异
- 不同硬件后端的性能对比

## 快速开始

### 编译基准测试工具

```bash
mkdir build
cd build
cmake ..
cmake --build . --target llama-bench
```

### 运行基准测试

```bash
./llama-bench -m model.gguf
```

### 使用运行脚本

```bash
cd benches
bash run.sh
```

## 运行脚本 (run.sh)

`run.sh` 提供了便捷的基准测试运行方式，支持自动配置和结果收集。

### 使用方法

```bash
bash run.sh [选项]
```

### 脚本功能

1. **自动检测硬件** - 识别可用的 GPU 和 CPU 配置
2. **多配置测试** - 自动测试不同参数组合
3. **结果收集** - 将结果保存到文件
4. **对比分析** - 生成性能对比报告

## 基准测试工具 (llama-bench)

### 命令行选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `-m` | 模型文件路径 | 必需 |
| `-p` | 提示词长度 | 512 |
| `-n` | 生成长度 | 512 |
| `-b` | 批处理大小 | 512 |
| `-t` | 线程数 | 自动检测 |
| `-ngl` | GPU 层数 | 99 |
| `-fa` | 启用 Flash Attention | false |
| `-mg` | 多图配置 | 1 |
| `-c` | 上下文大小 | 模型默认 |
| `-r` | 重复次数 | 1 |

### 基本用法示例

```bash
# 基本测试
./llama-bench -m model.gguf

# 自定义参数
./llama-bench -m model.gguf -p 128 -n 128 -b 1

# GPU 加速测试
./llama-bench -m model.gguf -ngl 99 -fa

# CPU 多线程测试
./llama-bench -m model.gguf -t 8

# 多次运行取平均值
./llama-bench -m model.gguf -r 3
```

## 性能指标

基准测试输出以下关键指标：

| 指标 | 说明 | 单位 |
|------|------|------|
| `t_load_ms` | 模型加载时间 | 毫秒 |
| `t_prompt_eval_per_token_ms` | 提示词评估时间/token | 毫秒 |
| `t_sample_per_token_ms` | 采样时间/token | 毫秒 |
| `t_gen_per_token_ms` | 生成时间/token | 毫秒 |
| `tokens_per_second` | 生成速度 | tokens/秒 |
| `n_tokens_evaluated` | 评估的 token 数 | 个 |
| `n_tokens_generated` | 生成的 token 数 | 个 |

### 输出示例

```
main: build = 1234 (12345678)
main: seed = 123456789
main: model loaded, n_ctx_max = 2048, n_ctx = 2048
main: run 0/1
main: run 1/1
main: benchmark completed with 1 runs

llama_bench_load_ms = 123.45
llama_bench_sample_ms = 1.23
llama_bench_p_eval_ms = 12.34
llama_bench_eval_ms = 23.45

main: perplexity: 10.23
main: n_tokens: 1024
main: t_load_ms = 123.45
main: t_p_eval_ms = 12345.67
main: t_gen_ms = 23456.78
main: t_p_eval_per_token_ms = 12.34
main: t_gen_per_token_ms = 23.45
main: n_tokens_per_second = 42.66
```

## 测试场景

### 1. 单次生成测试

测试单次文本生成的性能：

```bash
./llama-bench -m model.gguf -p 512 -n 512 -b 1
```

### 2. 批处理测试

测试批处理性能：

```bash
./llama-bench -m model.gguf -p 512 -n 512 -b 32
```

### 3. 长上下文测试

测试长上下文场景：

```bash
./llama-bench -m model.gguf -p 1024 -n 1024 -b 1
```

### 4. GPU 加速测试

测试 GPU 加速效果：

```bash
# 无 GPU
./llama-bench -m model.gguf -ngl 0

# 全 GPU
./llama-bench -m model.gguf -ngl 99

# 部分 GPU
./llama-bench -m model.gguf -ngl 20
```

### 5. Flash Attention 测试

测试 Flash Attention 的性能提升：

```bash
# 不启用
./llama-bench -m model.gguf

# 启用
./llama-bench -m model.gguf -fa
```

### 6. 量化格式对比

对比不同量化格式的性能：

```bash
# F16
./llama-bench -m model-f16.gguf

# Q4_K
./llama-bench -m model-q4_k.gguf

# Q8_0
./llama-bench -m model-q8_0.gguf
```

## MUSA 特定测试

对于摩尔线程 MUDA GPU，参考 [README-MUSA.md](./README-MUSA.md)。

### MUSA 测试示例

```bash
# MUDA GPU 测试
./llama-bench -m model.gguf -ngl 99 --device cuda

# 多 MUDA GPU 测试
./llama-bench -m model.gguf -ngl 99 --device cuda -mg 2
```

## 性能优化分析

使用基准测试识别性能瓶颈：

### 1. 识别瓶颈

```bash
./llama-bench -m model.gguf -v
```

查看详细的时间分解，找出最耗时的部分。

### 2. 内存分析

使用内存分析工具：

```bash
valgrind --tool=massif ./llama-bench -m model.gguf
```

### 3. CPU 分析

```bash
perf record -g ./llama-bench -m model.gguf
perf report
```

### 4. GPU 分析 (CUDA)

```bash
nsys profile ./llama-bench -m model.gguf
```

## 性能对比

### 创建对比报告

```bash
# 测试配置 A
./llama-bench -m model.gguf -ngl 0 > baseline.txt

# 测试配置 B
./llama-bench -m model.gguf -ngl 99 > optimized.txt

# 对比结果
diff baseline.txt optimized.txt
```

### 自动化测试脚本

创建自定义测试脚本：

```bash
#!/bin/bash
MODEL="model.gguf"

echo "Testing different thread counts..."
for t in 1 2 4 8 16; do
    echo "Threads: $t"
    ./llama-bench -m $MODEL -t $t
done

echo "Testing different batch sizes..."
for b in 1 8 16 32 64; do
    echo "Batch size: $b"
    ./llama-bench -m $MODEL -b $b
done
```

## CMake 配置

### 编译选项

```cmake
# 启用基准测试
cmake -DLLAMA_BENCH=ON ..

# 启用调试符号
cmake -DCMAKE_BUILD_TYPE=Debug ..

# 启用优化
cmake -DCMAKE_BUILD_TYPE=Release ..
```

## 最佳实践

1. **预热** - 多次运行取平均值
2. **隔离** - 关闭其他应用减少干扰
3. **一致环境** - 使用相同的测试环境
4. **记录配置** - 记录所有测试参数
5. **基线对比** - 建立性能基线

## 故障排除

### 问题：测试结果不稳定

解决方案：
- 增加运行次数（`-r` 选项）
- 检查系统负载
- 禁用 CPU 频率调节

### 问题：GPU 未使用

解决方案：
- 检查 `-ngl` 参数
- 验证 CUDA/Metal/ROCm 驱动
- 确认后端编译正确

### 问题：内存不足

解决方案：
- 减少上下文大小（`-c`）
- 使用量化模型
- 减少批处理大小（`-b`）

## 相关文档

- [llama.cpp 主 README](../README.md)
- [性能分析文档](../.pi/README.md)
- [量化指南](../docs/quantization.md)
- [后端文档](../docs/backend/)