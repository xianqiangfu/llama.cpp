# 概念验证代码说明

本目录包含 llama.cpp 的概念验证（Proof of Concept, PoC）代码。

## 简介

PoC 代码用于验证新想法、测试新技术或展示特定功能。这些代码可能不完全成熟，但提供了实现思路和参考。

## 目录结构

```
pocs/
├── CMakeLists.txt  # 构建配置
└── vdot/          # vdot 相关 PoC
    ├── q8dot.cpp  # Q8 点积实现
    └── vdot.cpp   # vdot 实现
```

## vdot

vdot（Vector Dot Product）是向量点积的优化实现。

### q8dot.cpp

Q8 量化点积的 PoC 实现。

**目的：**
- 验证 Q8 量化点积算法
- 优化点积计算性能
- 测试量化对精度的影响

**用法：**
```bash
cd pocs/vdot
make
./q8dot
```

### vdot.cpp

通用向量点积的 PoC 实现。

**目的：**
- 测试点积算法
- 比较不同实现
- 优化计算性能

**用法：**
```bash
cd pocs/vdot
make
./vdot
```

## 构建 PoC 代码

### 单独构建

```bash
cd pocs
cmake ..
make
```

### 集成构建

在主项目构建时包含：

```bash
cmake -DLLAMA_BUILD_POCS=ON ..
make
```

## 使用 PoC 代码

### vdot 点积测试

```bash
cd pocs/vdot
./vdot
```

输出示例：
```
Testing vector dot product...
Input vectors: [1.0, 2.0, 3.0], [4.0, 5.0, 6.0]
Result: 32.0
```

### Q8 量化测试

```bash
cd pocs/vdot
./q8dot
```

输出示例：
```
Testing Q8 dot product...
Quantization complete
Dot product result: 32
```

## 开发指南

### 添加新的 PoC

1. 在 `pocs/` 目录下创建新文件夹
2. 添加 CMakeLists.txt 配置
3. 实现你的代码
4. 添加文档说明

#### 示例结构

```
pocs/
└── my_poc/
    ├── CMakeLists.txt
    ├── my_poc.cpp
    └── README.md
```

#### CMakeLists.txt

```cmake
add_executable(my_poc my_poc.cpp)

if (LLAMA_BUILD_TESTS)
    target_link_libraries(my_poc PRIVATE ggml llama)
endif()
```

## PoC 代码特点

### 1. 简洁性

PoC 代码保持简单，专注于核心概念。

### 2. 独立性

PoC 代码尽可能独立，不依赖复杂系统。

### 3. 可测试性

PoC 代码易于测试和验证。

### 4. 文档化

包含清晰的注释和使用说明。

## 贡献指南

### 提交 PoC

1. **命名规范** - 使用描述性名称
2. **文档** - 添加 README 说明
3. **测试** - 包含测试用例
4. **注释** - 代码需要清晰注释

### PoC 到生产

如果 PoC 成功验证，可以：

1. 重构代码
2. 添加完整测试
3. 集成到主代码库
4. 更新相关文档

## 当前 PoC 状态

| PoC | 状态 | 说明 |
|------|------|------|
| vdot | 实验 | 向量点积优化 |
| q8dot | 实验 | Q8 量化点积 |

## 未来可能的 PoC

- 新量化算法
- 注意力机制优化
- 内存管理改进
- 新后端支持
- 多模态处理

## 常见问题

### Q: PoC 代码稳定吗？

A: PoC 代码可能不稳定，仅供实验使用。

### Q: 可以在生产环境使用 PoC 吗？

A: 不建议，PoC 代码需要经过充分测试。

### Q: 如何从 PoC 到正式功能？

A: 经过充分测试、代码审查和文档完善后集成。

### Q: PoC 代码会删除吗？

A: 验证成功后可能集成到主代码库，失败后可能被移除。

## 相关资源

- [开发文档](../docs/development/README_ZH.md)
- [测试总览](../tests/README_ZH.md)
- [GGML 文档](https://github.com/ggml-org/ggml)

## 参考资料

- [概念验证](https://en.wikipedia.org/wiki/Proof_of_concept)
- [向量运算](https://en.wikipedia.org/wiki/Vector_(mathematics_and_physics))
- [量化技术](https://en.wikipedia.org/wiki/Quantization_(signal_processing))

## 联系方式

如有 PoC 相关问题：

- GitHub Issues - 报告问题
- GitHub Discussions - 讨论 PoC
- Pull Request - 提交新 PoC