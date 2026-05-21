# ggml-sycl - SYCL 后端

## 概述

ggml-sycl 是基于 Intel SYCL (SYCL = SYCL = C++ for heterogeneous computing) 的 GGML 后端实现，支持 Intel GPU、CPU 和其他加速设备。SYCL 是 Khronos Group 制定的跨平台异构计算标准。

## 文件结构

```
ggml/src/ggml-sycl/
├── add-id.hpp              # ID 加法操作
├── add-id.cpp              # ID 加法实现
├── backend.hpp             # 后端接口定义
├── binbcast.hpp            # 二进制广播操作
├── binbcast.cpp            # 二进制广播实现
├── common.hpp              # 通用头文件
├── common.cpp              # 通用函数
├── concat.hpp              # 拼接操作
├── concat.cpp              # 拼接实现
├── conv.hpp                # 卷积操作
├── conv.cpp                # 卷积实现
├── convert.hpp             # 类型转换
├── convert.cpp             # 类型转换实现
├── count-equal.hpp         # 相等计数
├── count-equal.cpp         # 相等计数实现
├── cpy.hpp                 # 复制操作
├── cpy.cpp                 # 复制实现
├── cumsum.hpp              # 累积和
├── cumsum.cpp              # 累积和实现
├── CMakeLists.txt          # CMake 构建配置
├── diag.hpp                # 对角线操作
├── diag.cpp                # 对角线实现
├── dmmv.hpp                # 量化矩阵向量乘法
├── dmmv.cpp                # 量化矩阵向量乘法实现
├── element_wise.hpp        # 元素级操作
├── element_wise.cpp        # 元素级操作实现
├── fattn.hpp               # Flash Attention 头文件
├── fattn.cpp               # Flash Attention 实现
├── fattn-buffers.hpp       # Flash Attention 缓冲区
├── fattn-buffers.cpp       # Flash Attention 缓冲区实现
├── fattn-common.hpp        # Flash Attention 通用定义
├── fattn-tile.hpp          # Flash Attention Tile 实现
├── fattn-tile.cpp          # Flash Attention Tile 实现
├── fattn-vec.hpp           # Flash Attention 向量实现
├── fill.hpp                # 填充操作
├── fill.cpp                # 填充实现
├── gated_delta_net.hpp     # 门控 Delta 网络
├── gated_delta_net.cpp     # 门控 Delta 网络实现
├── gemm.hpp                # 矩阵乘法头文件
├── getrows.hpp             # 获取行操作
├── getrows.cpp             # 获取行实现
├── ggml-sycl.cpp            # SYCL 后端主实现
├── gla.hpp                 # Gated Linear Attention
├── gla.cpp                 # Gated Linear Attention 实现
├── im2col.hpp              # Image to Column
├── im2col.cpp              # Image to Column 实现
├── mmq.hpp                 # 矩阵乘数量化
├── mmq.cpp                 # 矩阵乘数量化实现
├── mmvq.hpp                # 矩阵向量乘数量化
├── mmvq.cpp                # 矩阵向量乘数量化实现
├── norm.hpp                # 归一化操作
├── norm.cpp                # 归一化实现
├── opdesc.h                # 操作描述符
├── outprod.hpp             # 外积
├── outprod.cpp             # 外积实现
├── pad.hpp                 # 填充
├── pad.cpp                 # 填充实现
├── pad_reflect_1d.hpp      # 1D 反射填充
├── pad_reflect_1d.cpp      # 1D 反射填充实现
├── presets.hpp             # 预设配置
├── quantize.hpp            # 量化
├── quants.hpp              # 量化头文件
├── repeat_back.hpp         # 反向重复
├── repeat_back.cpp         # 反向重复实现
├── roll.hpp                # 滚动操作
├── roll.cpp                # 滚动实现
├── rope.hpp                # RoPE 位置编码
├── rope.cpp                # RoPE 实现
├── set.hpp                 # 设置操作
├── set.cpp                 # 设置实现
├── set_rows.hpp            # 设置行
├── set_rows.cpp            # 设置行实现
├── ssm_conv.hpp            # SSM 卷积
├── ssm_conv.cpp            # SSM 卷积实现
├── ssm_scan.hpp            # SSM 扫描
├── ssm_scan.cpp            # SSM 扫描实现
├── solve_tri.hpp           # 解三角系统
├── solve_tri.cpp           # 解三角系统实现
├── softmax.hpp             # Softmax 操作
├── softmax.cpp             # Softmax 实现
├── sycl_hw.hpp             # SYCL 硬件抽象
├── sycl_hw.cpp             # SYCL 硬件抽象实现
├── tsembd.hpp              # 词嵌入
├── tsembd.cpp              # 词嵌入实现
├── type.hpp                # 类型定义
├── upscale.hpp             # 上采样
├── upscale.cpp             # 上采样实现
├── vecdotq.hpp             # 向量点积量化
├── wkv.hpp                 # WKV (RWKV 组件)
└── wkv.cpp                 # WKV 实现
```

## 实现原理

### 架构设计

SYCL 后端采用模块化架构：

1. **硬件抽象层**: 封装不同设备的特性
2. **操作层**: 实现各种 GGML 操作
3. **内存管理**: 管理设备和主机内存
4. **内核层**: SYCL 内核实现

### 硬件抽象

#### 设备信息

```cpp
struct ggml_sycl_device_info {
    int device_count;
    struct {
        int cc;           // 计算能力
        int nsm;          // 计算单元数量
        size_t smpbo;     // 共享内存大小
        size_t max_work_group_size;
        int max_wg_per_cu; // 每个 CU 的最大工作组
        opt_feature opt_feature;
        struct ggml_sycl_hw_info* hw_info;
    } devices[GGML_SYCL_MAX_DEVICES];
    std::array<float, GGML_SYCL_MAX_DEVICES> default_tensor_split;
};
```

#### 优化特性

```cpp
struct optimize_feature {
    bool reorder;  // 是否支持重排序优化
};
```

### 内存管理

#### 设备缓冲区

```cpp
struct ggml_backend_sycl_buffer_context {
    int device;
    void * dev_ptr;
    queue_ptr stream;
    std::string name;
    optimize_feature opt_feature;
    std::vector<ggml_tensor_extra_gpu *> tensor_extras;
};
```

#### 分割缓冲区

多 GPU 场景下的张量分割：

```cpp
struct ggml_backend_sycl_split_buffer_context {
    std::vector<ggml_tensor_extra_gpu *> tensor_extras;
    std::vector<queue_ptr> streams;
};

static void get_row_split(
    int64_t * row_low,
    int64_t * row_high,
    const ggml_tensor * tensor,
    const std::array<float, GGML_SYCL_MAX_DEVICES> & tensor_split,
    int id
) {
    const int64_t nrows = ggml_nrows(tensor);
    const int64_t rounding = get_row_rounding(tensor->type, tensor_split);

    *row_low = id == 0 ? 0 : nrows * tensor_split[id];
    *row_low -= *row_low % rounding;
    
    if (id == ggml_sycl_info().device_count - 1) {
        *row_high = nrows;
    } else {
        *row_high = nrows * tensor_split[id + 1];
        *row_high -= *row_high % rounding;
    }
}
```

### 内存池

#### 设备内存池

```cpp
struct ggml_sycl_pool_leg : public ggml_sycl_pool {
    static const int MAX_SYCL_BUFFERS = 256;
    
    struct ggml_sycl_buffer {
        void * ptr;
        size_t size;
    };
    
    ggml_sycl_buffer buffer_pool[MAX_SYCL_BUFFERS];
    size_t pool_size;
    
    void * alloc(size_t size, size_t * actual_size) override;
    void free(void * ptr, size_t size) override;
};
```

#### 主机内存池

```cpp
struct ggml_sycl_pool_host : public ggml_sycl_pool {
    static constexpr int MAX_POOL_SIZE = 64;
    
    struct ggml_sycl_buffer {
        void * ptr;
        size_t size;
    };
    
    std::vector<ggml_sycl_buffer> buffer_pool;
    size_t pool_size;
};
```

### 操作实现

#### Flash Attention

```cpp
template<typename T, typename QD>
void flash_attn_ext_vec(
    const T* const Q,
    const T* const K,
    const T* const V,
    const float* const mask,
    T* const dst,
    const float scale,
    const float max_bias,
    const float logit_softcap,
    const int ne0,
    const int ne1,
    const int ne2,
    const int ne3,
    const int gqa,
    const sycl::nd_item<3>& item_ct1
);
```

#### 矩阵乘法

```cpp
typedef void (*ggml_sycl_op_mul_mat_t)(
    ggml_backend_sycl_context & ctx,
    const ggml_tensor *src0,
    const ggml_tensor *src1,
    ggml_tensor *dst,
    const char *src0_dd_i,
    const float *src1_ddf_i,
    const char *src1_ddq_i,
    float *dst_dd_i,
    const int64_t row_low,
    const int64_t row_high,
    const int64_t src1_ncols,
    const int64_t src1_padded_row_size,
    const queue_ptr &stream
);
```

#### 量化运算

```cpp
template <typename block_q, typename block_q_d>
inline void dequantize_block_q(
    const block_q * x,
    block_q_d * dst,
    const int64_t ib,
    const int64_t iqs,
    const int64_t n_max_kv,
    const sycl::nd_item<3> &item_ct1
);
```

### 设备管理

#### 设备枚举

```cpp
void ggml_backend_sycl_print_sycl_devices() {
    int device_count = dpct::dev_mgr::instance().device_count();
    
    GGML_LOG_INFO("Found %d SYCL devices:\n", device_count);
    
    for (int id = 0; id < device_count; ++id) {
        sycl::device device = dpct::dev_mgr::instance().get_device(id);
        std::string device_type = get_device_backend_and_type(device);
        
        // 打印设备信息
        GGML_LOG_INFO("|%2d|%19s|%39s|\n", 
            id, device_type.c_str(),
            device.get_info<sycl::info::device::name>().c_str()
        );
    }
}
```

#### 设备选择

```cpp
inline void check_allow_gpu_index(const int device_index) {
    if (device_index >= ggml_sycl_info().device_count) {
        GGML_LOG_ERROR("device_index:%d is out of range: [0-%d]\n",
            device_index, ggml_sycl_info().device_count - 1);
        assert(false);
    }
}
```

### 编译选项

#### 编译宏

| 宏 | 说明 |
|----|------|
| `GGML_SYCL_FORCE_MMQ` | 强制使用 MMQ 实现 |
| `GGML_SYCL_F16` | 启用 F16 支持 |
| `GGML_SYCL_GRAPH` | 启用 SYCL 图支持 |
| `GGML_SYCL_DNNL` | 启用 oneDNN |
| `GGML_SYCL_SUPPORT_LEVEL_ZERO` | 启用 Level Zero API |

#### 环境变量

| 变量 | 说明 |
|------|------|
| `GGML_SYCL_DEBUG` | 启用调试日志 |
| `GGML_SYCL_DISABLE_OPT` | 禁用优化 |
| `GGML_SYCL_DISABLE_GRAPH` | 禁用图模式 |
| `GGML_SYCL_DISABLE_DNN` | 禁用 oneDNN |
| `GGML_SYCL_PRIORITIZE_DMMV` | 优先使用 DMMV |
| `GGML_SYCL_ENABLE_LEVEL_ZERO` | 启用 Level Zero |
| `GGML_SYCL_ENABLE_FLASH_ATTN` | 启用 Flash Attention |
| `GGML_SYCL_USE_ASYNC_MEM_OP` | 使用异步内存操作 |

## 使用方法

### 编译

```bash
# 基本 SYCL 支持
cmake -DGGML_SYCL=ON ..

# 启用图支持
cmake -DGGML_SYCL=ON -DGGML_SYCL_GRAPH=ON ..

# 启用 oneDNN
cmake -DGGML_SYCL=ON -DGGML_SYCL_DNNL=ON ..

# 启用 Level Zero
cmake -DGGML_SYCL=ON -DGGML_SYCL_SUPPORT_LEVEL_ZERO=ON ..
```

### 运行

```bash
# 单 GPU
./llama-cli --model model.gguf --backend sycl --device 0

# 多 GPU 分割
./llama-cli --model model.gguf --backend sycl --split-mode layer --n-gpu 2

# 使用 Level Zero
./llama-cli --model model.gguf --backend sycl --use-level-zero
```

### 设备选择

```bash
# 仅使用 CPU
export ONEAPI_DEVICE_SELECTOR=cpu

# 使用 Intel GPU
export ONEAPI_DEVICE_SELECTOR=gpu

# 使用特定 GPU
export ONEAPI_DEVICE_SELECTOR=gpu:0
```

## 性能调优

### 图优化

```bash
# 启用图模式 (推荐)
export GGML_SYCL_GRAPH=1
./llama-cli --model model.gguf --backend sycl
```

### 内存优化

```bash
# 使用异步内存操作
export GGML_SYCL_USE_ASYNC_MEM_OP=1

# 禁用 VMM (如遇到问题)
export GGML_SYCL_NO_VMM=1
```

### Flash Attention

```bash
# 启用 Flash Attention (默认启用)
export GGML_SYCL_ENABLE_FLASH_ATTN=1
./llama-cli --model model.gguf --backend sycl
```

### Level Zero

Level Zero 提供更低的延迟和更好的性能：

```bash
export GGML_SYCL_ENABLE_LEVEL_ZERO=1
./llama-cli --model model.gguf --backend sycl
```

## 限制

- 需要 Intel oneAPI 基础工具包
- Windows 支持有限
- 部分高级特性需要特定硬件
- 编译时间较长

## 适用场景

1. **Intel CPU**: 利用 AVX-512 等指令集
2. **Intel GPU**: Intel Arc Graphics
3. **跨平台**: 代码可在不同设备上运行
4. **高性能计算**: 需要优化的矩阵运算

## 相关链接

- [SYCL 规范](https://www.khronos.org/sycl/)
- [Intel oneAPI](https://www.intel.com/content/www/us/en/developer/tools/oneapi/overview.html)
- [oneDNN](https://github.com/oneapi-src/oneDNN)
- [Level Zero](https://github.com/oneapi-src/level-zero)