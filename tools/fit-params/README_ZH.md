# fit-params

llama.cpp 二进制文件可以自动拟合模型的预期内存使用以适应运行时可用的设备内存，这通过以 `-fit`/`--fit` 开头的 CLI 参数控制。
在内部，代码调用 `llama_params_fit` 来调整 `llama_model_params` 和 `llama_context_params` 结构。
`llama-fit-params` 是一个简单的实用程序，将这些调整对应的 CLI 参数打印到 stdout。
示例用法：

``` bash
# 首先，运行 llama-fit-params 并将结果保存到文件中：
> ./build/bin/llama-fit-params --model /opt/models/qwen_3-30b3a-f16.gguf | tee args.txt
ggml_cuda_init: GGML_CUDA_FORCE_MMQ:    no
ggml_cuda_init: GGML_CUDA_FORCE_CUBLAS: no
ggml_cuda_init: found 1 CUDA devices:
  Device 0: NVIDIA GeForce RTX 4090, compute capability 8.9, VMM: yes
build: 6895 (4341dc8bc) with cc (GCC) 15.2.1 20250813 for x86_64-pc-linux-gnu
llama_params_fit_impl: projected to use 61807 MiB of device memory vs. 24077 MiB of free device memory
llama_params_fit_impl: cannot fulfill margin of 1024 MiB, need to reduce device memory by 42444 MiB
llama_params_fit_impl: context size reduced from 40960 to 4096 -> need 3456 MiB less memory in total
llama_params_fit_impl: with only dense weights in device memory there is a total surplus of 16164 MiB
llama_params_fit_impl: distributing layers across devices with overflow to next device/system memory:
llama_params_fit_impl:   - CUDA0 (NVIDIA GeForce RTX 4090): 48 layers (34 overflowing),  19187 MiB used,   1199 MiB free
llama_params_fit: successfully fit params to free device memory
llama_params_fit: fitting params to free memory took 1.15 seconds
Printing fitted CLI arguments to stdout...
-c 4096 -ngl 48 -ot blk\.14\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.15\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.16\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.17\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.18\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.19\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.20\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.21\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.22\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.23\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.24\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.25\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.26\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.27\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.28\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.29\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.30\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.31\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.32\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.33\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.34\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.35\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.36\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.37\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.38\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.39\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.40\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.41\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.42\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.43\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.44\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.45\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.46\.ffn_(up|down|gate)_(ch|)exps=CPU,blk\.47\.ffn_(up|down|gate)_(ch|)exps=CPU

# 接下来，将这些结果用于 llama.cpp 二进制文件：
> cat args.txt | xargs ./build/bin/llama-server --model /opt/models/qwen_3-30b3a-f16.gguf
ggml_cuda_init: GGML_CUDA_FORCE_MMQ:    no
ggml_cuda_init: GGML_CUDA_FORCE_CUBLAS: no
ggml_cuda_init: found 1 CUDA devices:
  Device 0: NVIDIA GeForce RTX 4090, compute capability 8.9, VMM: yes
build: 6895 (4341dc8bc) with cc (GCC) 15.2.1 20250813 for x86_64-pc-linux-gnu
system info: n_threads = 16, n_threads_batch = 16, total_threads = 32

system info: n_threads = 16 (n_threads_batch = 16) / 32 | CUDA : ARCHS = 890 | USE_GRAPHS = 1 | PEER_MAX_BATCH_SIZE = 128 | CPU : SSE3 = 1 | SSSE3 = 1 | AVX = 1 | AVX_VNNI = 1 | AVX2 = 1 | F16C = 1 | FMA = 1 | BMI2 = 1 | AVX512 = 1 | AVX512_VBMI = 1 | AVX512_VNNI = 1 | AVX512_BF16 = 1 | LLAMAFILE = 1 | OPENMP = 1 | REPACK = 1 |

main: binding port with default address family
main: HTTP server is listening, hostname: 127.0.0.1, port: 8080, http threads: 31
main: loading model
srv    load_model: loading model '/opt/models/qwen_3-30b3a-f16.gguf'
llama_params_fit_impl: projected to use 19187 MiB of device memory vs. 24077 MiB of free device memory
llama_params_fit_impl: will leave 1199 >= 1024 MiB of free device memory, no changes needed
llama_params_fit: successfully fit params to free device memory
llama_params_fit: fitting params to free memory took 0.28 seconds
[...]
main: server is listening on http://127.0.0.1:8080 - starting the main loop
srv  update_slots: all slots are idle
^Csrv    operator(): operator(): cleaning up before exit...

llama_memory_breakdown_print: | 内存分解 [MiB] | 总计   空闲     自身   模型   上下文   计算    未计入 |
llama_memory_breakdown_print: |   - CUDA0 (RTX 4090)   | 24077 =  945 + (19187 = 17904 +     384 +     898) +        3945 |
llama_memory_breakdown_print: |   - 主机               |                 58271 = 58259 +       0 +      12                |
```

## 功能说明

`fit-params` 工具用于自动调整模型参数以适应可用设备内存。它分析模型的大小和可用内存，并生成适当的命令行参数，以便在有限内存的设备上运行大型模型。

### 主要功能

1. **内存分析**：计算模型所需的总内存和可用内存
2. **参数调整**：自动调整上下文大小、GPU 层数等参数
3. **层分配**：决定哪些层在 GPU 上运行，哪些层在 CPU 上运行
4. **边距管理**：确保保留足够的内存边距（默认为 1024 MiB）

### 调整策略

工具会按以下优先级调整参数：
1. 减少上下文大小（`-c`）
2. 减少 GPU 层数（`-ngl`）
3. 将特定张量卸载到 CPU（`-ot`）
4. 将特定层卸载到 CPU

### 输出参数

生成的参数包括：
- `-c`：调整后的上下文大小
- `-ngl`：GPU 层数量
- `-ot`：张量覆盖规则（将特定张量类型卸载到 CPU）

## 使用场景

- **显存不足**：当模型太大而无法完全加载到 GPU 时
- **多设备环境**：在多个 GPU 之间分配模型
- **自动化部署**：自动为不同硬件配置生成最佳参数

## 注意事项

1. 工具需要模型文件进行分析
2. 生成的参数应根据实际使用情况进行验证
3. 某些调整可能会影响推理速度，需要在速度和内存使用之间权衡