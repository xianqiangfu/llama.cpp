# GGUF 操作示例

此目录包含 GGUF (GPT-Generated Unified Format) 文件操作的示例代码。GGUF 是 llama.cpp 使用的二进制文件格式，用于存储大型语言模型的权重和元数据。

## 功能

此示例展示了如何：
- 创建新的 GGUF 文件
- 向 GGUF 文件写入各种类型的数据（整数、浮点数、字符串、数组等）
- 读取 GGUF 文件中的元数据
- 访问和检查张量信息
- 验证张量数据

## 使用方法

### 写入 GGUF 文件

```bash
./llama-gguf data.gguf w
```

这将创建一个包含示例数据和张量的 GGUF 文件。

### 读取 GGUF 文件

```bash
./llama-gguf data.gguf r
```

这将显示文件版本、对齐方式、数据偏移量、键值对数量、张量数量等信息，并读取张量数据。

### 读取并验证数据

```bash
./llama-gguf data.gguf r n
```

这与读取模式相同，但跳过数据验证（使用 `n` 参数）。

## 支持的数据类型

示例支持以下 GGUF 数据类型：
- 整数类型：`u8`、`i8`、`u16`、`i16`、`u32`、`i32`、`u64`、`i64`
- 浮点数类型：`f32`、`f64`
- 布尔类型：`bool`
- 字符串类型：`str`
- 数组类型：支持整数数组、浮点数数组、字符串数组

## 文件格式

GGUF 文件由以下部分组成：
1. 文件头：包含版本、对齐方式等信息
2. 键值存储 (KV)：包含模型的元数据和参数
3. 张量信息：包含每个张量的名称、大小、偏移量和数据类型
4. 张量数据：实际的权重数据

## 编译

确保已安装必要的依赖项，然后使用 CMake 编译：

```bash
cmake -B build
cmake --build build --target llama-gguf
```

## 注意事项

- 此示例主要用于开发和测试目的
- 生成的 GGUF 文件可用于测试 llama.cpp 的 GGUF 读取功能
- 对于生产环境，建议使用官方的 `llama-convert-hf-to-gguf` 工具来转换 HuggingFace 模型
