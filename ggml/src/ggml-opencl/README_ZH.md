# ggml-opencl - OpenCL 后端

## 概述

ggml-opencl 是基于 OpenCL (Open Computing Language) 的 GGML 后端实现，提供跨平台的 GPU 加速计算能力。OpenCL 支持多种硬件平台，包括 GPU、CPU 和其他加速器。

## 文件结构

```
ggml/src/ggml-opencl/
├── CMakeLists.txt          # CMake 构建配置
└── ggml-opencl.cpp          # OpenCL 后端实现
```

## 实现原理

### 架构设计

OpenCL 后端采用以下架构：

1. **平台层**: 发现和初始化 OpenCL 平台
2. **设备层**: 管理计算设备
3. **上下文层**: 管理 OpenCL 上下文和命令队列
4. **内核层**: 编译和执行 OpenCL 内核

### 核心组件

#### OpenCL 初始化

```cpp
cl_platform_id platform;
cl_device_id device;
cl_context context;
cl_command_queue queue;

// 初始化流程
clGetPlatformIDs(1, &platform, NULL);
clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
queue = clCreateCommandQueue(context, device, 0, NULL);
```

#### 内存管理

```cpp
// 分配缓冲区
cl_mem buffer = clCreateBuffer(
    context,
    CL_MEM_READ_WRITE,
    size,
    NULL,
    NULL
);

// 传输数据
clEnqueueWriteBuffer(
    queue,
    buffer,
    CL_TRUE,
    0,
    size,
    host_ptr,
    0,
    NULL,
    NULL
);
```

#### 内核编译

```cpp
cl_program program = clCreateProgramWithSource(
    context,
    1,
    &source,
    NULL,
    NULL
);

clBuildProgram(program, 1, &device, "-cl-std=CL3.0", NULL, NULL);
cl_kernel kernel = clCreateKernel(program, "kernel_name", NULL);
```

### 操作支持

OpenCL 后端支持的操作包括：

- **矩阵乘法**: MUL_MAT, MUL_MAT_ID
- **元素级操作**: ADD, MUL, DIV, SUB
- **归一化**: RMS_NORM, LAYER_NORM
- **激活函数**: GELU, SILU, TANH
- **RoPE**: 旋转位置编码
- **量化**: Q4_0, Q4_1, Q8_0 等格式

### 性能优化

#### 内核优化

1. **向量化**: 使用 OpenCL 向量类型 (float4, float8)
2. **局部内存**: 使用 `__local` 内存缓存数据
3. **工作组大小**: 根据设备特性优化工作组维度

#### 内存优化

1. **缓冲区重用**: 减少内存分配开销
2. **异步传输**: 使用事件同步提高吞吐量
3. **内存对齐**: 优化数据布局提高缓存利用率

### 设备查询

```cpp
cl_device_id device;
cl_char device_name[128];
cl_uint compute_units;
cl_ulong global_mem_size;

clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_name), device_name, NULL);
clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, NULL);
clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(global_mem_size), &global_mem_size, NULL);
```

### 错误处理

```cpp
#define CL_CHECK(err) \
    do { \
        if (err != CL_SUCCESS) { \
            GGML_LOG_ERROR("OpenCL error: %d at %s:%d\n", err, __FILE__, __LINE__); \
            return err; \
        } \
    } while (0)
```

## 使用方法

### 编译

```bash
# 基本 OpenCL 支持
cmake -DGGML_OPENCL=ON ..

# 指定 OpenCL 路径
cmake -DGGML_OPENCL=ON -DOpenCL_DIR=/path/to/opencl ..
```

### 运行

```bash
# 使用默认设备
./llama-cli --model model.gguf --backend opencl

# 选择特定设备
./llama-cli --model model.gguf --backend opencl --device 0
```

### 环境变量

| 变量 | 说明 |
|------|------|
| `OPENCL_DEVICE_TYPE` | 设备类型 (GPU/CPU/ACCELERATOR) |
| `OPENCL_PLATFORM_ID` | 平台 ID |
| `OPENCL_DEVICE_ID` | 设备 ID |

## 性能调优

### 工作组大小调整

```bash
# 设置工作组大小
export GGML_OPENCL_WORKGROUP_SIZE=256
```

### 内存模式选择

```bash
# 使用 USM (统一共享内存)
export GGML_OPENCL_USE_USM=1
```

## 限制

- 性能受 OpenCL 实现质量影响
- 不同平台的性能差异较大
- 部分高级特性可能不支持
- 相比 CUDA/HIP 优化程度较低

## 适用场景

1. **跨平台需求**: 需要在多种硬件上运行
2. **Intel 集成显卡**: Intel HD/Iris Graphics
3. **AMD GPU**: 不使用 ROCm 的情况
4. **GPU 加速**: 无专用 GPU 驱动的情况

## 相关链接

- [OpenCL 规范](https://www.khronos.org/opencl/)
- [OpenCL 编程指南](https://www.khronos.org/opencl/resources)
- [OpenCL C++ 绑定](https://github.com/KhronosGroup/OpenCL-CLHPP)