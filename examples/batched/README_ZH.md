# llama.cpp/example/batched

本示例演示如何从给定提示词进行批处理文本生成。

## 快速开始

```bash
./llama-batched -m ./models/llama-7b-v2/ggml-model-f16.gguf -p "Hello my name is" -np 4 --kv-unified
```

## 示例输出

```
...
main: n_len = 32, n_ctx = 2048, n_parallel = 4, n_kv_req = 113

 Hello my name is

main: generating 4 sequences ...

main: stream 0 finished
main: stream 1 finished
main: stream 2 finished
main: stream 3 finished

sequence 0:

Hello my name is Shirley. I am a 25-year-old female who has been working for over 5 years as a b

sequence 1:

Hello my name is Renee and I'm a 32 year old female from the United States. I'm looking for a man between

sequence 2:

Hello my name is Diana. I am looking for a housekeeping job. I have experience with children and have my own transportation. I am

sequence 3:

Hello my name is Cody. I am a 3 year old neutered male. I am a very friendly cat. I am very playful and

main: decoded 108 tokens in 3.57 s, speed: 30.26 t/s

llama_print_timings:        load time =   587.00 ms
llama_print_timings:      sample time =     2.56 ms /   112 runs   (    0.02 ms per token, 43664.72 tokens per second)
llama_print_timings: prompt eval time =  4089.11 ms /   118 tokens (   34.65 ms per token,    28.86 tokens per second)
llama_print_timings:        eval time =     0.00 ms /     1 runs   (    0.00 ms per token,      inf tokens per second)
llama_print_timings:       total time =  4156.04 ms
```

## 参数说明

| 参数 | 说明 |
|------|------|
| `-m, --model` | 模型文件路径（必需） |
| `-p, --prompt` | 输入提示词 |
| `-np, --n-parallel` | 并行生成的序列数量 |
| `-n, --n-predict` | 每个序列生成的 token 数量 |
| `-c, --ctx-size` | 上下文窗口大小 |
| `-ngl, --n-gpu-layers` | GPU 层数 |
| `-t, --threads` | 使用的线程数 |
| `--kv-unified` | 使用统一的 KV 缓存 |

## 功能特点

- **并行生成**：同时生成多个文本序列
- **共享上下文**：所有序列共享相同的输入提示词
- **独立输出**：每个序列独立完成生成过程

## 使用场景

- 生成多个候选文本
- 批量文本生成任务
- 测试模型的多样性
- 服务器端多请求处理

## 性能优化

- 批处理可以提高 GPU 利用率
- 共享 KV 缓存减少内存使用
- 适合在服务器环境中使用

## 相关示例

- [simple](../simple/README_ZH.md) - 单序列文本生成
- [batched.swift](../batched.swift/README_ZH.md) - Swift 版本
- [parallel](../parallel/README_ZH.md) - 并行请求处理模拟

## Swift 版本

Swift 版本示例位于 [batched.swift](../batched.swift/README_ZH.md) 目录。