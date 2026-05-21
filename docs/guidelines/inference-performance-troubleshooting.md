# llama.cpp 推理性能问题排查方法

本文档总结 llama.cpp 项目中推理性能问题的排查方法和优化建议。

## 1. 性能问题识别

### 1.1 性能指标

**关键性能指标**：
- **吞吐量**：tokens/second
- **延迟**：首个 token 延迟 (TTFT)、每个 token 延迟 (TPOT)
- **资源利用率**：CPU、GPU、内存使用率

```cpp
// 获取性能数据
llama_perf_context_data perf = llama_perf_context_get_data(ctx);

// 计算性能指标
double t_prompt_eval_ms = perf.t_p_eval_ms;
double t_eval_ms = perf.t_eval_ms;

double prompt_speed = perf.n_p_eval / (perf.t_p_eval_us / 1000000.0);
double generation_speed = perf.n_eval / (perf.t_eval_us / 1000000.0);

printf("Prompt eval: %d tokens, %.2f ms, %.2f tokens/s\n",
       perf.n_p_eval, t_prompt_eval_ms, prompt_speed);
printf("Generation: %d tokens, %.2f ms, %.2f tokens/s\n",
       perf.n_eval, t_eval_ms, generation_speed);
```

### 1.2 性能基准

**正常性能参考**：
- CPU (现代多核): 5-20 tokens/s (取决于模型大小)
- GPU (RTX 3080): 50-100 tokens/s (Q4 量化)
- GPU (A100): 200-500 tokens/s (Q4 量化)

```bash
# 运行基准测试
./tools/llama-bench/llama-bench -m model.gguf -p 512 -n 128

# 多线程测试
./tools/llama-bench/llama-bench -m model.gguf -t 1,2,4,8,16
```

## 2. 性能瓶颈分析

### 2.1 识别瓶颈

#### 2.1.1 CPU 瓶颈

**症状**：
- CPU 利用率高 (90%+)
- 吞吐量低
- GPU 利用率低

**检查方法**：
```bash
# 监控 CPU 使用
top -H  # 查看线程
htop    # 可视化

# 性能分析
perf top -p $(pidof your_program)
```

**原因**：
- 线程数不足
- CPU 核心利用率不高
- SIMD 未启用
- 未使用 GPU

#### 2.1.2 内存瓶颈

**症状**：
- 内存使用率高
- 缓存未命中率高
- CPU 等待内存

**检查方法**：
```bash
# 监控内存使用
vmstat 1
free -h

# 查看缓存命中率
perf stat -e cache-references,cache-misses your_program
```

**原因**：
- 模型太大
- 未使用内存映射
- 内存访问模式不佳

#### 2.1.3 GPU 瓶颈

**症状**：
- GPU 利用率低
- CPU-GPU 数据传输慢
- GPU 内存不足

**检查方法**：
```bash
# NVIDIA GPU
nvidia-smi
nvidia-smi dmon

# NVIDIA 性能分析
nsys profile --stats=true your_program
nvprof your_program

# AMD GPU (ROCm)
rocm-smi
rocprof your_program
```

**原因**：
- GPU 层数太少
- 批处理大小不合适
- 数据传输开销大

#### 2.1.4 I/O 瓶颈

**症状**：
- 启动时间长
- 模型加载慢
- 磁盘 I/O 等待

**检查方法**：
```bash
# 监控 I/O
iotop
iostat -x 1

# 磁盘速度
dd if=/dev/zero of=test bs=1M count=1024 conv=fdatasync
```

**原因**：
- 使用 HDD 而非 SSD
- 网络存储
- 未使用内存映射

### 2.2 性能分析工具

#### 2.2.1 内置性能统计

```cpp
// 获取内存使用明细
llama_memory_breakdown breakdown = llama_memory_breakdown(ctx);

// 重置性能统计
llama_perf_context_reset(ctx);

// 打印性能信息
void print_perf_stats(llama_context * ctx) {
    llama_perf_context_data perf = llama_perf_context_get_data(ctx);

    printf("\n=== Performance Stats ===\n");
    printf("Load time:     %.2f ms\n", perf.t_load_us / 1000.0);
    printf("Prompt eval:   %.2f ms (%d tokens, %.2f tokens/s)\n",
           perf.t_p_eval_us / 1000.0, perf.n_p_eval,
           perf.n_p_eval / (perf.t_p_eval_us / 1000000.0));
    printf("Generation:    %.2f ms (%d tokens, %.2f tokens/s)\n",
           perf.t_eval_us / 1000.0, perf.n_eval,
           perf.n_eval / (perf.t_eval_us / 1000000.0));
    printf("Total time:    %.2f ms\n",
           (perf.t_p_eval_us + perf.t_eval_us) / 1000.0);
    printf("\n");
}
```

#### 2.2.2 外部工具

**Linux perf**：
```bash
# 记录性能数据
perf record -g -F 99 your_program

# 查看报告
perf report

# 查看热点函数
perf report --stdio | head -20
```

**Intel VTune**：
```bash
# 热点分析
vtune -collect hotspots your_program

# 内存分析
vtune -collect memory-access your_program

# 线程分析
vtune -collect threading your_program
```

**NVIDIA Nsight**：
```bash
# 记录 GPU 性能
nsys profile --stats=true your_program

# 查看报告
nsys stats report.qdrep
```

## 3. 常见性能问题

### 3.1 提示处理慢

**问题**：提示 token 处理速度慢

**原因**：
- 批处理大小不合适
- 未使用 BLAS
- 线程数不足

**解决方案**：
```cpp
// 增大批处理大小
llama_context_params params = llama_context_default_params();
params.n_batch = 1024;  // 增大批处理

// 使用 BLAS
cmake -B build -DGGML_BLAS=ON -DGGML_BLAS_VENDOR=OpenBLAS

// 增加线程数
params.n_threads = 8;
```

### 3.2 生成速度慢

**问题**：生成 token 速度慢

**原因**：
- 未使用 GPU
- 量化级别不合适
- 投机解码未启用

**解决方案**：
```cpp
// 使用 GPU
params.n_gpu_layers = 30;  // 将更多层放到 GPU

// 使用更激进的量化
// Q4_K_M 比 Q6_K 快

// 启用投机解码
params.n_draft = 16;

// 启用 Flash Attention
params.flash_attn = true;
```

### 3.3 首个 token 延迟高

**问题**：首个 token 输出延迟高

**原因**：
- 冷启动开销
- 模型预热未完成
- 初始化开销

**解决方案**：
```cpp
// 预热模型
void warmup_model(llama_context * ctx) {
    llama_batch batch = llama_batch_init(1, 0, 1);
    batch.n_tokens = 1;
    llama_decode(ctx, batch);
    llama_batch_free(batch);
}

// 启用 KV Cache 重用
params.cyclic_k_shift = 32;  // 循环 KV Cache
```

### 3.4 内存使用高

**问题**：内存使用过高

**原因**：
- 模型未量化
- 上下文长度过大
- KV Cache 过大

**解决方案**：
```cpp
// 使用量化
params.f16_kv = true;  // KV Cache 使用 FP16

// 减小上下文长度
params.n_ctx = 2048;

// 启用 KV Cache 压缩
params.type_k = GGML_TYPE_Q4_K;
params.type_v = GGML_TYPE_Q4_K;
```

### 3.5 GPU 利用率低

**问题**：GPU 利用率低

**原因**：
- GPU 层数太少
- 批处理大小太小
- CPU-GPU 数据传输瓶颈

**解决方案**：
```cpp
// 增加 GPU 层数
params.n_gpu_layers = min(max_layers, gpu_layers);

// 增大批处理
params.n_batch = 512;
params.n_ubatch = 32;

// 使用 CUDA Streams
params.cuda_streams = 4;
```

## 4. 优化策略

### 4.1 量化优化

```bash
# 量化模型
./tools/quantize/quantize model.gguf model-q4_k_m.gguf Q4_K_M

# 比较不同量化的性能
./tools/llama-bench/llama-bench -m model-f16.gguf
./tools/llama-bench/llama-bench -m model-q4_k_m.gguf
./tools/llama-bench/llama-bench -m model-q5_k_m.gguf
```

### 4.2 批处理优化

```cpp
// 根据应用场景调整批处理
if (low_latency_required) {
    params.n_batch = 128;  // 低延迟
} else if (high_throughput_required) {
    params.n_batch = 1024;  // 高吞吐
} else {
    params.n_batch = 512;  // 平衡
}
```

### 4.3 线程优化

```cpp
// 设置合理的线程数
int physical_cores = get_physical_core_count();
params.n_threads = physical_cores;

// 区分编码和解码线程
params.n_threads = physical_cores;
params.n_threads_batch = physical_cores / 2;
```

### 4.4 后端优化

```cpp
// 根据硬件选择最佳后端
if (has_nvidia_gpu()) {
    // 使用 CUDA
    cmake -B build -DGGML_CUDA=ON
} else if (is_macos()) {
    // 使用 Metal
    cmake -B build -DGGML_METAL=ON
} else {
    // 使用 CPU + BLAS
    cmake -B build -DGGML_BLAS=ON -DGGML_BLAS_VENDOR=OpenBLAS
}
```

## 5. 性能调优流程

### 5.1 基准测试

```bash
# 建立基准
./tools/llama-bench/llama-bench -m model.gguf -p 512 -n 128 > baseline.txt
```

### 5.2 问题识别

```bash
# 分析性能数据
# 比较与基准的差异
# 识别瓶颈
```

### 5.3 优化尝试

```bash
# 尝试不同配置
# 每次修改一个参数
# 重新测试
```

### 5.4 结果验证

```bash
# 比较优化前后性能
# 确认改进效果
# 检查副作用
```

## 6. 监控与诊断

### 6.1 实时监控

```bash
# CPU 使用
watch -n 1 'ps aux | grep your_program'

# GPU 使用
watch -n 1 nvidia-smi

# 内存使用
watch -n 1 'cat /proc/$(pidof your_program)/status | grep Vm'
```

### 6.2 日志分析

```cpp
// 启用详细日志
llama_log_set(log_callback, nullptr);

// 记录关键事件
LLAMA_LOG_INFO("Model loaded in %.2f ms\n", load_time_ms);
LLAMA_LOG_INFO("Prompt processed in %.2f ms\n", prompt_time_ms);
LLAMA_LOG_INFO("Generated %d tokens in %.2f ms\n", n_tokens, gen_time_ms);
```

### 6.3 性能剖析

```bash
# 使用 perf 进行 CPU 剖析
perf record -F 99 -g your_program
perf report

# 使用 Nsight 进行 GPU 剖析
nsys profile your_program
```

## 7. 最佳实践

### 7.1 配置优化

**根据硬件配置**：
```cpp
// CPU 优化
params.n_threads = min(physical_cores, 8);
params.n_batch = 512;

// GPU 优化
params.n_gpu_layers = min(model_layers, gpu_layers);
params.n_batch = 1024;
params.n_ubatch = 32;
```

**根据应用场景**：
```cpp
// 低延迟优先
params.n_batch = 128;
params.n_ubatch = 8;
params.flash_attn = true;

// 高吞吐优先
params.n_batch = 1024;
params.n_ubatch = 32;
params.n_parallel = 4;
```

### 7.2 资源管理

```cpp
// 监控资源使用
void monitor_resources() {
    while (running) {
        auto cpu_usage = get_cpu_usage();
        auto mem_usage = get_memory_usage();
        auto gpu_usage = get_gpu_usage();

        printf("CPU: %.1f%%, Memory: %.1f%%, GPU: %.1f%%\n",
               cpu_usage, mem_usage, gpu_usage);

        sleep(1);
    }
}
```

### 7.3 性能测试

```bash
# 系统性能测试
./tools/llama-bench/llama-bench -m model.gguf -t 1,2,4,8

# 压力测试
./tools/llama-bench/llama-bench -m model.gguf -n 1024

# 长文本测试
./tools/llama-bench/llama-bench -m model.gguf -p 4096
```

## 8. 总结

llama.cpp 推理性能问题排查要点：

1. **性能指标**：监控吞吐量、延迟、资源利用率
2. **瓶颈识别**：使用工具分析 CPU、内存、GPU、I/O 瓶颈
3. **常见问题**：提示慢、生成慢、延迟高、内存高、GPU 利用率低
4. **优化策略**：量化、批处理、线程、后端优化
5. **调优流程**：基准测试 → 问题识别 → 优化尝试 → 结果验证
6. **监控诊断**：实时监控、日志分析、性能剖析

遵循这些方法可以有效识别和解决推理性能问题。