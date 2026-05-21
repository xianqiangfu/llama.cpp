# llama.cpp/example/tts
此示例演示了文本转语音功能。它使用来自 [outeai](https://www.outeai.com/) 的[模型](https://www.outeai.com/blog/outetts-0.2-500m)。

## 快速开始

如果您已经构建了具有 SSL 支持的 llama.cpp，只需运行以下命令，所需的模型将自动下载：
```console
$ build/bin/llama-tts --tts-oute-default -p "Hello world" && aplay output.wav
```
有关模型和如何将它们转换为所需格式的详细信息，请参阅以下章节。

### 模型转换

检出或下载包含 LLM 模型的模型：
```console
$ pushd models
$ git clone --branch main --single-branch --depth 1 https://huggingface.co/OuteAI/OuteTTS-0.2-500M
$ cd OuteTTS-0.2-500M && git lfs install && git lfs pull
$ popd
```
将模型转换为 .gguf 格式：
```console
(venv) python convert_hf_to_gguf.py models/OuteTTS-0.2-500M \
    --outfile models/outetts-0.2-0.5B-f16.gguf --outtype f16
```
生成的模型将是 `models/outetts-0.2-0.5B-f16.gguf`。

我们可以使用以下命令将其量化为 Q8_0：
```console
$ build/bin/llama-quantize models/outetts-0.2-0.5B-f16.gguf \
    models/outetts-0.2-0.5B-q8_0.gguf q8_0
```
量化模型将是 `models/outetts-0.2-0.5B-q8_0.gguf`。

接下来，我们对音频解码器执行类似操作。首先检出或下载语音解码器的模型：
```console
$ pushd models
$ git clone --branch main --single-branch --depth 1 https://huggingface.co/novateur/WavTokenizer-large-speech-75token
$ cd WavTokenizer-large-speech-75token && git lfs install && git lfs pull
$ popd
```
此模型文件是 PyTorch 检查点（.ckpt），我们首先需要将其转换为 huggingface 格式：
```console
(venv) python tools/tts/convert_pt_to_hf.py \
    models/WavTokenizer-large-speech-75token/wavtokenizer_large_speech_320_24k.ckpt
...
Model has been successfully converted and saved to models/WavTokenizer-large-speech-75token/model.safetensors
Metadata has been saved to models/WavTokenizer-large-speech-75token/index.json
Config has been saved to models/WavTokenizer-large-speech-75tokenconfig.json
```
然后我们可以将 huggingface 格式转换为 gguf：
```console
(venv) python convert_hf_to_gguf.py models/WavTokenizer-large-speech-75token \
    --outfile models/wavtokenizer-large-75-f16.gguf --outtype f16
...
INFO:hf-to-gguf:Model successfully exported to models/wavtokenizer-large-75-f16.gguf
```

### 运行示例

生成了两个模型后，LLM 模型和语音解码器模型，我们可以运行示例：
```console
$ build/bin/llama-tts -m  ./models/outetts-0.2-0.5B-q8_0.gguf \
    -mv ./models/wavtokenizer-large-75-f16.gguf \
    -p "Hello world"
...
main: audio written to file 'output.wav'
```
output.wav 文件将包含提示词的音频。这可以通过使用媒体播放器播放文件来听到。在 Linux 上，以下命令将播放音频：
```console
$ aplay output.wav
```

### 使用 llama-server 运行示例

也可以使用 `llama-server` 运行此示例，需要启动两个服务器实例。一个将服务于 LLM 模型，另一个将服务于语音解码器模型。

LLM 模型服务器可以使用以下命令启动：
```console
$ ./build/bin/llama-server -m ./models/outetts-0.2-0.5B-q8_0.gguf --port 8020
```

语音解码器模型服务器可以使用以下命令启动：
```console
./build/bin/llama-server -m ./models/wavtokenizer-large-75-f16.gguf --port 8021 --embeddings --pooling none
```

然后我们可以运行 [tts-outetts.py](tts-outetts.py) 来生成音频。

首先为 python 创建虚拟环境并安装所需的依赖项（这只需要做一次）：
```console
$ python3 -m venv venv
$ source venv/bin/activate
(venv) pip install requests numpy
```

然后使用以下命令运行 python 脚本：
```console
(venv) python ./tools/tts/tts-outetts.py http://localhost:8020 http://localhost:8021 "Hello world"
spectrogram generated: n_codes: 90, n_embd: 1282
converting to audio ...
audio generated: 28800 samples
audio written to file "output.wav"
```
要播放音频，我们可以再次使用 aplay 或任何其他媒体播放器：
```console
$ aplay output.wav
```

## 命令行选项

### 常用选项

| 选项 | 说明 |
|------|------|
| `-m, --model PATH` | LLM 模型文件路径 |
| `-mv, --model-vocoder PATH` | 语音解码器模型文件路径 |
| `-p, --prompt TEXT` | 要转换为语音的文本 |
| `-o, --output PATH` | 输出音频文件路径（默认：output.wav） |
| `-s, --seed SEED` | 随机种子（默认：-1） |
| `--tts-oute-default` | 使用默认 OuteTTS 配置 |

### 高级选项

| 选项 | 说明 |
|------|------|
| `-t, --threads N` | 使用的线程数 |
| `-ngl, --n-gpu-layers N` | 卸载到 GPU 的层数 |
| `-c, --ctx-size N` | 上下文大小 |
| `--temperature N` | 采样温度 |
| `--top-p N` | Top-p 采样参数 |
| `--top-k N` | Top-k 采样参数 |
| `-h, --help` | 显示帮助信息 |

## 支持的语言和模型

### OuteTTS

OuteTTS 是一个高性能的文本转语音模型，支持：

- 英语
- 多种其他语言（取决于训练数据）

模型特性：
- 高质量音频输出
- 快速推理速度
- 低内存占用

### WavTokenizer

WavTokenizer 是一个音频编解码器，用于将音频转换为离散令牌和反向转换。

## 性能优化

### GPU 加速

```bash
# 使用 GPU 加速
./llama-tts -m model.gguf -mv vocoder.gguf -p "Hello" -ngl 99
```

### 量化

```bash
# 使用量化模型
./llama-tts -m model-q4_k_m.gguf -mv vocoder-q8_0.gguf -p "Hello"
```

### 批处理

```bash
# 处理多个文本（需要自定义脚本）
for text in "Hello" "World" "Test"; do
    ./llama-tts -p "$text" -o "$text.wav"
done
```

## Python 脚本使用

### tts-outetts.py

用于通过 llama-server 运行 TTS 的 Python 脚本。

#### 安装依赖

```bash
pip install requests numpy
```

#### 使用方法

```bash
python tools/tts/tts-outetts.py <llm-server-url> <vocoder-server-url> "<text>"
```

#### 示例

```bash
python tools/tts/tts-outetts.py \
    http://localhost:8020 \
    http://localhost:8021 \
    "This is a test of the text to speech system"
```

## 故障排除

### 常见问题

1. **SSL 证书错误**
   - 确保系统有正确的 SSL 证书
   - 更新 CA 证书包

2. **模型加载失败**
   - 检查模型文件路径是否正确
   - 确保模型文件没有损坏

3. **音频质量问题**
   - 尝试不同的量化级别
   - 调整采样参数
   - 使用更高质量的模型

4. **内存不足**
   - 使用量化模型
   - 减少上下文大小
   - 释放不需要的 GPU 内存

### 调试技巧

```bash
# 启用详细输出
./llama-tts -m model.gguf -mv vocoder.gguf -p "Hello" --verbose

# 使用较小的模型进行测试
./llama-tts -m small-model.gguf -mv small-vocoder.gguf -p "Hello"
```

## 相关资源

- [OuteTTS 模型](https://www.outeai.com/blog/outetts-0.2-500m)
- [WavTokenizer](https://huggingface.co/novateur/WavTokenizer-large-speech-75token)
- [llama.cpp 文档](../../README.md)
- [模型转换脚本](../../convert_hf_to_gguf.py)

## 注意事项

1. **版权和许可**：确保遵守所用模型的许可条款
2. **性能考虑**：TTS 需要大量计算资源，特别是在没有 GPU 加速的情况下
3. **音频质量**：不同的模型和参数设置会产生不同的音频质量
4. **语言支持**：确保模型支持您需要的语言