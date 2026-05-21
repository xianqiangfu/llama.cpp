# Issue #22: 各后端文档摘要

## 文档来源
- 文档路径: `F:\work2025\llama.cpp\docs\backend/`
- 包含文档: BLIS.md, CANN.md, CUDA-FEDORA.md, OPENCL.md, OPENVINO.md, SYCL.md, VirtGPU.md, ZenDNN.md, zDNN.md

## 各后端配置整理

### 1. BLIS后端 (BLIS.md)

#### 背景介绍
- **BLIS**: 高性能BLAS-like密集线性代数库软件框架
- **特点**: 获得多个奖项，提供基于对象的API、类型化API、BLAS和CBLAS兼容层
- **项目URL**: https://github.com/flame/blis

#### 编译配置
```bash
# 编译BLIS
git clone https://github.com/flame/blis
cd blis
./configure --enable-cblas -t openmp,pthreads auto
make -j
sudo make install
```

#### llama.cpp编译
```bash
mkdir build
cd build
cmake -DGGML_BLAS=ON -DGGML_BLAS_VENDOR=FLAME ..
make -j
```

#### 运行配置
```bash
# 设置OpenMP线程亲和性
export GOMP_CPU_AFFINITY="0-19"
export BLIS_NUM_THREADS=14
```

#### 性能优化要点
- 推荐使用OpenMP以获得更好的线程控制
- 可根据CPU核心数调整`BLIS_NUM_THREADS`
- Intel平台可能遇到`libimf.so`缺失问题，需要安装相关依赖

---

### 2. CANN后端 (CANN.md)

#### 背景介绍
- **昇腾NPU**: 使用神经网络处理单元的AI处理器系列
- **CANN**: 面向AI场景的异构计算架构
- **支持硬件**: Atlas 300I A2 (310P)、Atlas 300I Duo (310P)、Atlas A2 (910B)等

#### 操作系统支持
- **Linux**: Ubuntu 22.04, OpenEuler 22.03

#### 模型支持
##### 纯文本模型
支持Llama-2/3、Mistral-7B、Falcon、Qwen、Phi等100+种模型
数据类型: FP16, Q4_0, Q8_0

##### 多模态模型
支持BakLLaVA、Mini CPM、Moondream、Qwen2-VL等
数据类型: FP16, Q4_0, Q8_0

#### 数据类型支持
| 数据类型 | 910B  | 310P   |
|---------|--------|---------|
| FP16    | 支持   | 支持   |
| Q8_0    | 支持   | 部分   |
| Q4_0    | 支持   | 部分   |
| BF16    | 支持   |         |

#### 环境配置
```bash
# 1. 配置昇腾用户和组
sudo groupadd HwHiAiUser
sudo useradd -g HwHiAiUser -d /home/HwHiAiUser -m HwHiAiUser -s /bin/bash
sudo usermod -aG HwHiAiUser $USER

# 2. 安装CANN驱动和工具包
wget https://ascend-repo.obs.cn-east-2.myhuaweicloud.com/CANN/CANN%208.5.T63/Ascend-cann_8.5.0_linux-$ARCH.run
sudo bash ./Ascend-cann_8.5.0_linux-$ARCH.run --install

# 3. 验证安装
npu-smi info
source /usr/local/Ascend/cann/set_env.sh
```

#### 编译llama.cpp
```bash
cmake -B build -DGGML_CANN=on -DCMAKE_BUILD_TYPE=release
cmake --build build --config release
```

#### 运行配置
- **单设备**: `--split-mode none --main-gpu DEVICE_ID`
- **多设备**: `--split-mode layer` (默认)

#### 环境变量
- `GGML_CANN_MEM_POOL`: 内存池管理策略 (vmm/prio/leg)
- `GGML_CANN_DISABLE_BUF_POOL_CLEAN`: 控制内存池自动清理
- `GGML_CANN_WEIGHT_NZ`: 启用权重NZ格式转换
- `GGML_CANN_ACL_GRAPH`: 启用ACL图执行
- `GGML_CANN_OPERATOR_FUSION`: 启用算子融合

---

### 3. CUDA后端 (CUDA-FEDORA.md)

#### 背景介绍
- **适用场景**: Fedora Workstation、Atomic Desktops、Spins等
- **安装方式**: 通过toolbox容器环境安装

#### 环境配置
```bash
# 1. 创建toolbox容器
toolbox create --image registry.fedoraproject.org/fedora-toolbox:41 --container fedora-toolbox-41-cuda
toolbox enter --container fedora-toolbox-41-cuda

# 2. 安装开发工具
sudo dnf install @c-development @development-tools cmake

# 3. 添加CUDA仓库
sudo dnf config-manager addrepo --from-repofile=https://developer.download.nvidia.com/compute/cuda/repos/fedora41/x86_64/cuda-fedora41.repo

# 4. 安装NVIDIA驱动库
sudo dnf install nvidia-driver-cuda nvidia-driver-libs nvidia-driver-cuda-libs nvidia-persistenced

# 5. 安装CUDA元包
sudo dnf install cuda

# 6. 配置环境
sudo sh -c 'echo "export PATH=\$PATH:/usr/local/cuda/bin" >> /etc/profile.d/cuda.sh'
source /etc/profile.d/cuda.sh
```

#### 验证安装
```bash
nvcc --version
```

#### 编译llama.cpp
参考[构建文档](../build.md)中的CUDA支持说明

---

### 4. OpenCL后端 (OPENCL.md)

#### 背景介绍
- **OpenCL**: 跨平台并行编程标准
- **主要目标**: Qualcomm Adreno GPU
- **兼容性**: 支持Intel GPU等，但性能不是最优

#### 操作系统支持
- **Android**: Snapdragon 8 Gen 3, Snapdragon 8 Elite
- **Windows**: Windows 11 Arm64 with Snapdragon X Elite
- **Linux**: Ubuntu 22.04 WSL2 with Intel 12700H

#### 硬件支持
- **Adreno 750** (Snapdragon 8 Gen 3)
- **Adreno 830** (Snapdragon 8 Elite)
- **Adreno X85** (Snapdragon X Elite)

#### 数据类型支持
- **Q4_0**: 支持，已优化
- **Q6_K**: 支持，但未优化
- **Q8_0**: 支持
- **MXFP4**: 支持（MoE模型）

#### CMake选项
- `GGML_OPENCL_EMBED_KERNELS`: 嵌入OpenCL内核到可执行文件
- `GGML_OPENCL_USE_ADRENO_KERNELS`: 使用Adreno优化内核

#### Android编译
```bash
# 安装NDK
wget https://dl.google.com/android/repository/commandlinetools-linux-8512546_latest.zip
unzip commandlinetools-linux-8512546_latest.zip
~/android-sdk/cmdline-tools/latest/bin/sdkmanager "ndk;26.3.11579264"

# 安装OpenCL头文件和库
git clone https://github.com/KhronosGroup/OpenCL-Headers
cp -r CL ~/android-sdk/ndk/26.3.11579264/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include

git clone https://github.com/KhronosGroup/OpenCL-ICD-Loader
cd OpenCL-ICD-Loader && mkdir build_ndk26 && cd build_ndk26
cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE=$HOME/android-sdk/ndk/26.3.11579264/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=24
ninja
cp libOpenCL.so ~/android-sdk/ndk/26.3.11579264/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android

# 编译llama.cpp
cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE=$HOME/android-sdk/ndk/26.3.11579264/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-28 -DBUILD_SHARED_LIBS=OFF -DGGML_OPENCL=ON
ninja
```

#### 模型量化建议
```bash
# 使用--pure参数进行纯Q4_0量化
./llama-quantize --pure ggml-model-qwen2.5-3b-f16.gguf ggml-model-qwen-3b-Q4_0.gguf Q4_0
```

#### 已知问题
- Flash attention并不总是提升性能
- A6xx GPU在手机上由于旧驱动和编译器可能不支持

---

### 5. OpenVINO后端 (OPENVINO.md)

#### 背景介绍
- **OpenVINO**: 英特尔AI推理优化工具包
- **支持硬件**: 英特尔CPU、GPU（集成和独立）、NPU
- **特点**: 硬件加速推理，兼容GGUF模型生态系统

#### 支持设备
- **Intel CPUs**: 全面支持
- **Intel GPUs**: 集成和独立GPU
- **Intel NPUs**: 英特尔Core Ultra系列

#### 模型精度支持
- `FP16`: 全面支持
- `BF16`: 支持Intel Xeon
- `Q8_0`, `Q4_0`, `Q4_1`: 支持
- `Q4_K`, `Q4_K_M`: 支持
- `Q5_K`, `Q6_K`: 运行时转换为Q8_0_C

#### 已验证模型
- Llama-3.2-1B-Instruct
- Meta-Llama-3.1-8B-Instruct
- Phi-3-mini-4k-instruct
- Qwen2.5-1.5B-Instruct
- Qwen3-8B-Instruct
- DeepSeek-R1-Distill-Llama-8B
等20+种模型

#### 编译配置
##### Linux
```bash
source /opt/intel/openvino/setupvars.sh
cmake -B build/ReleaseOV -G Ninja -DCMAKE_BUILD_TYPE=Release -DGGML_OPENVINO=ON
cmake --build build/ReleaseOV --parallel
```

##### Windows
```cmd
"C:\Program Files (x86)\Intel\openvino_2026.0\setupvars.bat"
cmake -B build\ReleaseOV -G Ninja -DCMAKE_BUILD_TYPE=Release -DGGML_OPENVINO=ON -DLLAMA_CURL=OFF
cmake --build build\ReleaseOV --parallel
```

#### 运行配置
```bash
# GPU设备
export GGML_OPENVINO_DEVICE=GPU
export GGML_OPENVINO_STATEFUL_EXECUTION=1

# NPU设备
export GGML_OPENVINO_DEVICE=NPU

# 运行推理
./build/ReleaseOV/bin/llama-cli -m ~/models/model.gguf -c 1024
```

#### 环境变量
| 变量名 | 默认值 | 描述 |
|--------|--------|------|
| GGML_OPENVINO_DEVICE | CPU | 目标设备 (CPU/GPU/NPU) |
| GGML_OPENVINO_CACHE_DIR | 未设置 | OpenVINO模型缓存目录 |
| GGML_OPENVINO_STATEFUL_EXECUTION | 0 | 启用有状态KV缓存 |
| GGML_OPENVINO_PROFILING | 0 | 启用性能分析 |
| GGML_OPENVINO_PREFILL_CHUNK_SIZE | 256 | NPU预填充块大小 |

#### 已知问题
- GPU无状态执行存在已知问题，需启用`GGML_OPENVINO_STATEFUL_EXECUTION=1`
- NPU在大上下文时可能失败，建议设置较小的上下文大小
- 编码器模型（embedding、reranking）暂不支持
- `llama-server`在启用状态执行时只支持单个会话

#### Docker支持
```bash
# 构建镜像
docker build -t llama-openvino:light --target=light -f .devops/openvino.Dockerfile .

# 运行容器
docker run --rm -it -v ~/models:/models --device=/dev/dri --group-add=$(stat -c "%g" /dev/dri/render* | head -n 1) -u $(id -u):$(id -g) --env=GGML_OPENVINO_DEVICE=GPU --env=GGML_OPENVINO_STATEFUL_EXECUTION=1 llama-openvino:light --no-warmup -c 1024 -m /models/model.gguf
```

---

### 6. SYCL后端 (SYCL.md)

#### 背景介绍
- **SYCL**: 高级并行编程模型，基于C++17
- **oneAPI**: 开放生态系统，支持多架构
- **主要目标**: Intel GPU

#### 推荐版本
##### Windows
- b5377: Arc B580/Linux/oneAPI 2025.1
- b4040: Arc A770/Linux/oneAPI 2024.1
- b3038: Arc A770/Linux/oneAPI 2024.1

##### Ubuntu 24.04
提供预编译的Ubuntu 24.04 x64包

#### 操作系统支持
- **Linux**: Ubuntu 22.04, Fedora Silverblue 39, Arch Linux
- **Windows**: Windows 11

#### 硬件支持
- **Intel Data Center Max Series**: Max 1550, 1100
- **Intel Data Center Flex Series**: Flex 170
- **Intel Arc A-Series**: Arc A770, Arc A730M, Arc A750
- **Intel Arc B-Series**: Arc B580
- **Intel built-in Arc GPU**: Meteor Lake, Arrow Lake, Lunar Lake
- **Intel iGPU**: 11代及更新Core CPU

#### 性能优化
- MUL_MAT Q4_0在Intel GPU上性能提升21%-87%
- 支持Flash-Attention，减少内存使用
- Fused MoE优化

#### 编译配置
##### Linux
```bash
source /opt/intel/oneapi/setvars.sh

# FP32 (推荐)
cmake -B build -DGGML_SYCL=ON -DCMAKE_C_COMPILER=icx -DCMAKE_CXX_COMPILER=icpx

# FP16
cmake -B build -DGGML_SYCL=ON -DCMAKE_C_COMPILER=icx -DCMAKE_CXX_COMPILER=icpx -DGGML_SYCL_F16=ON

cmake --build build --config Release -j -v
```

##### Windows
```powershell
# 下载预编译包或从源码编译
# 方法1: 从源码编译
cmake -B build -G "Ninja" -DGGML_SYCL=ON -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=icx -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j

# 方法2: 使用CMake预设
cmake --preset x64-windows-sycl-release
cmake --build build-x64-windows-sycl-release -j
```

#### 设备选择
- **单设备**: `--split-mode none --main-gpu DEVICE_ID`
- **多设备**: `--split-mode layer` (默认)

#### 环境变量
| 变量名 | 默认值 | 描述 |
|--------|--------|------|
| GGML_SYCL_F16 | OFF | 启用FP16构建 |
| GGML_SYCL_ENABLE_FLASH_ATTN | 1 | 启用Flash-Attention |
| GGML_SYCL_DISABLE_OPT | 0 | 禁用Intel GPU优化 |
| GGML_SYCL_ENABLE_LEVEL_ZERO | 1 | 使用Level Zero API |
| GGML_SYCL_DISABLE_DNN | 0 | 禁用oneDNN |
| ZES_ENABLE_SYSMAN | 0 | 支持获取GPU空闲内存 |

#### Docker支持
```bash
# FP32构建
docker build -t llama-cpp-sycl --build-arg="GGML_SYCL_F16=OFF" --target light -f .devops/intel.Dockerfile .

# FP16构建
docker build -t llama-cpp-sycl --build-arg="GGML_SYCL_F16=ON" --target light -f .devops/intel.Dockerfile .

# 运行容器
docker run -it --rm -v "/path/to/models:/models" --device /dev/dri/renderD128:/dev/dri/renderD128 --device /dev/dri/card0:/dev/dri/card0 llama-cpp-sycl -m /models/model.gguf -p "prompt" -n 400 -e -ngl 33
```

---

### 7. VirtGPU后端 (VirtGPU.md)

#### 背景介绍
- **VirtGPU**: 虚拟GPU后端，使GGML应用在虚拟机中运行
- **特点**: 使用主机-客户机共享内存进行高效数据传输
- **组成**: 前端（客户机）和后端（主机）两个库

#### 操作系统支持
| OS | 状态 | 后端 | CI测试 | 备注 |
|----|------|------|--------|------|
| MacOS 14 | 支持 | ggml-metal | X | 在MacOS 14上编译可工作 |
| MacOS 15 | 支持 | ggml-metal | X | 在MacOS 14或15上编译可工作 |
| Linux | 开发中 | ggml-vulkan | 测试中 | 本地工作，CI遇到死锁 |

#### 架构组件
1. **客户机前端**: 实现GGML后端接口，将操作转发到主机
2. **主机后端**: 接收转发的操作，在实际硬件后端上执行
3. **通信层**: 使用virtio-gpu超调用和共享内存

#### 通信协议
##### 超调用和共享内存
- **DRM_IOCTL_VIRTGPU_EXECBUFFER**: 从客户机到主机触发远程执行
- **共享内存页**: 张量和参数的零拷贝数据传输

##### 共享内存布局
- **数据缓冲区**: 24 MiB，用于命令/响应数据和张量传输
- **回复缓冲区**: 16 KiB，用于命令回复和状态信息
- **数据缓冲区**: 动态分配的主机-客户机共享缓冲区

#### 支持操作
- **设备操作**: 设备枚举、能力查询、内存信息、后端类型检测
- **缓冲区操作**: 缓冲区分配/释放、张量数据传输、内存拷贝/清除
- **计算操作**: 图执行转发

#### 构建要求
##### 客户机依赖
- `libdrm`用于DRM/virtio-gpu通信
- C++20兼容编译器
- CMake 3.14+

##### 主机依赖
- 支持APIR的virglrenderer
- 目标后端库（libggml-metal, libggml-vulkan等）

#### 环境变量
- `GGML_VIRTGPU_BACKEND_LIBRARY`: 主机端后端库路径
- `GGML_VIRTGPU_DEBUG`: 启用调试日志

#### 构建选项
- `GGML_VIRTGPU`: 启用VirtGPU后端
- `GGML_VIRTGPU_BACKEND`: 构建主机端后端组件

#### 限制
- **VM专用**: 只在支持virtio-gpu的虚拟机中工作
- **主机依赖**: 需要正确配置的主机端后端
- **延迟**: 每个操作从VM逃逸的少量开销
- **共享内存大小**: 在libkrun虚拟机管理器下，可寻址内存限制为64GB

---

### 8. ZenDNN后端 (ZenDNN.md)

#### 背景介绍
- **ZenDNN**: AMD EPYC CPU高性能深度学习推理库
- **特点**: 使用AMD的LowOHA（低开销硬件加速）MatMul算子
- **支持架构**: Zen 2及更新架构

#### 注意区别
- **ZenDNN**: AMD的深度学习库（本文档）
- **zDNN**: IBM的深度神经网络加速库

#### 操作系统支持
- **Linux**: Ubuntu 20.04, 22.04, 24.04

#### 硬件支持
- **AMD EPYC 9005系列** (Turin): 5代 - Zen 5架构
- **AMD EPYC 9004系列** (Genoa): 4代 - Zen 4架构
- **AMD EPYC 7003系列** (Milan): 3代 - Zen 3架构
- **AMD Ryzen AI MAX** (Strix Halo): 高性能移动处理器

#### 支持操作
- **MUL_MAT**: 通过ZenDNN LowOHA MatMul加速
- **MUL_MAT_ID**: 通过ZenDNN LowOHA MatMul加速（MoE）

#### 数据类型支持
- **FP32**: 全精度浮点
- **BF16**: BFloat16（在Zen 4/Zen 5上性能最佳）

#### 编译配置
##### 自动下载和构建（推荐）
```bash
cmake -B build -DGGML_ZENDNN=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j $(nproc)
```

##### 自定义ZenDNN路径
```bash
# 构建ZenDNN
git clone https://github.com/amd/ZenDNN.git
cd ZenDNN
mkdir build && cd build
cmake ..
cmake --build . --target all

# 使用自定义路径构建llama.cpp
export ZENDNN_ROOT=/path/to/ZenDNN/build/install
cmake -B build -DGGML_ZENDNN=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j $(nproc)
```

#### 运行配置
```bash
# 设置最优配置
export ZENDNNL_MATMUL_ALGO=1    # 阻塞AOCL DLP算法，最佳性能

# 启动服务器
./build/bin/llama-server \
    -m models/Llama-3.1-8B-Instruct.BF16.gguf \
    --host 0.0.0.0 \
    --port 8080 \
    -t 64
```

#### 性能优化
```bash
# NUMA系统
numactl --cpunodebind=0 --membind=0 ./build/bin/llama-server ...

# 最佳性能算法
export ZENDNNL_MATMUL_ALGO=1    # 阻塞AOCL DLP算法（推荐）
```

#### 已知问题
- **操作支持有限**: 目前只加速MUL_MAT和MUL_MAT_ID
- **BF16支持**: BF16操作需要AMD Zen 4或Zen 5架构
- **NUMA感知**: 多插槽系统可能需要手动NUMA绑定以获得最佳性能

---

### 9. zDNN后端 (zDNN.md)

#### 背景介绍
- **IBM zDNN**: IBM z系统深度神经网络加速库
- **目标硬件**: IBM Telum I和II处理器中的NNPA加速器
- **支持系统**: IBM z17及更新系统

#### 注意区别
- **zDNN**: IBM的深度神经网络加速库（本文档）
- **ZenDNN**: AMD的深度学习库

#### 软件和硬件支持
| 硬件级别 | 状态 | 已验证 |
|----------|------|--------|
| IBM z17 / LinuxONE 5 | 支持 | RHEL 9.6, IBM z17, 40 IFLs |
| IBM z16 / LinuxONE 4 | 不支持 | |

#### 数据类型支持
- **F32**: 支持
- **F16**: 支持
- **BF16**: 支持

#### CMake选项
- `GGML_ZDNN`: 编译llama.cpp时启用zDNN支持（默认OFF）
- `ZDNN_ROOT`: 覆盖zDNN库查找路径

#### 安装zDNN库
```bash
git clone --recurse-submodules https://github.com/IBM/zDNN
cd zDNN

autoreconf .
./configure --prefix=/opt/zdnn-libs

make build
sudo make install
```

#### 编译llama.cpp
```bash
git clone https://github.com/ggml-org/llama.cpp
cd llama.cpp

cmake -S . -G Ninja -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DGGML_ZDNN=ON \
    -DZDNN_ROOT=/opt/zdnn-libs
cmake --build build --config Release -j$(nproc)
```

---

## 后端选择建议总结

### GPU加速后端
- **NVIDIA GPU**: 优先使用CUDA后端
- **Apple Silicon**: 使用Metal后端（MTL）
- **Intel GPU**: 使用SYCL后端
- **通用GPU**: 使用Vulkan后端
- **Web端**: 使用WebGPU后端

### CPU优化后端
- **通用CPU**: 使用CPU后端
- **BLAS加速**: 使用BLIS后端
- **AMD EPYC**: 使用ZenDNN后端
- **IBM z系统**: 使用zDNN后端

### NPU专用后端
- **华为昇腾**: 使用CANN后端
- **Intel NPU**: 使用OpenVINO后端

### 虚拟化环境
- **虚拟机GPU**: 使用VirtGPU后端

### 移动端
- **Android OpenCL**: 使用OpenCL后端（Adreno GPU）

## 性能对比参考

### 后端性能排序（大致）
1. **CUDA** (NVIDIA GPU) - 最优性能
2. **Metal** (Apple Silicon) - macOS平台最优
3. **SYCL** (Intel GPU) - Intel平台良好性能
4. **ZenDNN** (AMD EPYC) - AMD CPU优化
5. **OpenVINO** (Intel NPU) - NPU专用优化
6. **CANN** (华为昇腾) - 昇腾NPU优化
7. **Vulkan** - 跨平台兼容性好
8. **OpenCL** - 兼容性好但性能一般
9. **CPU** - 通用但性能较低

### 兼容性考虑
- **跨平台**: Vulkan、OpenCL、CPU后端
- **硬件专用**: CUDA、Metal、SYCL、CANN等
- **开发阶段**: CPU后端便于调试

---

## GitHub贡献指南

各后端文档都提到了GitHub贡献时需要添加特定前缀：

- **[CANN]**: CANN后端相关问题
- **[SYCL]**: SYCL后端相关问题  
- **[ZenDNN]**: ZenDNN后端相关问题
- **[OpenVINO]**: OpenVINO后端相关问题

这样有助于后端团队快速识别和处理相关问题。

---

## 后端开发未来工作

### 共性TODO
- 性能和内存优化
- 精度验证
- 更广泛的量化覆盖
- 支持更多模型架构

### 特定后端TODO
- **OpenCL**: Q6_K优化、Q4_K支持、Flash Attention改进
- **SYCL**: AOT构建支持、Split-mode row支持
- **ZenDNN**: 扩展操作支持（注意力操作、激活函数等）
- **CANN**: Flash Attention量化版本更新

---

## 总结

llama.cpp提供了丰富的后端支持，覆盖了从通用CPU到专用加速器的各种硬件平台。选择合适后端时需要考虑：

1. **硬件兼容性**: 确保后端支持目标硬件
2. **性能需求**: 根据性能要求选择最优后端
3. **模型支持**: 检查模型和量化格式是否被支持
4. **部署环境**: 考虑操作系统、容器化等环境因素
5. **维护活跃度**: 选择维护活跃、文档完善的后端

通过合理选择和配置后端，可以在不同硬件平台上获得最佳的性能和兼容性。
