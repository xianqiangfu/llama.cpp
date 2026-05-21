# Python 依赖说明

本目录包含 llama.cpp 项目各个功能模块所需的 Python 依赖文件。

## 简介

llama.cpp 使用 Python 进行模型转换、测试、基准测试和辅助功能。本目录包含不同功能模块的依赖文件。

## 依赖文件

### 主依赖文件

| 文件 | 说明 |
|------|------|
| requirements-all.txt | 所有依赖的汇总文件 |

### 工具依赖

| 文件 | 说明 |
|------|------|
| requirements-compare-llama-bench.txt | llama-bench 比较工具依赖 |
| requirements-server-bench.txt | 服务器基准测试依赖 |
| requirements-tool_bench.txt | 工具基准测试依赖 |
| requirements-pydantic.txt | Pydantic 数据验证依赖 |
| requirements-test-tokenizer-random.txt | Tokenizer 随机测试依赖 |

### 模型转换依赖

| 文件 | 说明 |
|------|------|
| requirements-convert_hf_to_gguf.txt | Hugging Face 转 GGUF 依赖 |
| requirements-convert_hf_to_gguf_update.txt | 转换更新工具依赖 |
| requirements-convert_legacy_llama.txt | 旧版 Llama 转换依赖 |
| requirements-convert_llama_ggml_to_gguf.txt | GGML 转 GGUF 依赖 |

### GGUF 工具依赖

| 文件 | 说明 |
|------|------|
| requirements-gguf_editor_gui.txt | GGUF GUI 编辑器依赖 |

### 根目录依赖

这些依赖在项目根目录：

| 文件 | 说明 |
|------|------|
| requirements.txt | 基础依赖 |

## 安装依赖

### 安装所有依赖

```bash
pip install -r requirements/requirements-all.txt
```

### 安装特定功能依赖

```bash
# 模型转换
pip install -r requirements/requirements-convert_hf_to_gguf.txt

# 服务器基准测试
pip install -r requirements/requirements-server-bench.txt

# GGUF 编辑器
pip install -r requirements/requirements-gguf_editor_gui.txt
```

### 安装基础依赖

```bash
pip install -r requirements.txt
```

## 主要依赖包

### 模型转换

| 包 | 版本 | 用途 |
|------|------|------|
| torch | >= 2.0.0 | 模型加载和转换 |
| transformers | >= 4.30.0 | Hugging Face 模型 |
| numpy | >= 1.20.0 | 数值计算 |
| gguf | latest | GGUF 格式处理 |

### GGUF 工具

| 包 | 版本 | 用途 |
|------|------|------|
| gguf | latest | GGUF 文件处理 |
| PyQt5 | >= 5.15.0 | GUI 编辑器 |
| or PySide2 | >= 5.15.0 | GUI 编辑器 |

### 测试和基准

| 包 | 版本 | 用途 |
|------|------|------|
| pytest | >= 7.0.0 | 测试框架 |
| pytest-benchmark | >= 4.0.0 | 基准测试 |
| matplotlib | >= 3.5.0 | 数据可视化 |

### 数据验证

| 包 | 版本 | 用途 |
|------|------|------|
| pydantic | >= 2.0.0 | 数据验证 |
| pydantic-settings | >= 2.0.0 | 配置验证 |

## 虚拟环境

### 创建虚拟环境

```bash
# 使用 venv
python -m venv venv
source venv/bin/activate  # Linux/Mac
# 或
venv\Scripts\activate  # Windows

# 使用 conda
conda create -n llama-cpp python=3.10
conda activate llama-cpp
```

### 安装依赖

```bash
pip install --upgrade pip
pip install -r requirements/requirements-all.txt
```

### 保存依赖

```bash
pip freeze > requirements.txt
```

## 版本要求

### Python 版本

- **推荐**: Python 3.10 - 3.12
- **最低**: Python 3.8
- **测试**: Python 3.11

### 平台支持

| 平台 | 支持 | 备注 |
|------|------|------|
| Linux | ✅ | 完全支持 |
| macOS | ✅ | 完全支持 |
| Windows | ✅ | 完全支持 |

## 故障排查

### 依赖冲突

```bash
# 使用 pipdeptree 检查依赖关系
pip install pipdeptree
pipdeptree
```

### 版本不匹配

```bash
# 强制重新安装
pip install --force-reinstall <package>
```

### 安装失败

```bash
# 尝试使用国内镜像
pip install -r requirements.txt -i https://pypi.tuna.tsinghua.edu.cn/simple
```

## 常见问题

### Q: 需要安装所有依赖吗？

A: 不需要，根据需要的功能安装对应依赖。

### Q: torch 安装困难怎么办？

A: 使用 conda 安装：`conda install pytorch`

### Q: 可以使用虚拟环境吗？

A: 推荐，避免依赖冲突。

### Q: 如何查看已安装的包？

A: `pip list` 或 `pip freeze`

### Q: 依赖文件会更新吗？

A: 会的，定期检查更新。

## 性能优化

### 使用编译包

某些包提供预编译版本：

```bash
# PyTorch 预编译版本
pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu118
```

### 减少依赖

只安装必要的依赖：

```bash
pip install torch transformers gguf
```

## 安全建议

### 1. 使用最新版本

```bash
pip install --upgrade <package>
```

### 2. 检查安全漏洞

```bash
pip install safety
safety check
```

### 3. 使用虚拟环境

隔离项目依赖，避免全局污染。

## 相关文档

- [模型转换](../conversion/README_ZH.md)
- [gguf-py](../gguf-py/README_ZH.md)
- [构建指南](../docs/build.md)

## 参考资料

- [PyPI](https://pypi.org/)
- [PyTorch 文档](https://pytorch.org/)
- [Transformers 文档](https://huggingface.co/docs/transformers)
- [虚拟环境指南](https://docs.python.org/3/library/venv.html)

## 贡献

更新依赖时：

1. 测试新版本兼容性
2. 更新版本要求
3. 更新文档
4. 提交 Pull Request

## 联系方式

如有依赖相关问题：

- GitHub Issues - 报告问题
- GitHub Discussions - 讨论依赖
- Pull Request - 提交更新