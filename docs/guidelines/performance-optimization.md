# llama.cpp 性能优化注意事项

本文档总结 llama.cpp 项目的性能优化注意事项，帮助开发者优化推理性能。

## 1. 性能分析概述

llama.cpp 的性能主要受以下因素影响：
- 模型大小和量化级别
- 硬件配置（CPU/GPU）
- 批处理大小
- 上下文长度
- 后端选择

## 2. 性能瓶颈分析

### 2.1 常见性能瓶颈

#### 2.1.1 内存带宽瓶颈

**表现**：
- CPU 利用率不高
- 内存读写密集
- 延迟主要由内存访问引起

**解决方法**：
- 使用更快的内存
- 启用内存预取
- 优化内存访问模式
- 使用缓存友好的算法

#### 2.1.2 计算瓶颈

**表现**：
- CPU 利用率高
- 计算密集型操作
- GPU 利用率高

**解决方法**：
- 使用量化降低计算量
- 启用 SIMD 指令
- 使用 GPU 加速
- 优化计算算法

#### 2.1.3 I/O 瓶颈

**表现**：
- 模型加载时间长
- 磁盘 I/O 等待时间长
- 网络 I/O 等待时间长

**解决方法**：
- 使用 SSD 存储模型
- 使用内存映射
- 启用模型缓存
- 使用更快的网络

### 2.2 性能分析工具

#### 2.2.1 内置性能统计

```cpp
// 获取性能数据
llama_perf_context_data perf_get_data() const;

struct llama_perf_context_data {
    int64_t t_start_us;    // 启动时间
    int64_t t_load_us;     // 加载时间
    int64_t t_p_eval_us;   // 提示评估时间
    int64_t t_eval_us;     // 评估时间
    int32_t n_p_eval;      // 提示 token 数
    int32_t n_eval;        // 生成 token 数
    int32_t n_reused;      // 重用次数
};
```

#### 2.2.2 性能计算

```cpp
// 计算性能指标
double t_p_eval_ms = data.t_p_eval_us / 1000.0;
double t_eval_ms = data.t_eval_us / 1000.0;

// 提示处理速度
double prompt_speed = data.n_p_eval / (data.t_p_eval_us / 1000000.0);

// 生成速度
double generation_speed = data.n_eval / (data.t_eval_us / 1000000.0);
```

#### 2.2.3 外部工具

- **perf** (Linux): CPU 性能分析
- **NVIDIA Nsight**: GPU 性能分析
- **VTune**: Intel CPU 性能分析
- **Instruments** (macOS): 综合性能分析

## 3. 优化策略

### 3.1 模型优化

#### 3.1.1 量化

量化可以显著减少模型大小和计算量：

| 量化级别 | 大小减少 | 精度损失 | 性能提升 |
|---------|---------|---------|---------|
| Q4_K_M  | ~4x     | 小      | 高      |
| Q5_K_M  | ~3x     | 更小    | 中      |
| Q6_K    | ~2.5x   | 最小    | 低      |
| Q8_0    | ~2x     | 几乎没有| 很低    |

**量化工具**：
```bash
./tools/quantize/quantize model.gguf model-q4_k_m.gguf Q4_K_M
```

#### 3.1.2 模型选择

根据需求选择合适的模型：
- 小模型：快速推理，但精度较低
- 大模型：高精度，但推理慢
- 特定任务模型：针对特定任务优化

### 3.2 后端优化

#### 3.2.1 CPU 优化

```cpp
struct common_cpu_params {
    int      n_threads                   = -1;  // 线程数
    bool     cpumask[GGML_MAX_N_THREADS] = {false};  // CPU 亲和性
    bool     mask_valid                  = false;
    enum ggml_sched_priority  priority   = GGML_SCHED_PRIO_NORMAL;  // 优先级
    bool     strict_cpu                  = false;
    uint32_t poll                        = 50;   // 轮询级别
};
```

**优化建议**：
- 设置合理的线程数（通常等于物理核心数）
- 设置 CPU 亲和性减少上下文切换
- 使用高优先级
- 启用轮询降低延迟

#### 3.2.2 GPU 优化

```cpp
// 设置 GPU 层数
params.n_gpu_layers = 20;  // 将前 20 层放在 GPU 上
```

**优化建议**：
- 将尽可能多的层放在 GPU 上
- 监控 GPU 内存使用
- 使用批处理提高 GPU 利用率
- 启用 GPU 张量核心（如果可用）

#### 3.2.3 Metal 优化 (macOS)

**优化建议**：
- 启用 Metal 后端（默认）
- 调整 GPU 层数
- 使用 Apple Silicon 的专用优化

#### 3.2.4 Vulkan 优化

**优化建议**：
- 使用 Vulkan 实现跨平台 GPU 加速
- 调整批处理大小
- 监控 GPU 利用率

### 3.3 批处理优化

#### 3.3.1 批处理大小

```cpp
// 设置批处理大小
params.n_batch = 512;     // 逻辑批大小
params.n_ubatch = 32;     // 微批大小
```

**优化建议**：
- 大批处理：提高吞吐量，但增加延迟
- 小批处理：降低延迟，但降低吞吐量
- 根据应用场景选择合适的批处理大小

#### 3.3.2 动态批处理

```cpp
// 动态调整批处理大小
void adjust_batch_size(int current_queue_length) {
    if (current_queue_length > 10) {
        params.n_batch = 1024;  // 高负载时增大批处理
    } else {
        params.n_batch = 512;   // 低负载时减小批处理
    }
}
```

### 3.4 内存优化

#### 3.4.1 内存映射

```cpp
// 使用内存映射加载模型
bool llama_supports_mmap(void);
```

**优点**：
- 减少内存使用
- 加快模型加载
- 支持大模型

**注意事项**：
- 需要 mmap 支持
- 可能影响性能

#### 3.4.2 内存锁定

```cpp
// 锁定内存防止被交换
bool llama_supports_mlock(void);
```

**优点**：
- 防止内存被交换
- 提高性能稳定性

**注意事项**：
- 需要 root 权限
- 锁定过多内存会影响其他进程

#### 3.4.3 KV Cache 优化

```cpp
// 调整 KV Cache 大小
params.n_ctx = 4096;  // 上下文长度
params.f16_kv = true; // 使用 FP16 KV Cache
```

**优化建议**：
- 使用 FP16 KV Cache 减少内存
- 调整上下文长度适应需求
- 启用 KV Cache 压缩

### 3.5 推理优化

#### 3.5.1 Flash Attention

```cpp
// 启用 Flash Attention
params.flash_attn = true;
```

**优点**：
- 减少内存使用
- 提高注意力计算速度
- 支持长上下文

**注意事项**：
- 需要硬件支持
- 可能影响精度

#### 3.5.2 投机解码

```cpp
// 启用投机解码
params.n_draft = 16;  // 投机 token 数
```

**优点**：
- 提高生成速度
- 特别是对于确定性模型

**注意事项**：
- 需要合适的草案模型
- 可能影响质量

#### 3.5.3 并行解码

```cpp
// 并行生成多个候选
params.n_parallel = 4;  // 并行候选数
```

**优点**：
- 提高吞吐量
- 适用于需要多个候选的场景

**注意事项**：
- 增加内存使用
- 增加计算量

## 4. 性能评估方法

### 4.1 吞吐量评估

```cpp
// 计算吞吐量 (tokens/second)
double throughput = n_tokens / (time_us / 1000000.0);

// 提示吞吐量
double prompt_throughput = n_prompt_tokens / (prompt_time_us / 1000000.0);

// 生成吞吐量
double generation_throughput = n_gen_tokens / (gen_time_us / 1000000.0);
```

### 4.2 延迟评估

```cpp
// 计算延迟 (ms)
double latency_ms = time_us / 1000.0;

// 首个 token 延迟
double ttft_ms = first_token_time_us / 1000.0;

// 每个 token 延迟
double tpot_ms = (total_time_us - first_token_time_us) / (n_tokens - 1) / 1000.0;
```

### 4.3 资源利用率

```cpp
// CPU 利用率
double cpu_usage = get_cpu_usage();

// GPU 利用率
double gpu_usage = get_gpu_usage();

// 内存使用
size_t memory_usage = get_memory_usage();

// GPU 内存使用
size_t gpu_memory_usage = get_gpu_memory_usage();
```

### 4.4 基准测试

使用 `llama-bench` 工具进行基准测试：

```bash
# 运行基准测试
./tools/llama-bench/llama-bench -m model.gguf -p 512 -n 128

# 多线程测试
./tools/llama-bench/llama-bench -m model.ggml -t 1,2,4,8,16

# GPU 测试
./tools/llama-bench/llama-bench -m model.ggml --n-gpu-layers 30
```

## 5. 常见性能问题与解决

### 5.1 启动时间长

**原因**：
- 模型加载慢
- 初始化开销大

**解决方法**：
- 使用量化模型
- 使用内存映射
- 预加载模型
- 使用更快的存储

### 5.2 首个 token 延迟高

**原因**：
- 批处理预热
- 初始化开销

**解决方法**：
- 减小批处理大小
- 预热模型
- 使用更快的后端

### 5.3 生成速度慢

**原因**：
- 计算瓶颈
- 内存瓶颈
- 批处理大小不合适

**解决方法**：
- 使用量化
- 启用 GPU 加速
- 调整批处理大小
- 使用投机解码

### 5.4 内存不足

**原因**：
- 模型太大
- 批处理太大
- KV Cache 太大

**解决方法**：
- 使用量化模型
- 减小批处理大小
- 减小上下文长度
- 使用更小的模型

### 5.5 GPU 利用率低

**原因**：
- CPU-GPU 数据传输瓶颈
- 批处理大小太小
- GPU 层数太少

**解决方法**：
- 增加 GPU 层数
- 增大批处理大小
- 使用 CUDA Streams
- 优化数据传输

## 6. 高级优化

### 6.1 混合精度

```cpp
// 使用混合精度计算
params.f16_kv = true;      // KV Cache 使用 FP16
```

### 6.2 模型并行

```cpp
// 多 GPU 推理
--split-mode layer  // 按层分割
--split-mode row    // 按行分割
```

### 6.3 张量并行

```cpp
// 张量并行加速
export LLAMA_CPP_TENSOR_PARALLEL=4
```

### 6.4 流水线并行

```cpp
// 流水线并行处理
// 实现 pipeline 模式
```

## 7. 监控与调试

### 7.1 性能监控

```cpp
// 启用性能统计
llama_perf_context_data data = ctx->perf_get_data();

// 打印性能信息
printf("load time: %.2f ms\n", data.t_load_us / 1000.0);
printf("prompt eval time: %.2f ms\n", data.t_p_eval_us / 1000.0);
printf("eval time: %.2f ms\n", data.t_eval_us / 1000.0);
```

### 7.2 日志级别

```cpp
// 设置日志级别
llama_log_set(log_callback, log_data);

// 日志级别
#define LLAMA_LOG_LEVEL_DEBUG  0
#define LLAMA_LOG_LEVEL_INFO   1
#define LLAMA_LOG_LEVEL_WARN   2
#define LLAMA_LOG_LEVEL_ERROR  3
```

### 7.3 性能分析

```bash
# 使用 perf 分析 (Linux)
perf record -g ./your_program
perf report

# 使用 NVIDIA Nsight 分析
nsys profile --stats=true ./your_program

# 使用 VTune 分析
vtune -collect hotspots ./your_program
```

## 8. 最佳实践

### 8.1 配置优化

根据硬件和应用场景选择合适的配置：

**小模型 + 高延迟容忍**：
- 量化：Q4_K_M
- 后端：CPU
- 批处理：小

**大模型 + 低延迟要求**：
- 量化：Q6_K 或 Q8_0
- 后端：GPU
- 批处理：中

**吞吐量优先**：
- 量化：Q4_K_M
- 后端：GPU
- 批处理：大
- 启用并行解码

### 8.2 资源管理

```cpp
// 合理设置线程数
int n_threads = std::min(physical_cores, 8);
params.n_threads = n_threads;

// 合理设置批处理大小
params.n_batch = 512;  // 根据内存调整
params.n_ubatch = 32;  // 根据延迟需求调整

// 合理设置 GPU 层数
params.n_gpu_layers = min(max_layers, available_memory / layer_memory);
```

### 8.3 监控与调优

```cpp
// 定期检查性能
while (running) {
    auto data = ctx->perf_get_data();
    log_performance(data);
    sleep(interval);
}

// 根据性能调整配置
adjust_configuration(performance_data);
```

## 9. 总结

llama.cpp 性能优化要点：

1. **量化模型**：减少模型大小和计算量
2. **合理后端**：根据硬件选择合适的后端
3. **批处理优化**：根据场景调整批处理大小
4. **内存优化**：使用 mmap、mlock 等
5. **高级技术**：Flash Attention、投机解码等
6. **监控调优**：持续监控和优化

遵循这些注意事项可以有效优化 llama.cpp 的推理性能。