# 第三方库说明

本目录包含 llama.cpp 使用的第三方库的源代码。

## 简介

llama.cpp 使用多个第三方库来提供各种功能，如 HTTP 服务器、音频处理、图像处理、JSON 处理等。这些库直接嵌入到项目中以简化依赖管理。

## 库列表

| 目录 | 库名 | 许可证 | 用途 |
|------|------|--------|------|
| cpp-httplib | cpp-httplib | MIT | HTTP 服务器 |
| miniaudio | miniaudio | MIT/Public Domain | 音频处理 |
| nlohmann | nlohmann/json | MIT | JSON 处理 |
| sheredom | subprocess.h | MIT | 子进程 |
| stb | stb | MIT/Public Domain | 图像处理 |

## 库详情

### cpp-httplib

**目录**: `vendor/cpp-httplib/`

**文件**:
- `httplib.h` - HTTP 库头文件
- `httplib.cpp` - HTTP 库实现
- `CMakeLists.txt` - 构建配置
- `LICENSE` - MIT 许可证

**用途**: 提供 HTTP 服务器功能，用于 `llama-server`

**特点**:
- 跨平台支持
- 支持异步请求
- 内置 WebSocket 支持

**许可证**: [MIT](./cpp-httplib/LICENSE)

### miniaudio

**目录**: `vendor/miniaudio/`

**文件**:
- `miniaudio.h` - 音频库（单文件）

**用途**: 提供音频处理功能，用于 TTS 相关功能

**特点**:
- 单文件头文件库
- 跨平台支持
- 无外部依赖

**许可证**: MIT/Public Domain

### nlohmann

**目录**: `vendor/nlohmann/`

**文件**:
- `json.hpp` - JSON 库（主文件）
- `json_fwd.hpp` - JSON 前向声明

**用途**: 提供 JSON 处理功能

**特点**:
- 现代 C++ 接口
- 易于使用
- 高性能

**许可证**: MIT

**详细信息**: [licenses/LICENSE-jsonhpp](../licenses/LICENSE-jsonhpp)

### sheredom

**目录**: `vendor/sheredom/`

**文件**:
- `subprocess.h` - 子进程库（单文件）

**用途**: 提供子进程创建和管理功能

**特点**:
- 单文件头文件库
- 跨平台支持
- 简洁的 API

**许可证**: MIT

### stb

**目录**: `vendor/stb/`

**文件**:
- `stb_image.h` - 图像加载库

**用途**: 提供图像加载和处理功能

**特点**:
- 单文件头文件库
- 支持多种图像格式
- 无外部依赖

**许可证**: MIT/Public Domain

## 许可证合规

所有第三方库都使用宽松的许可证（MIT 或 Public Domain），允许：

- ✅ 商业使用
- ✅ 修改
- ✅ 分发
- ✅ 私人使用

**要求**:
- 保留许可证和版权声明
- 不需要开源修改后的代码

## 使用这些库

### HTTP 服务器 (cpp-httplib)

```cpp
#include "vendor/cpp-httplib/httplib.h"

httplib::Server svr;
svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
    res.set_content("Hello World!", "text/plain");
});
svr.listen("0.0.0.0", 8080);
```

### JSON 处理 (nlohmann)

```cpp
#include "vendor/nlohmann/json.hpp"
using json = nlohmann::json;

json j = {
    {"name", "llama.cpp"},
    {"version", "1.0.0"}
};
std::string s = j.dump();
```

### 音频处理 (miniaudio)

```c
#define MINIAUDIO_IMPLEMENTATION
#include "vendor/miniaudio/miniaudio.h"

// 音频处理代码...
```

### 图像处理 (stb)

```c
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb/stb_image.h"

int width, height, channels;
unsigned char *img = stbi_load("image.png", &width, &height, &channels, 0);
```

### 子进程 (sheredom)

```cpp
#include "vendor/sheredom/subprocess.h"

// 子进程代码...
```

## 更新库

### 更新步骤

1. **备份当前版本**
   ```bash
   cp -r vendor/cpp-httplib vendor/cpp-httplib.bak
   ```

2. **获取新版本**
   ```bash
   git clone https://github.com/yhirose/cpp-httplib.git temp
   cp temp/httplib.h vendor/cpp-httplib/
   cp temp/httplib.cpp vendor/cpp-httplib/
   ```

3. **更新许可证**
   ```bash
   cp temp/LICENSE vendor/cpp-httplib/LICENSE
   ```

4. **更新文档**
   - 更新本 README
   - 更新 licenses/ 目录

5. **测试**
   - 编译项目
   - 运行测试
   - 验证功能

## 添加新库

### 添加步骤

1. **检查许可证** - 确保许可证兼容
2. **下载库** - 获取库文件
3. **复制到 vendor/** - 组织文件
4. **添加许可证** - 复制到 licenses/
5. **更新文档** - 更新本文档
6. **更新构建** - 修改 CMakeLists.txt
7. **测试** - 验证集成

### 推荐的库类型

- 单文件头文件库（如 stb）
- MIT/BSD 许可证
- 无外部依赖
- 活跃维护

## 安全考虑

### 1. 定期更新

定期检查库的安全更新。

### 2. 审计代码

审查关键库的代码。

### 3. 限制暴露

不要暴露不必要的功能。

### 4. 使用最新版本

确保使用最新稳定版本。

## 性能考虑

### 优化建议

1. **链接时优化 (LTO)**
   ```cmake
   set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
   ```

2. **编译选项**
   ```cmake
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
   ```

3. **静态链接**
   ```cmake
   target_link_libraries(mytarget PRIVATE static_lib)
   ```

## 故障排查

### 编译错误

```bash
# 检查 C++ 标准
cmake -DCMAKE_CXX_STANDARD=17 ..

# 清理重新构建
make clean && cmake .. && make
```

### 链接错误

```bash
# 检查库路径
ldd ./llama-cli

# 静态链接
cmake -DSTATIC_LIB=ON ..
```

## 相关文档

- [许可证文件](../licenses/README_ZH.md)
- [构建指南](../docs/build.md)
- [开发文档](../docs/development/README_ZH.md)

## 参考资料

- [cpp-httplib](https://github.com/yhirose/cpp-httplib)
- [miniaudio](https://github.com/mackron/miniaudio)
- [nlohmann/json](https://github.com/nlohmann/json)
- [stb](https://github.com/nothings/stb)
- [sheredom/subprocess.h](https://github.com/sheredom/subprocess.h)

## 贡献

更新或添加库时：

1. 确保许可证兼容
2. 更新所有文档
3. 测试功能
4. 提交 Pull Request

## 常见问题

### Q: 为什么不使用系统包管理器？

A: 嵌入库简化了依赖管理和跨平台支持。

### Q: 可以替换这些库吗？

A: 可以，修改 CMakeLists.txt 使用系统库。

### Q: 库有多旧？

A: 定期更新，但优先考虑稳定性。

### Q: 如何查看许可证？

A: 每个库目录和 `licenses/` 目录都有许可证文件。

### Q: 可以删除不需要的库吗？

A: 可以，修改构建配置排除不用的功能。