# Android 文档说明

本目录包含 llama.cpp 在 Android 平台上的集成和使用文档。

## 目录内容

```
docs/android/
└── imported-into-android-studio.jpg    # Android Studio 集成截图
```

## 概述

llama.cpp 可以集成到 Android 应用中，实现设备上的本地 LLM 推理。这种部署方式具有以下优势：

- 数据隐私：所有推理在本地完成，无需将数据发送到云端
- 低延迟：无需网络请求，响应速度快
- 离线工作：无网络时仍可使用
- 成本效益：无需支付云端推理费用

## 快速开始

### 方式一：使用示例项目

参考 [examples/llama.android](../../examples/llama.android/) 目录中的示例项目：

```bash
cd examples/llama.android
```

该示例项目演示了：

- C++ 核心库的集成
- JNI (Java Native Interface) 接口
- Kotlin/Java 调用示例
- 基础的聊天界面

### 方式二：手动集成到现有项目

#### 1. 导入 native 库

将 llama.cpp 的 C++ 源码集成到 Android 项目的 `app/src/main/cpp/` 目录：

```cmake
# app/src/main/cpp/CMakeLists.txt
cmake_minimum_required(VERSION 3.18.1)

# 设置 llama.cpp 源码路径
set(LLAMACPP_DIR ${CMAKE_SOURCE_DIR}/../../../../..)

# 添加 llama.cpp 子模块
add_subdirectory(${LLAMACPP_DIR} llama_cpp)

# 创建原生库
add_library(native-lib SHARED native-lib.cpp)

# 链接 llama.cpp
target_link_libraries(native-lib llama)
```

#### 2. 创建 JNI 接口

在 `native-lib.cpp` 中创建 Java 可调用的接口：

```cpp
#include <jni.h>
#include "llama.h"

extern "C" JNIEXPORT jlong JNICALL
Java_com_example_llama_LlamaWrapper_loadModel(
    JNIEnv *env,
    jobject /* this */,
    jstring model_path) {
    const char *path = env->GetStringUTFChars(model_path, nullptr);
    llama_model_params params = llama_model_default_params();
    llama_model *model = llama_load_model_from_file(path, params);
    env->ReleaseStringUTFChars(model_path, path);
    return reinterpret_cast<jlong>(model);
}
```

#### 3. 在 Kotlin/Java 中调用

```kotlin
class LlamaWrapper {
    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }

    external fun loadModel(modelPath: String): Long
}
```

## 主要组件

### 1. Native 库

- 使用 CMake 构建 llama.cpp 核心库
- 支持 ARM64 架构
- 可选的 CPU 优化（NEON）

### 2. JNI 接口

- 提供 Java/Kotlin 与 C++ 的桥接
- 支持模型加载、推理、采样等操作
- 处理字符串和数组的转换

### 3. Android 集成

- Kotlin/Java 应用层
- 用户界面
- 模型文件管理

## 构建配置

### Gradle 配置

在 `app/build.gradle` 中添加：

```gradle
android {
    defaultConfig {
        ndk {
            abiFilters 'arm64-v8a'
        }
    }

    externalNativeBuild {
        cmake {
            cppFlags "-std=c++17"
        }
    }
}

dependencies {
    // 可选的依赖
    implementation("androidx.core:core-ktx:1.12.0")
}
```

### CMake 选项

```cmake
# 启用特定功能
set(LLAMA_CUBLAS OFF)    # Android 不需要 CUDA
set(LLAMA_METAL OFF)     # Android 不需要 Metal
set(LLAMA_VULKAN ON)     # 可选：Vulkan 支持
```

## 性能优化

### 1. 量化模型

使用量化模型减少内存占用：

```bash
# 在桌面端量化模型
./llama-quantize original_model.gguf model-q4_0.gguf q4_0
```

推荐的量化等级：
- Q4_0: 平衡性能和质量
- Q4_K_M: 更好的质量
- Q3_K: 更小的体积

### 2. 内存管理

- 使用 `n_ctx` 参数限制上下文长度
- 及时释放不用的模型资源
- 考虑使用 `mmap` 加载模型

### 3. 线程配置

根据设备核心数设置线程数：

```cpp
llama_context_params params = llama_context_default_params();
params.n_threads = 4;  // 根据设备调整
```

## 常见问题

### Q: 支持 Android x86 架构吗？

A: 主要支持 ARM64，如需 x86 支持需要额外配置和测试。

### Q: 如何减小 APK 体积？

A:
- 使用量化模型
- 只包含必要的架构
- 分离模型文件，通过下载获取

### Q: Android 版本要求？

A: 最低 Android 7.0 (API 24)，推荐 Android 10+。

### Q: 如何处理大模型文件？

A:
- 使用应用内下载
- 存储在外部存储
- 考虑增量加载

## 参考资源

- [Android 示例项目](../../examples/llama.android/)
- [Android Studio 集成截图](./imported-into-android-studio.jpg)
- [JNI 编程指南](https://developer.android.com/training/articles/perf-jni)
- [CMake 构建文档](../build.md)

## 相关文档

- [文档目录总览](../README_ZH.md)
- [后端文档](../backend/README_ZH.md)
- [开发文档](../development/README_ZH.md)