# 许可证文件说明

本目录包含 llama.cpp 使用的第三方库的许可证文件。

## 简介

llama.cpp 在开发和运行过程中使用了多个第三方库和组件。每个库都有其自己的许可证，本目录包含这些许可证的副本，以确保项目的合规性和透明度。

## 许可证文件

| 文件 | 库 | 许可证 |
|------|---|--------|
| LICENSE-jsonhpp | nlohmann/json | MIT |

## 许可证类型

### MIT License

MIT 许可证是一种宽松的许可证，允许：

- ✅ 商业使用
- ✅ 修改
- ✅ 分发
- ✅ 私人使用

**条件：**
- 在副本中包含许可证和版权声明
- ✅ 不需要开源修改后的代码

## 其他依赖的许可证

除了本目录中的文件，llama.cpp 还使用了其他库，它们的许可证通常在：

1. **vendor/ 目录** - 第三方库源代码中的 LICENSE 文件
2. **源文件头部** - 直接在代码文件中声明
3. **项目根目录** - 主要依赖的许可证

## 主要第三方依赖

| 库 | 用途 | 许可证 |
|------|------|--------|
| nlohmann/json | JSON 处理 | MIT |
| stb | 图像处理 | MIT/Public Domain |
| utf8proc | Unicode 处理 | MIT |
| miniaudio | 音频处理 | MIT/Public Domain |
| cpp-httplib | HTTP 服务器 | MIT |

## vendor/ 目录

详细代码在 [vendor/](../vendor/README_ZH.md) 目录中，包括：

- **cpp-httplib/** - HTTP 服务器库
- **miniaudio/** - 音频处理库
- **nlohmann/** - JSON 库
- **sheredom/** - 各种实用工具
- **stb/** - 图像处理库

每个 vendor 目录都应包含其许可证文件。

## 许可证合规性

### 使用 llama.cpp 的要求

1. **遵守许可证条款** - 遵守所有第三方库的许可证要求
2. **保留版权声明** - 不要删除许可证和版权声明
3. **分发许可证** - 分发项目时包含所有许可证文件

### 在项目中使用

如果将 llama.cpp 集成到你的项目中：

1. 检查你的项目许可证与第三方库许可证兼容
2. 在你的项目中包含本目录中的许可证文件
3. 在文档中说明使用的第三方库

## 商业使用

llama.cpp 本身使用 MIT 许可证，可以商业使用。但需要注意：

- MIT 许可证允许商业使用
- 需要保留许可证声明
- 不需要开源你的代码

## 代码审查

所有第三方依赖都经过审查：

- ✅ 许可证兼容性
- ✅ 安全性
- ✅ 代码质量
- ✅ 维护状态

## 添加新依赖

添加新的第三方库时：

1. **检查许可证** - 确保许可证兼容
2. **添加许可证文件** - 将许可证复制到此目录
3. **更新文档** - 在 README 中说明新依赖
4. **更新依赖列表** - 在上述表格中添加条目

## 常见问题

### Q: 可以在商业项目中使用 llama.cpp 吗？

A: 可以，llama.cpp 使用 MIT 许可证，允许商业使用。

### Q: 需要开源我的代码吗？

A: 不需要，MIT 许可证允许闭源使用。

### Q: 如何知道某个库的许可证？

A: 查看该库的 README、LICENSE 文件或源代码头部。

### Q: 许可证文件在哪里？

A: 在本目录和 vendor/ 目录中。

## 相关文档

- [vendor/ README](../vendor/README_ZH.md) - 第三方库代码
- [主项目 README](../README.md) - 主项目说明
- [构建指南](../docs/build.md) - 构建和依赖

## 参考资料

- [开源许可证选择指南](https://choosealicense.com/)
- [MIT 许可证](https://opensource.org/licenses/MIT)
- [开源协议合规性](https://www.linuxfoundation.org/resources/open-source-guides/)

## 联系方式

如有许可证相关问题：

- GitHub Issues - 提问
- GitHub Discussions - 讨论
- 直接邮件 - 联系维护者