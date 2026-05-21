# llama.cpp/example/passkey

密钥检索任务是用于衡量语言模型从长上下文中回忆信息能力的评估方法。

更多信息请参见以下 PR：

- https://github.com/ggml-org/llama.cpp/pull/3856
- https://github.com/ggml-org/llama.cpp/pull/4810

### 使用方法

```bash
llama-passkey -m ./models/llama-7b-v2/ggml-model-f16.gguf --junk 250
```
