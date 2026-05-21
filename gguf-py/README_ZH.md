# GGUF Python 库说明

GGUF (GGML Universal File) Python 库用于读写 GGUF 格式的二进制文件。

## 简介

GGUF 是 llama.cpp 使用的模型文件格式，本 Python 库提供了创建、修改和分析 GGUF 文件的功能。

## 安装

### 基础安装

```bash
pip install gguf
```

### 完整安装（包含可视化编辑器）

```bash
pip install gguf[gui]
```

## 目录结构

```
gguf-py/
├── gguf/
│   ├── __init__.py              # 模块入口
│   ├── gguf_reader.py           # GGUF 读取器
│   ├── gguf_writer.py           # GGUF 写入器
│   ├── vocab.py                 # 词汇表处理
│   └── scripts/                 # 实用脚本
├── examples/                    # 示例代码
└── tests/                       # 测试文件
```

## 主要功能

### 1. 读取 GGUF 文件

```python
import gguf

reader = gguf.GGUFReader("model.gguf")
print(reader.keys)
print(reader.tensors)
```

### 2. 写入 GGUF 文件

```python
import gguf

writer = gguf.GGUFWriter("output.gguf", arch="llama")
# 添加键值对
writer.add_key("general.architecture", "llama")
writer.add_key("general.file_type", 2)
# 添加张量
writer.add_tensor("token_embd.weight", tensor_data)
writer.write_header()
writer.write_tensors()
writer.close()
```

### 3. 修改 GGUF 元数据

```python
from gguf.scripts.gguf_set_metadata import set_metadata

set_metadata(
    input_path="model.gguf",
    output_path="model-updated.gguf",
    updates={
        "general.name": "Custom Model",
        "general.description": "My custom model"
    }
)
```

## 实用工具

### gguf_dump.py - 转储 GGUF 信息

查看 GGUF 文件的元数据和张量信息：

```bash
python -m gguf.scripts.gguf_dump model.gguf
```

**输出示例：**
```
General metadata:
  architecture: llama
  file_type: 4
  parameter_count: 6738415616

Tensors:
  token_embd.weight: shape=[128256, 4096], type=F16
  blk.0.attn_q.weight: shape=[4096, 4096], type=F16
  ...
```

### gguf_set_metadata.py - 修改元数据

修改 GGUF 文件的键值对：

```bash
python -m gguf.scripts.gguf_set_metadata \
  input.gguf \
  output.gguf \
  general.name="My Model" \
  general.description="Custom description"
```

### gguf_convert_endian.py - 字节序转换

转换 GGUF 文件的字节序：

```bash
python -m gguf.scripts.gguf_convert_endian \
  input.gguf \
  output.gguf
```

### gguf_new_metadata.py - 创建新版本

复制 GGUF 文件并添加/修改/删除元数据：

```bash
python -m gguf.scripts.gguf_new_metadata \
  input.gguf \
  output.gguf \
  --set general.name="New Name" \
  --delete deprecated.key
```

### gguf_editor_gui.py - 可视化编辑器

使用 Qt 图形界面编辑 GGUF 文件（需要安装 GUI 组件）：

```bash
python -m gguf.scripts.gguf_editor_gui model.gguf
```

**功能：**
- 查看和编辑元数据
- 查看张量信息
- 添加/删除键值对
- 图形化操作界面

## 示例代码

### examples/writer.py - 创建 GGUF 文件

演示如何生成一个 GGUF 文件：

```bash
python gguf-py/examples/writer.py
```

生成 `example.gguf` 文件（注意：这不是可用模型）。

### examples/reader.py - 读取 GGUF 文件

演示如何读取和显示 GGUF 文件内容：

```bash
python gguf-py/examples/reader.py model.gguf
```

输出键值对和张量详情。

## API 文档

### GGUFReader

读取 GGUF 文件的主要类：

```python
import gguf

reader = gguf.GGUFReader("model.gguf")

# 获取键值对
for key in reader.keys:
    value = reader.fields[key]
    print(f"{key}: {value}")

# 获取张量
for tensor_name in reader.tensors:
    tensor = reader.tensors[tensor_name]
    print(f"{tensor_name}: shape={tensor.shape}, type={tensor.tensor_type}")

# 获取特定值
arch = reader.fields.get("general.architecture")
```

### GGUFWriter

写入 GGUF 文件的主要类：

```python
import gguf
import numpy as np

writer = gguf.GGUFWriter("output.gguf", arch="llama")

# 添加键值对
writer.add_key("general.architecture", "llama")
writer.add_key("general.file_type", 4)
writer.add_key("general.name", "My Model")
writer.add_int("general.file_type", 4)
writer.add_float("general.frequency", 1.0)
writer.add_bool("general.is_quantized", True)
writer.add_string("general.description", "A model")

# 添加张量
data = np.random.rand(4096, 4096).astype(np.float16)
writer.add_tensor("weight", data, raw_shape=[4096, 4096])

# 写入文件
writer.write_header()
writer.write_tensors()
writer.close()
```

### 词汇表操作

```python
from gguf import GGUFWriter

# 添加词汇表
writer = gguf.GGUFWriter("output.gguf", arch="llama")
writer.add_tokenizer_model("llama")
writer.add_token_list(["hello", "world", "!"])
writer.add_merges(["h", "e", "l"])
writer.add_bos_token_id(1)
writer.add_eos_token_id(2)
```

## 量化类型

GGUF 支持多种量化类型：

| 类型 | 描述 | 比特/权重 |
|------|------|-----------|
| F32 | 32位浮点 | 32 |
| F16 | 16位浮点 | 16 |
| Q8_0 | 8位量化 | 8 |
| Q5_K_M | 5位量化，混合 | ~5 |
| Q4_K_M | 4位量化，混合 | ~4 |
| Q3_K | 3位量化 | ~3 |

## 开发指南

### 可编辑模式安装

开发者建议使用可编辑模式安装：

```bash
cd /path/to/llama.cpp/gguf-py

pip install --editable .
```

如果提示需要 setup.py：

```bash
pip install --upgrade pip
```

### 运行单元测试

```bash
python -m unittest discover ./gguf-py -v
```

## 自动发布

CI 自动发布流程：

1. 在 `pyproject.toml` 中更新版本号
2. 创建格式为 `gguf-vx.x.x` 的标签

```bash
git tag -a gguf-v1.0.0 -m "Version 1.0 release"
git push origin --tags
```

## 手动发布

如需手动发布：

```bash
# 安装工具
pip install build twine

# 更新版本
# 编辑 pyproject.toml

# 构建包
python -m build

# 上传到 PyPI
python -m twine upload dist/*
```

## 常见问题

### Q: GGUF 和 GGML 有什么区别？

A: GGUF 是 GGML 的升级版，提供更好的扩展性和性能。

### Q: 可以修改模型权重吗？

A: 可以，但通常不建议，因为这会影响模型性能。

### Q: 如何减小模型文件大小？

A: 使用量化工具 `llama-quantize`。

### Q: GGUF 编辑器需要什么依赖？

A: 需要安装 `PyQt5` 或 `PySide2`。

## 相关工具

- [llama-quantize](../tools/quantize/) - 量化工具
- [convert_hf_to_gguf.py](../convert_hf_to_gguf.py) - 模型转换
- [conversion/](../conversion/README_ZH.md) - 模型转换脚本

## 相关文档

- [GGUF 格式说明](https://github.com/ggml-org/ggml/pull/302)
- [模型文件说明](../models/README_ZH.md)
- [量化说明](../tools/quantize/README_ZH.md)

## 贡献

欢迎贡献新功能或修复：

1. Fork 仓库
2. 创建特性分支
3. 提交更改
4. 创建 Pull Request

## 参考资料

- [GGML 仓库](https://github.com/ggml-org/ggml)
- [llama.cpp 文档](https://github.com/ggml-org/llama.cpp)
- [Hugging Face](https://huggingface.co/)