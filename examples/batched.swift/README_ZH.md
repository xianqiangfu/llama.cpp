# llama.cpp/example/batched.swift

这是 `examples/batched` 的 Swift 版本克隆。

## 快速开始

```bash
./llama-batched-swift MODEL_PATH [PROMPT] [PARALLEL]
```

## 参数说明

| 参数 | 说明 |
|------|------|
| `MODEL_PATH` | 模型文件路径（必需） |
| `PROMPT` | 输入提示词（可选） |
| `PARALLEL` | 并行生成的序列数量（可选） |

## 功能特点

- **Swift 实现**：使用 Swift 语言重新实现的批处理示例
- **相同功能**：与 C++ 版本功能相同
- **跨平台支持**：支持 macOS 和 iOS 平台

## 使用场景

- 在 Swift 项目中集成 llama.cpp
- iOS/macOS 应用开发
- Swift 生态系统的模型推理

## 编译要求

- Swift 5.5 或更高版本
- 支持 Swift 的开发环境（Xcode 或 Swift 命令行工具）

## 相关示例

- [batched](../batched/README_ZH.md) - C++ 版本的批处理示例

## 注意事项

- 需要先配置 Swift 开发环境
- 确保 llama.cpp 的 Swift 绑定已正确编译
- 可能需要额外的依赖库

## Swift 集成

此示例展示了如何在 Swift 代码中：
1. 加载 GGUF 模型
2. 执行批处理推理
3. 管理多个并行序列
4. 处理生成结果

适合想要在 Swift 项目中使用 llama.cpp 的开发者参考。