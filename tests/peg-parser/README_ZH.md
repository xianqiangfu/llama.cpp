# PEG 解析器测试说明

本目录包含 PEG (Parsing Expression Grammar) 解析器的测试代码。

## 简介

PEG 解析器是 llama.cpp 中用于解析和验证模型输出的重要组件，特别是用于支持语法约束（GBNF）功能。

## 目录结构

```
tests/peg-parser/
├── test-basic.cpp              # 基础解析测试
├── test-gbnf-generation.cpp    # GBNF 生成测试
├── test-json-parser.cpp        # JSON 解析器测试
├── test-json-serialization.cpp # JSON 序列化测试
├── test-python-dict-parser.cpp # Python 字典解析器测试
├── test-unicode.cpp            # Unicode 处理测试
├── simple-tokenize.cpp         # 简单分词实现
├── simple-tokenize.h           # 简单分词头文件
└── tests.h                     # 测试公共定义
```

## 测试文件说明

### test-basic.cpp

测试 PEG 解析器的基本功能，包括：

- 基本语法规则的解析
- 字符匹配和范围匹配
- 序列和选择
- 可选和重复操作符

### test-gbnf-generation.cpp

测试 GBNF (GGML BNF) 语法的生成功能：

- 从抽象语法树生成 GBNF 语法
- 语法规则的正确性验证
- 复杂语法的生成测试

### test-json-parser.cpp

专门测试 JSON 解析功能：

- JSON 对象解析
- JSON 数组解析
- 嵌套 JSON 结构处理
- JSON 数据类型支持

### test-json-serialization.cpp

测试 JSON 序列化功能：

- 将解析结果序列化为 JSON
- 数据格式正确性验证
- 序列化性能测试

### test-python-dict-parser.cpp

测试 Python 字典风格的解析：

- Python 字典语法支持
- 键值对解析
- 嵌套字典处理

### test-unicode.cpp

全面测试 Unicode 支持：

- 多语言字符处理
- Unicode 转义序列
- 字符范围匹配
- 编码转换

### simple-tokenize.cpp/h

提供简单的分词实现，用于：

- 字符级别分词
- 辅助解析器测试
- 基础分词功能演示

## 编译和运行

### 编译

```bash
cmake -DLLAMA_BUILD_TESTS=ON ..
make
```

### 运行单个测试

```bash
./tests/peg-parser/test-basic
./tests/peg-parser/test-gbnf-generation
./tests/peg-parser/test-json-parser
```

### 运行所有 PEG 解析器测试

```bash
ctest -R peg-parser
```

## 测试要点

### 基础功能

- ✅ 字符字面量匹配
- ✅ 字符范围（如 `[a-z]`）
- ✅ 字符类（如 `[abc]`）
- ✅ 字符取反（如 `[^abc]`）

### 组合操作

- ✅ 序列：`A B`
- ✅ 选择：`A | B`
- ✅ 可选：`A?`
- ✅ 零次或多次重复：`A*`
- ✅ 一次或多次重复：`A+`

### 语法规则

- ✅ 非终结符定义
- ✅ 递归规则
- ✅ 规则引用

### 特殊功能

- ✅ 注释支持（`#` 开头）
- ✅ Unicode 字符处理
- ✅ 转义序列
- ✅ Token 匹配

## 常见问题

### Q: 如何添加新的测试用例？

A: 在相应的测试文件中添加测试函数，遵循现有测试的模式。

### Q: 测试失败时如何调试？

A: 使用调试器（如 GDB）或添加日志输出，查看解析过程的详细信息。

### Q: PEG 解析器与 GBNF 的关系？

A: PEG 解析器是底层实现，GBNF 是基于 PEG 的特定语法格式，用于约束模型输出。

## 相关文档

- [GBNF 语法指南](../../grammars/README_ZH.md)
- [测试总览](../README_ZH.md)
- [主项目 README](../../README.md)

## 贡献

欢迎贡献新的测试用例或改进现有测试。请确保：

1. 测试覆盖新的功能点
2. 测试用例清晰明了
3. 添加适当的注释说明