# 辅助脚本说明

本目录包含 llama.cpp 项目的各种辅助脚本，用于构建、测试、基准测试和开发。

## 目录结构

```
scripts/
├── apple/              # Apple 平台相关脚本
├── hip/                # AMD HIP 相关脚本
├── jinja/              # Jinja 模板相关脚本
├── snapdragon/         # Snapdragon 相关脚本
├── *.sh                # Bash 脚本
├── *.py                # Python 脚本
└── *.cmake             # CMake 脚本
```

## 脚本分类

### 构建相关

| 脚本 | 说明 |
|------|------|
| build-info.sh | 生成构建信息 |
| get-flags.mk | 获取编译标志 |

### 测试相关

| 脚本 | 说明 |
|------|------|
| debug-test.sh | 调试测试 |
| get-hellaswag.sh | 获取 HellaSwag 测试集 |
| get-wikitext-2.sh | 获取 WikiText-2 数据集 |
| get-winogrande.sh | 获取 WinoGrande 数据集 |

### 基准测试

| 脚本 | 说明 |
|------|------|
| bench-models.sh | 模型基准测试 |
| compare-llama-bench.py | 对比 llama-bench 结果 |
| server-bench.py | 服务器基准测试 |
| tool_bench.py / tool_bench.sh | 工具基准测试 |

### 模型获取

| 脚本 | 说明 |
|------|------|
| fetch_server_test_models.py | 获取服务器测试模型 |
| get-pg.sh | 获取 PG 数据集 |
| get_chat_template.py | 获取聊天模板 |

### 开发工具

| 脚本 | 说明 |
|------|------|
| git-bisect.sh | Git 二分查找 |
| gen-authors.sh | 生成作者列表 |
| compare-commits.sh | 比较提交 |
| sync-ggml.sh | 同步 GGML |
| sync-ggml-am.sh | 同步 GGML AMD |

### 数据处理

| 脚本 | 说明 |
|------|------|
| compare-logprobs.py | 对比日志概率 |
| gen-unicode-data.py | 生成 Unicode 数据 |
| verify-checksum-models.py | 验证模型校验和 |

### 功能测试

| 脚本 | 说明 |
|------|------|
| server-test-function-call.py | 服务器函数调用测试 |
| server-test-model.py | 服务器模型测试 |
| server-test-parallel-tc.py | 服务器并行测试 |
| server-test-structured.py | 服务器结构化输出测试 |

### 系统工具

| 脚本 | 说明 |
|------|------|
| check-requirements.sh | 检查依赖要求 |
| create_ops_docs.py | 创建操作文档 |
| hf.sh | Hugging Face 工具 |
| serve-static.js | 静态服务器 |
| xxd.cmake | xxd 工具 |

## 主要脚本详解

### bench-models.sh

运行模型基准测试：

```bash
./scripts/bench-models.sh
```

**输出：**
- 各模型的推理速度
- 内存使用情况
- 不同量化等级的性能

### compare-llama-bench.py

对比不同版本的基准测试结果：

```bash
./scripts/compare-llama-bench.py \
  --baseline old_results.csv \
  --current new_results.csv
```

### server-bench.py

服务器性能基准测试：

```bash
./scripts/server-bench.py \
  --model model.gguf \
  --port 8080 \
  --concurrent 10
```

### fetch_server_test_models.py

获取服务器测试所需的模型：

```bash
./scripts/fetch_server_test_models.py
```

**下载模型到：**
- `models/` 目录
- 按模型组织

### get_chat_template.py

从 Hugging Face 获取聊天模板：

```bash
./scripts/get_chat_template.py \
  meta-llama/Meta-Llama-3-8B-Instruct \
  > models/templates/meta-llama-Llama-3-8B-Instruct.jinja
```

### check-requirements.sh

检查系统依赖是否满足要求：

```bash
./scripts/check-requirements.sh
```

**检查项：**
- 编译器版本
- CMake 版本
- 依赖库
- GPU 驱动

### sync-ggml.sh

同步 GGML 子模块：

```bash
./scripts/sync-ggml.sh
```

**功能：**
- 拉取最新 GGML 代码
- 更新子模块引用

## 平台特定脚本

### Apple

位于 `scripts/apple/`：

- macOS 特定构建脚本
- Metal 后端配置

### HIP

位于 `scripts/hip/`：

- AMD GPU 支持
- HIP 编译设置

### Snapdragon

位于 `scripts/snapdragon/`：

- 移动设备优化
- Snapdragon 处理器支持

### Jinja

位于 `scripts/jinja/`：

- Jinja 模板工具
- 聊天模板处理

## 使用示例

### 1. 设置开发环境

```bash
# 检查依赖
./scripts/check-requirements.sh

# 同步 GGML
./scripts/sync-ggml.sh

# 构建项目
cmake -DLLAMA_BUILD_TESTS=ON ..
make
```

### 2. 运行基准测试

```bash
# 获取测试数据
./scripts/get-wikitext-2.sh

# 运行基准测试
./scripts/bench-models.sh

# 对比结果
./scripts/compare-llama-bench.py --baseline v1.csv --current v2.csv
```

### 3. 测试服务器

```bash
# 获取测试模型
./scripts/fetch_server_test_models.py

# 启动服务器
./llama-server -m model.gguf

# 运行服务器测试
./scripts/server-bench.py
```

### 4. 获取聊天模板

```bash
# 获取模板
./scripts/get_chat_template.py meta-llama/Meta-Llama-3-8B-Instruct

# 使用模板
./llama-cli -m model.gguf \
  --chat-template models/templates/meta-llama-Llama-3-8B-Instruct.jinja \
  -p "Hello"
```

## 脚本开发

### 添加新脚本

1. **添加 Shebang**
   ```bash
   #!/bin/bash
   ```

2. **添加文档**
   ```bash
   #!/bin/bash
   # Description: Script description
   # Usage: ./script.sh [options]
   ```

3. **添加错误处理**
   ```bash
   set -e  # 遇到错误退出
   ```

4. **更新文档**
   - 更新本 README
   - 添加使用示例

### 脚本规范

**Bash 脚本：**
- 使用 `set -e`
- 添加文档字符串
- 支持参数
- 错误处理

**Python 脚本：**
- 添加类型提示
- 使用 logging
- 异常处理
- 文档字符串

## 故障排查

### 脚本执行失败

```bash
# 检查权限
chmod +x script.sh

# 检查依赖
./scripts/check-requirements.sh

# 调试模式
bash -x script.sh
```

### 网络问题

```bash
# 使用代理
export http_proxy=http://proxy:port
export https_proxy=http://proxy:port

# 重试下载
./scripts/fetch_server_test_models.py
```

### 权限问题

```bash
# 修复权限
chmod -R 755 scripts/
```

## 性能优化

### 并行下载

```bash
# 并行下载模型
./scripts/fetch_server_test_models.py --parallel 4
```

### 缓存使用

```bash
# 使用缓存
./scripts/bench-models.sh --cache ./cache
```

## 常见问题

### Q: 脚本在哪里？

A: 所有脚本在 `scripts/` 目录中。

### Q: 如何运行脚本？

A: `./scripts/script.sh` 或 `python3 scripts/script.py`

### Q: 需要权限吗？

A: 某些脚本可能需要 `chmod +x`

### Q: 脚本失败怎么办？

A: 检查依赖、权限和错误日志。

### Q: 可以贡献脚本吗？

A: 欢迎，遵循代码规范。

## 相关文档

- [构建指南](../docs/build.md)
- [测试总览](../tests/README_ZH.md)
- [运维文档](../docs/ops/README_ZH.md)

## 参考资料

- [Bash 脚本指南](https://www.gnu.org/software/bash/manual/)
- [Python 脚本指南](https://docs.python.org/3/)
- [CMake 文档](https://cmake.org/documentation/)

## 贡献

欢迎贡献新脚本：

1. 遵循现有风格
2. 添加文档
3. 测试脚本
4. 更新本文档