## 将 llama2.c 模型转换为 ggml

此示例从 [llama2.c](https://github.com/karpathy/llama2.c) 项目读取权重并将其保存为 ggml 兼容格式。默认使用 `models/ggml-vocab.bin` 中可用的词汇表。

要转换模型，首先从 [llama2.c](https://github.com/karpathy/llama2.c) 仓库下载模型。

```
用法: ./llama-convert-llama2c-to-ggml [选项]

选项:
  -h, --help                       显示此帮助消息并退出
  --copy-vocab-from-model FNAME    从中复制词汇表的 GGUF llama 模型或 llama2.c 词汇表路径（默认为 'models/7B/ggml-model-f16.gguf'）
  --llama2c-model FNAME            [必需] 从中加载 Karpathy 的 llama2.c 模型的模型路径
  --llama2c-output-model FNAME     保存转换后的 llama2.c 模型的模型路径（默认为 'ak_llama_model.bin'）
```

使用来自 [karpathy/tinyllamas](https://huggingface.co/karpathy/tinyllamas) 的模型的示例命令如下：

`$ ./llama-convert-llama2c-to-ggml --copy-vocab-from-model llama-2-7b-chat.gguf.q2_K.bin --llama2c-model stories42M.bin --llama2c-output-model stories42M.gguf.bin`

注意：`stories260K.bin` 的词汇表应该是其自己的分词器 `tok512.bin`，可在 [karpathy/tinyllamas/stories260K](https://huggingface.co/karpathy/tinyllamas/tree/main/stories260K) 中找到。

现在您可以使用如下命令来使用模型：

`$ ./llama-cli -m stories42M.gguf.bin -p "One day, Lily met a Shoggoth" -n 500 -c 256`
