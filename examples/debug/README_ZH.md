# llama.cpp/examples/debug

这是一个用于调试模型的实用工具，通过注册一个回调来记录 GGML 操作和张量数据。它还可以存储生成的 logits 或嵌入以及提示和令牌 ID，以便与原始模型进行比较。

### 使用方法

```shell
llama-debug \
  --hf-repo ggml-org/models \
  --hf-file phi-2/ggml-model-q4_0.gguf \
  --model phi-2-q4_0.gguf \
  --prompt hello \
  --save-logits \
  --verbose
```
张量数据作为调试记录，需要 `--verbose` 标志。这样做的原因是，虽然对于具有许多层的模型很有用，但可能会有大量输出。您可以使用 `--tensor-filter` 选项过滤张量名称。

推荐的方法是首先运行不带 `--verbose` 的程序，看看生成的 logits/嵌入是否接近原始模型。如果不接近，那么可能需要逐张量检查，在这种情况下，启用 `--verbose` 标志以及 `--tensor-filter` 来专注于特定张量是很有用的。

### 选项
此示例支持所有标准的 `llama.cpp` 选项，并且还接受以下选项：
```console
$ llama-debug --help
...

----- example-specific params -----

--save-logits                           将最终 logits 保存到文件以供验证（默认：false）
--logits-output-dir PATH                保存 logits 输出文件的目录（默认：data）
--tensor-filter REGEX                   过滤张量名称以进行调试输出（正则表达式模式，可以指定多次）
```

### 输出文件

当启用 `--save-logits` 时，将在输出目录中创建以下文件：

* `llamacpp-<model>[-embeddings].bin`        - 二进制输出（logits 或嵌入）
* `llamacpp-<model>[-embeddings].txt`        - 文本输出（logits 或嵌入，每行一个）
* `llamacpp-<model>[-embeddings]-prompt.txt` - 提示文本和令牌 ID
* `llamacpp-<model>[-embeddings]-tokens.bin` - 用于程序化比较的二进制令牌 ID

这些文件可以与原始模型的输出进行比较，以验证转换后的模型。
