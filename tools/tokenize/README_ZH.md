# llama.cpp/tools/tokenize

分词程序使用给定模型对提示进行分词，并将生成的 token 打印到标准输出。

它需要一个模型文件、一个提示，以及可选的其他标志来控制分词器的行为。

    可能的选项：

    -h, --help                           打印此帮助并退出
    -m MODEL_PATH, --model MODEL_PATH    模型路径
    --ids                                如果给定，仅打印数字 token ID，而不打印 token 字符串。
                                         输出格式看起来像 [1, 2, 3]，即可以由 Python 解析。
    -f PROMPT_FNAME, --file PROMPT_FNAME 从文件中读取提示
    -p PROMPT, --prompt PROMPT           从参数中读取提示
    --stdin                              从标准输入中读取提示
    --no-bos                             即使模型通常使用 BOS token，也不要在提示中添加 BOS token
    --no-escape                          不要转义输入（例如 \n、\t 等）
    --no-parse-special                   不要解析控制 token
    --log-disable                        禁用日志。使加载模型时的 stderr 静音
    --show-count                         打印 token 总数

## 功能说明

`tokenize` 工具用于对文本进行分词，将文本转换为模型可以理解的 token 序列。这对于理解模型如何处理输入文本非常有用。

### 主要功能

1. **分词转换**：将文本输入转换为 token ID 序列
2. **多输入源**：支持从文件、命令行参数或标准输入读取文本
3. **特殊 token 处理**：可以控制是否处理和添加特殊 token（如 BOS）
4. **字符转义**：支持或禁用转义序列处理

## 使用示例

### 基本用法

从命令行参数读取提示并显示 token 及其文本表示：

```bash
./llama-tokenize -m model.gguf -p "Hello world"
```

### 仅显示 Token ID

如果您只需要数字形式的 token ID：

```bash
./llama-tokenize -m model.gguf --ids -p "Hello world"
```

输出格式：`[1, 3155, 1125, 2]`

### 从文件读取提示

从文本文件中读取提示：

```bash
./llama-tokenize -m model.gguf -f input.txt
```

### 从标准输入读取

从标准输入读取提示（适合管道操作）：

```bash
echo "Hello world" | ./llama-tokenize -m model.gguf --stdin
```

### 禁用 BOS Token

某些模型可能会在开头添加 BOS (Beginning of Sequence) token。您可以禁用它：

```bash
./llama-tokenize -m model.gguf --no-bos -p "Hello"
```

### 禁用特殊 token 解析

某些模型使用特殊 token（如 `<|im_start|>`）。您可以选择不解析它们：

```bash
./llama-tokenize -m model.gguf --no-parse-special -p "Some prompt <|im_start|>with special tokens"
```

### 显示 Token 总数

如果您只想知道文本中有多少个 token：

```bash
./llama-tokenize -m model.gguf --show-count -p "Hello world"
```

输出示例：`Total number of tokens: 4`

### 禁用日志输出

如果您想要安静的输出（加载模型时不显示日志信息）：

```bash
./llama-tokenize -m model.gguf --log-disable -p "Hello"
```

## Windows 支持

在 Windows 上，`tokenize` 工具正确处理非 ASCII 字符（如中文字符），确保在 cmd.exe 中使用时不会出现错误。

## 输出格式说明

- **默认模式**：每行显示一个 token，格式为：`<token_id> -> '<token_text>'`
- **--ids 模式**：输出 Python 可解析的列表格式，如 `[1, 2, 3]`
- **--show-count 模式**：在输出最后显示 token 总数

## 注意事项

1. 模型必须首先加载，分词器从模型的词汇表中读取
2. 不同的模型有不同的分词器，相同的文本可能产生不同的 token 序列
3. 特殊 token 的处理取决于模型和配置
4. Windows 上的非 ASCII 字符支持需要特殊处理，此工具已内置支持

## 与服务器 API 的区别

这个工具与服务器 `/tokenize` API 端点的区别：
- 服务器 API 提供更丰富的选项（如解析特殊 token、返回片段等）
- 工具主要用于快速测试和调试
- 服务器 API 适合在应用程序中使用

## 相关文档

- 服务器 `/tokenize` 端点：`tools/server/README.md` 中的 POST `/tokenize` 部分
- 服务器 `/detokenize` 端点：将 token 转换回文本