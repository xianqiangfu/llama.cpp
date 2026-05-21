# llama.cpp/examples/llama.swiftui

在 iPhone 上进行 llama.cpp 本地推理。这是一个示例应用，可作为更高级项目的起点。

有关使用说明和性能统计信息，请查看以下讨论：https://github.com/ggml-org/llama.cpp/discussions/4508

### 构建步骤

首先需要构建 llama.cpp 并创建 XCFramework。可以通过在 llama.cpp 项目根目录下运行以下脚本来完成：

```console
$ ./build-xcframework.sh
```

在 Xcode 中打开 `llama.swiftui.xcodeproj` 项目，您应该能够在模拟器或真实设备上构建和运行应用。

要将框架用于不同的项目，可以通过将 `build-apple/llama.xcframework` 拖放到项目导航器中，或在项目设置的"Frameworks, Libraries, and Embedded Content"部分手动选择框架来将 XCFramework 添加到项目中。

![image](https://github.com/ggml-org/llama.cpp/assets/1991296/2b40284f-8421-47a2-b634-74eece09a299)

视频演示：

https://github.com/bachittle/llama.cpp/assets/39804642/e290827a-4edb-4093-9642-2a5e399ec545
