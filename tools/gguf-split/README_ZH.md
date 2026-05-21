# GGUF 分割工具

GGUF 文件的分割/合并命令行工具。

**命令行选项：**

- `--split`: 将 GGUF 分割为多个 GGUF 文件，默认操作
- `--split-max-size`: 每个分割文件的最大大小，单位为 `M` 或 `G`，例如 `500M` 或 `2G`
- `--split-max-tensors`: 每个分割文件中的最大张量数，默认为 128
- `--merge`: 将多个 GGUF 文件合并为一个 GGUF 文件。您只需要指定要合并的第一个 GGUF 文件的名称和合并后的 GGUF 文件名称，CLI 将在同一文件夹中找到所需的其他 GGUF 文件

## 用法示例

### 分割 GGUF 文件

按最大张量数分割：
```bash
./llama-gguf-split --split --split-max-tensors 128 input_model.gguf output_prefix
```

按文件大小分割（例如每个文件最大 2GB）：
```bash
./llama-gguf-split --split --split-max-size 2G input_model.gguf output_prefix
```

### 合并 GGUF 文件

```bash
./llama-gguf-split --merge output_prefix.0001.gguf merged_model.gguf
```

工具会自动在同一目录中查找其他分割文件（output_prefix.0002.gguf、output_prefix.0003.gguf 等）。

## 注意事项

1. 分割后会在指定的输出前缀后面添加 `.0001.gguf`、`.0002.gguf` 等后缀
2. 合并时只需指定第一个分割文件
3. 合并操作需要所有分割文件都在同一目录下
4. 确保在分割和合并过程中不要修改任何分割文件