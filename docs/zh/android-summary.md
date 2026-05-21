# Android 平台文档摘要 (android.md)

## 概述

llama.cpp 为 Android 平台提供了多种部署方式，包括 GUI 应用、Termux 命令行环境和交叉编译。支持硬件加速，包括 Arm CPU 的 SME2 和 x86-64 CPU 的 AMX 指令集。项目自动检测主机硬件功能并加载兼容的内核，能够在最新高端设备和旧设备上无缝运行。

## Android 端编译配置

### 方式 1：使用 Android Studio 构建 GUI 应用

#### 导入项目

1. 在 Android Studio 中导入 `examples/llama.android` 目录
2. 执行 Gradle 同步
3. 构建项目

![项目导入 Android Studio](./android/imported-into-android-studio.jpg)

#### 硬件加速支持

Android 绑定支持多种硬件加速：
- **Arm CPU**：最高支持 SME2 指令集
- **x86-64 CPU**：支持 AMX 指令集
- **自动检测**：自动检测主机硬件功能
- **兼容性**：在最新设备和旧设备上均可运行

#### 应用功能

最小化 Android 应用前端展示了绑定的核心功能：

1. **解析 GGUF 元数据**
   - 使用 `GgufMetadataReader` 类
   - 支持共享存储中的 `ContentResolver` 提供的 `Uri`
   - 支持应用私有存储中的本地 `File`

2. **获取推理引擎**
   - 通过 `AiChat` 门面获取 `InferenceEngine` 实例
   - 通过应用私有文件路径加载选定模型

3. **发送原始用户提示**
   - 自动模板格式化
   - 预填充处理
   - 批量解码
   - 通过 Kotlin `Flow` 收集生成的 token

#### 生产级应用参考

对于生产就绪的体验，推荐使用 [Arm AI Chat](https://play.google.com/store/apps/details?id=com.arm.aichat)（Google Play）。该项目由 Arm 的 **CT-ML**、**CE-ML** 和 **STE** 团队协作开发，提供以下高级功能：
- 系统提示支持
- 性能基准测试
- 模型管理
- Arm 功能可视化器
- 友好的用户界面

| 主界面 | 系统提示 | 性能指标 |
|:------:|:-------:|:--------:|
| ![Home screen](https://naco-siren.github.io/ai-chat/policy/index/1-llm-starter-pack.png) | ![System prompt](https://naco-siren.github.io/ai-chat/policy/index/5-system-prompt.png) | ![Haiku](https://naco-siren.github.io/ai-chat/policy/index/4-metrics.png) |

### 方式 2：使用 Termux 构建 CLI 环境

#### 环境准备

[Termux](https://termux.dev/en/) 是 Android 终端模拟器和 Linux 环境应用（无需 root）。获取方式：
- Google Play Store（实验性）
- 项目官方仓库
- F-Droid

#### 安装步骤

```bash
# 更新包管理器
$ apt update && apt upgrade -y

# 安装构建工具
$ apt install git cmake

# 下载模型（推荐放在 ~/ 目录以获得最佳性能）
$ curl -L {model-url} -o ~/{model}.gguf
```

#### 编译和运行

```bash
# 按照 CMake 构建说明进行编译
# 参考：https://github.com/ggml-org/llama.cpp/blob/master/docs/build.md

# 运行推理
$ cd llama.cpp
$ ./build/bin/llama-cli -m ~/{model}.gguf -c {context-size} -p "{your-prompt}"
```

**重要提示**：
- 理论上 `examples` 目录下的任何可执行文件都可以工作
- 设置合理的 `context-size`（例如 4096）以避免内存峰值导致终端崩溃
- 将模型放在 `~/` 目录可获得最佳性能

### 方式 3：交叉编译 CLI 使用 Android NDK

#### 环境准备

确保已准备好交叉编译 Android 程序的环境（安装 Android SDK）。**注意**：与桌面环境不同，Android 环境只提供有限的原生库集合。

#### CMake 配置

```bash
$ cmake \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-28 \
  -DCMAKE_C_FLAGS="-march=armv8.7a" \
  -DCMAKE_CXX_FLAGS="-march=armv8.7a" \
  -DGGML_OPENMP=OFF \
  -DGGML_LLAMAFILE=OFF \
  -B build-android
```

#### 配置说明

- **工具链**：使用 Android NDK 的 CMake 工具链文件
- **架构**：arm64-v8a（64 位 ARM）
- **平台版本**：android-28（Android 9）
- **CPU 特性**：armv8.7a（现代设备最高性能选项）
- **OpenMP**：禁用（Android NDK 的 OpenMP 需要作为依赖项安装，目前不支持）
- **llamafile**：禁用（不支持 Android 设备）

**注意**：即使设备不支持 `armv8.7a`，llama.cpp 也会在运行时检查可用的 CPU 特性。

#### 编译和安装

```bash
# 编译
$ cmake --build build-android --config Release -j{n}

# 安装
$ cmake --install build-android --prefix {install-dir} --config Release
```

#### 部署到设备

```bash
# 创建目录
$ adb shell "mkdir /data/local/tmp/llama.cpp"

# 复制可执行文件
$ adb push {install-dir} /data/local/tmp/llama.cpp/

# 复制模型文件
$ adb push {model}.gguf /data/local/tmp/llama.cpp/

# 进入设备 shell
$ adb shell
```

#### 在设备上运行

```bash
$ cd /data/local/tmp/llama.cpp
$ LD_LIBRARY_PATH=lib ./bin/llama-simple -m {model}.gguf -c {context-size} -p "{your-prompt}"
```

**重要说明**：
- Android 无法自动找到 `lib` 库路径，必须指定 `LD_LIBRARY_PATH`
- 后续 API 版本支持 RPATH，未来可能改变
- 参考 Android 配置信息设置合理的 `context-size`

## Android 端部署方法

### 方案对比

| 方案 | 优点 | 缺点 | 适用场景 |
|------|------|------|---------|
| Android Studio GUI | 完整应用体验 | 需要开发环境 | 生产应用 |
| Termux CLI | 简单快速 | 性能较低 | 开发测试 |
| NDK 交叉编译 | 性能最优 | 配置复杂 | 生产部署 |

### 部署最佳实践

#### 1. 性能优化

- **模型放置**：将模型放在 `~/` 目录（Termux）或应用私有存储
- **上下文大小**：根据设备内存设置合理的上下文大小
- **CPU 特性**：使用支持最高 CPU 指令集的编译选项

#### 2. 内存管理

- **监控内存**：避免设置过大的上下文窗口
- **量化选择**：使用量化模型减少内存占用
- **资源释放**：及时释放不再使用的资源

#### 3. 兼容性考虑

- **最低版本**：Android 9 (API 28) 是合理的最低支持版本
- **设备差异**：考虑不同设备的 CPU 能力
- **自动检测**：利用 llama.cpp 的硬件检测功能

### 常见问题解决

#### 1. 内存不足

**症状**：应用崩溃或终端被杀死

**解决方案**：
- 减小上下文大小 `-c 4096` 或更小
- 使用量化模型（Q4_K_M、Q3_K_M）
- 关闭其他应用释放内存

#### 2. 性能不佳

**症状**：推理速度慢

**解决方案**：
- 使用支持硬件加速的编译选项
- 选择适当的模型大小
- 优化模型量化级别

#### 3. 兼容性问题

**症状**：在某些设备上无法运行

**解决方案**：
- 检查 Android 版本（最低 API 28）
- 验证 CPU 架构兼容性
- 使用运行时硬件检测

## 开发指南

### GUI 应用开发

1. **项目结构**：参考 `examples/llama.android`
2. **核心类**：
   - `GgufMetadataReader`：模型元数据解析
   - `InferenceEngine`：推理引擎接口
   - `AiChat`：应用门面类
3. **数据流**：
   - 用户输入 → 模板格式化 → 预填充 → 批量解码 → token 收集

### CLI 应用开发

1. **Termux 环境**：使用标准的 llama.cpp CLI 工具
2. **跨平台**：遵循标准的 CMake 构建流程
3. **部署**：使用 ADB 推送文件到设备

### 性能调优

1. **CPU 利用率**：启用多线程处理
2. **内存效率**：使用适当的量化级别
3. **批量处理**：利用批量解码提高效率

## 硬件特性支持

### Arm CPU 支持

- **SME2**：最新的 Arm 扩展指令集
- **NEON**：传统 SIMD 指令集
- **自动检测**：运行时检测可用特性

### x86-64 CPU 支持

- **AMX**：高级矩阵扩展指令集
- **AVX/AVX2**：传统 SIMD 指令集
- **兼容性**：支持多种处理器

### 自动优化

llama.cpp 自动检测硬件特性：
- 无需手动配置
- 兼容新旧设备
- 选择最优实现

## 安全考虑

### 应用权限

- 存储访问：请求适当的存储权限
- 网络访问：如需在线加载模型
- 后台服务：合理使用后台资源

### 数据隐私

- 模型存储：使用应用私有存储
- 用户数据：遵守隐私政策
- 网络传输：使用安全连接

## 总结

llama.cpp 为 Android 平台提供了完整的支持，包括 GUI 应用、Termux CLI 环境和 NDK 交叉编译。开发者可以根据需求选择合适的部署方式，并通过硬件加速和自动优化获得最佳性能。建议在生产环境中使用 Android Studio 构建的 GUI 应用，在开发测试中使用 Termux，在需要极致性能时使用 NDK 交叉编译。