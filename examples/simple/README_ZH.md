# llama.cpp/example/simple

本示例演示 llama.cpp 的最小化用法，用于根据给定的提示词生成文本。

## 快速开始

运行以下命令，确保使用正确的模型路径：

```bash
./llama-simple -m ./models/llama-7b-v2/ggml-model-f16.gguf "Hello my name is"
```

## 示例输出

```
...
main: n_len = 32, n_ctx = 2048, n_parallel = 1, n_kv_req = 32

 Hello my name is Shawn and I'm a 20 year old male from the United States. I'm a 20 year old

main: decoded 27 tokens in 2.31 s, speed: 11.68 t/s

llama_print_timings:        load time =   579.15 ms
llama_print_timings:      sample time =     0.72 ms /    28 runs   (    0.03 ms per token, 38888.89 tokens per second)
llama_print_timings: prompt eval time =   655.63 ms /    10 tokens (   65.56 ms per token,    15.25 tokens per second)
llama_print_timings:        eval time =  2180.97 ms /    27 runs   (   80.78 ms per token,    12.38 tokens per second)
llama_print_timings:       total time =  2891.13 ms
```

## 参数说明

| 参数 | 说明 |
|------|------|
| `-m, --model` | 模型文件路径（必需） |
| 提示词文本 | 直接在命令行末尾输入提示词 |
| `-n, --n-predict` | 生成的 token 数量 |
| `-c, --ctx-size` | 上下文窗口大小 |
| `-ngl, --n-gpu-layers` | GPU 层数 |
| `-t, --threads` | 使用的线程数 |
| `-b, --batch-size` | 批处理大小 |

## 使用场景

- 测试模型加载和基本推理功能
- 理解 llama.cpp API 的基本用法
- 快速验证模型文件是否可用

## 注意事项

- 此示例使用默认的采样参数
- 生成的文本长度受 `-n` 参数限制
- 建议使用较小的上下文窗口（默认 2048）进行快速测试

## 相关示例

- [simple-chat](../simple-chat/README_ZH.md) - 使用聊天模板的交互式示例
- [batched](../batched/README_ZH.md) - 批处理文本生成