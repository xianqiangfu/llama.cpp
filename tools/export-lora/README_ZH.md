# export-lora

将 LORA 适配器应用于基础模型并导出生成的模型。

```
usage: llama-export-lora [options]

options:
  -m,    --model                  模型路径，用于加载基础模型（默认 ''）
         --lora FNAME             LoRA 适配器路径（可以重复以使用多个适配器）
         --lora-scaled FNAME S    带有用户定义缩放 S 的 LoRA 适配器路径（可以重复以使用多个适配器）
  -t,    --threads N              计算期间使用的线程数（默认：4）
  -o,    --output FNAME           输出文件（默认：'ggml-lora-merged-f16.gguf'）
```

## 功能说明

`export-lora` 工具用于将一个或多个 LoRA (Low-Rank Adaptation) 适配器合并到基础模型中，生成一个新的模型文件。合并后的模型包含 LoRA 适配器的修改，可以在不需要额外加载适配器的情况下使用。

### 主要功能

1. **单个适配器合并**：将一个 LoRA 适配器应用到基础模型
2. **多个适配器合并**：同时应用多个 LoRA 适配器
3. **缩放控制**：为每个适配器指定自定义缩放因子
4. **灵活的输出**：指定输出文件名和格式

### 使用场景

- **模型部署**：将训练好的 LoRA 适配器永久合并到模型中
- **模型分发**：合并后的模型更容易部署和分发
- **性能优化**：避免运行时加载多个适配器的开销

## 示例

### 基本用法 - 单个 LoRA 适配器

```bash
./bin/llama-export-lora \
    -m open-llama-3b-v2.gguf \
    -o open-llama-3b-v2-english2tokipona-chat.gguf \
    --lora lora-open-llama-3b-v2-english2tokipona-chat-LATEST.gguf
```

### 多个 LoRA 适配器 - 相同缩放

```bash
./bin/llama-export-lora \
    -m your_base_model.gguf \
    -o your_merged_model.gguf \
    --lora lora_task_A.gguf \
    --lora lora_task_B.gguf
```

### 多个 LoRA 适配器 - 自定义缩放

```bash
./bin/llama-export-lora \
    -m your_base_model.gguf \
    -o your_merged_model.gguf \
    --lora-scaled lora_task_A.gguf 0.5 \
    --lora-scaled lora_task_B.gguf 0.5
```

### 混合使用缩放和默认缩放

```bash
./bin/llama-export-lora \
    -m your_base_model.gguf \
    -o your_merged_model.gguf \
    --lora-scaled lora_task_A.gguf 0.7 \
    --lora-scaled lora_task_B.gguf 0.3 \
    --lora lora_task_C.gguf  # 使用默认缩放 1.0
```

## 参数说明

- `-m, --model`：基础模型的路径（必需）
- `--lora`：LoRA 适配器文件路径（可重复）
- `--lora-scaled`：带有缩放因子的 LoRA 适配器文件路径，格式为 `FNAME:SCALE`（可重复）
- `-t, --threads`：用于计算的线程数（默认：4）
- `-o, --output`：输出文件路径（默认：'ggml-lora-merged-f16.gguf'）

## 注意事项

1. LoRA 适配器必须与基础模型兼容
2. 多个适配器修改相同参数时，效果是累加的
3. 合并后的模型不能"撤销"适配器的修改
4. 输出模型的大小会包含基础模型和适配器的权重

## 与运行时应用 LoRA 的区别

- **export-lora**：永久合并，生成新的模型文件，适合长期使用
- **运行时应用**：在推理时动态加载，灵活性更高，但每次加载都有开销

选择哪种方式取决于具体的使用场景：
- 如果适配器是固定的且长期使用，建议使用 export-lora 合并
- 如果需要频繁切换适配器，建议使用运行时加载