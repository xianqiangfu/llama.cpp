# 测试总览

本目录包含 llama.cpp 项目的所有测试代码和测试工具。

## 目录结构

```
tests/
├── CMakeLists.txt          # 测试构建配置
├── testing.h               # 测试公共头文件
├── peg-parser/             # PEG 解析器测试
└── test-*.cpp              # 各类测试文件
```

## 主要测试类别

### 1. 后端测试

- **test-backend-ops.cpp** - 测试各个硬件后端的操作实现
- **test-backend-sampler.cpp** - 测试采样器的后端实现

### 2. 聊天和模板测试

- **test-chat.cpp** - 聊天功能测试
- **test-chat-auto-parser.cpp** - 自动解析器测试
- **test-chat-peg-parser.cpp** - PEG 解析器测试
- **test-chat-template.cpp** - 聊天模板测试
- **test-jinja.cpp** - Jinja 模板引擎测试

### 3. 语法和解析测试

- **test-grammar-integration.cpp** - 语法集成测试
- **test-grammar-llguidance.cpp** - LLM-guidance 语法测试
- **test-grammar-parser.cpp** - 语法解析器测试
- **test-gbnf-validator.cpp** - GBNF 语法验证器测试

### 4. JSON 测试

- **test-json-partial.cpp** - 部分 JSON 解析测试
- **test-json-schema-to-grammar.cpp** - JSON Schema 到语法转换测试

### 5. Tokenizer 测试

- **test-tokenizer-0.cpp** - Tokenizer 基础测试
- **test-tokenizer-0.py** - Tokenizer Python 测试
- **test-tokenizer-1-bpe.cpp** - BPE tokenizer 测试
- **test-tokenizer-1-spm.cpp** - SentencePiece tokenizer 测试

### 6. GGUF 测试

- **test-gguf.cpp** - GGUF 文件格式测试
- **test-gguf-model-data.cpp** - GGUF 模型数据测试

### 7. 量化测试

- **test-quantize-fns.cpp** - 量化函数测试
- **test-quantize-perf.cpp** - 量化性能测试
- **test-quantize-stats.cpp** - 量化统计测试
- **test-quant-type-selection.cpp** - 量化类型选择测试

### 8. 架构测试

- **test-llama-archs.cpp** - LLaMA 架构测试

### 9. 采样测试

- **test-sampling.cpp** - 采样算法测试
- **test-reasoning-budget.cpp** - 推理预算测试

### 10. 其他测试

- **test-alloc.cpp** - 内存分配测试
- **test-arg-parser.cpp** - 参数解析器测试
- **test-autorelease.cpp** - 自动释放测试
- **test-barrier.cpp** - 屏障同步测试
- **test-double-float.cpp** - 双精度浮点测试
- **test-log.cpp** - 日志测试
- **test-opt.cpp** - 优化测试
- **test-rope.cpp** - RoPE (旋转位置编码) 测试
- **test-save-load-state.cpp** - 状态保存加载测试
- **test-state-restore-fragmented.cpp** - 状态恢复测试
- **test-thread-safety.cpp** - 线程安全测试

## PEG 解析器测试

PEG 解析器测试位于 `peg-parser/` 子目录，包含：

- **test-basic.cpp** - 基础解析测试
- **test-gbnf-generation.cpp** - GBNF 生成测试
- **test-json-parser.cpp** - JSON 解析器测试
- **test-json-serialization.cpp** - JSON 序列化测试
- **test-python-dict-parser.cpp** - Python 字典解析器测试
- **test-unicode.cpp** - Unicode 处理测试
- **simple-tokenize.cpp/h** - 简单分词工具

详细说明请参见 [PEG 解析器测试文档](./peg-parser/README_ZH.md)。

## 构建和运行测试

### 构建测试

使用 CMake 构建测试：

```bash
cmake -DLLAMA_BUILD_TESTS=ON ..
make
```

### 运行单个测试

```bash
./test-backend-ops
./test-chat-template
```

### 运行所有测试

```bash
ctest
```

## 测试覆盖范围

- 硬件后端操作的正确性和性能
- 各种模型的加载和推理
- 聊天功能和模板渲染
- 语法约束和输出验证
- 分词器的各种实现
- 量化和反量化
- 内存管理和线程安全
- 多模态输入处理

## 贡献指南

添加新测试时，请遵循以下准则：

1. 使用 `testing.h` 中提供的测试框架
2. 测试文件命名格式：`test-<feature>.cpp`
3. 确保测试独立运行，不依赖其他测试
4. 添加适当的注释说明测试目的
5. 在 CMakeLists.txt 中注册新测试