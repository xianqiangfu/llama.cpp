# llama.cpp/examples/llama.android

llama.cpp 在 Android 平台上的应用示例。这是一个 Android 项目，演示了如何在 Android 设备上使用 llama.cpp 进行本地推理。

### 项目结构

- `build.gradle.kts`: 项目构建配置文件
- `settings.gradle.kts`: 项目设置文件
- `gradle.properties`: Gradle 属性配置
- `gradlew`: Gradle 包装脚本

### 构建步骤

1. 确保已安装 Android Studio 和必要的 Android SDK
2. 使用 Gradle 构建项目：
   ```bash
   ./gradlew assembleDebug
   ```

3. 将构建的 APK 安装到设备：
   ```bash
   ./gradlew installDebug
   ```

### 使用说明

详细的使用说明和开发指南请参考 llama.cpp 主项目的 Android 集成文档。此示例展示了：
- 模型加载和初始化
- 文本生成推理
- UI 交互
- 模型下载和管理

### 注意事项

- 需要较新的 Android 设备以获得良好的性能
- 模型文件较大，确保设备有足够的存储空间
- 首次运行需要下载 GGUF 格式的模型文件
