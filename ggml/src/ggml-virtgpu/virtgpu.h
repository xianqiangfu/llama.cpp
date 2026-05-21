/**
 * @file virtgpu.h
 * @brief VirtIO GPU 后端接口头文件
 *
 * 本文件定义了 VirtIO GPU 后端的接口和数据结构，用于通过虚拟 GPU 进行计算加速。
 * 主要功能包括：
 * - VirtGPU 设备初始化和管理
 * - APIR（API Remoting）通信支持
 * - 共享内存管理
 * - 远程调用接口
 *
 * @author llama.cpp contributors
 * @copyright MIT License
 */

#pragma once

// clang-format off
#include "virtgpu-utils.h"
#include "virtgpu-shm.h"
#include "virtgpu-apir.h"

#include "backend/shared/api_remoting.h"
#include "backend/shared/apir_cs.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <threads.h>
#include <xf86drm.h>

#include <cstring>

#define VIRGL_RENDERER_UNSTABLE_APIS 1
#include "apir_hw.h"
#include <drm/virtgpu_drm.h>
#include "venus_hw.h"
// clang-format on

#ifndef VIRTGPU_DRM_CAPSET_APIR
// 将在 include/drm/virtgpu_drm.h 中定义，当以下合并请求合并时：
// https://gitlab.freedesktop.org/virgl/virglrenderer/-/merge_requests/1590/diffs
#    define VIRTGPU_DRM_CAPSET_APIR 10
#endif

// Mesa/Virglrenderer Venus 内部。仅在 Virglrenderer 的 Venus->APIR 过渡期间需要
#define VENUS_COMMAND_TYPE_LENGTH 331

#ifndef VIRTGPU_DRM_CAPSET_VENUS  // 仅在 Linux >= v6.16 时可用
#    define VIRTGPU_DRM_CAPSET_VENUS 4
#endif

typedef uint32_t virgl_renderer_capset;

/* 来自 src/virtio/vulkan/vn_renderer_virtgpu.c */
#define VIRTGPU_PCI_VENDOR_ID       0x1af4
#define VIRTGPU_PCI_DEVICE_ID       0x1050
#define VIRTGPU_BLOB_MEM_GUEST_VRAM 0x0004
#define VIRTGPU_PARAM_GUEST_VRAM    9

// 共享内存大小配置
#define SHMEM_DATA_SIZE  0x1830000  // 24MiB - 数据缓冲区
#define SHMEM_REPLY_SIZE 0x4000     // 16KiB - 响应缓冲区

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/**
 * @brief VirtGPU 操作结果枚举
 */
enum virt_gpu_result_t {
    APIR_SUCCESS                     = 0,  // 成功
    APIR_ERROR_INITIALIZATION_FAILED = -1, // 初始化失败
};

// printf 格式检查属性
#define PRINTFLIKE(f, a) __attribute__((format(__printf__, f, a)))

/**
 * @brief VirtGPU 设备结构体
 *
 * 管理与虚拟 GPU 设备的通信和状态
 */
struct virtgpu {
    bool use_apir_capset;  // 是否使用 APIR capset

    int fd;  // DRM 设备文件描述符

    // Capset 信息
    struct {
        virgl_renderer_capset      id;      // Capset ID
        uint32_t                   version; // Capset 版本
        virgl_renderer_capset_apir data;    // Capset 数据
    } capset;

    util_sparse_array shmem_array;  // 稀疏数组管理

    /* APIR 通信页面 */
    virtgpu_shmem reply_shmem;  // 响应共享内存
    virtgpu_shmem data_shmem;   // 数据共享内存

    /* 互斥锁保护共享 data_shmem 缓冲区免受并发访问 */
    mtx_t data_shmem_mutex;

    /* 缓存的设备信息，防止内存泄漏和竞争条件 */
    struct {
        char *   description;  // 设备描述
        char *   name;         // 设备名称
        int32_t  device_count; // 设备数量
        uint32_t type;         // 设备类型
        size_t   memory_free;  // 可用内存
        size_t   memory_total; // 总内存
    } cached_device_info;

    /* 缓存的缓冲区类型信息，防止内存泄漏和竞争条件 */
    struct {
        apir_buffer_type_host_handle_t host_handle; // 主机句柄
        char *                         name;       // 名称
        size_t                         alignment;  // 对齐要求
        size_t                         max_size;   // 最大大小
    } cached_buffer_type;
};

/**
 * @brief 执行 VirtGPU ioctl 调用
 * @param gpu VirtGPU 设备指针
 * @param request 请求类型
 * @param args 参数指针
 * @return ioctl 返回值
 */
static inline int virtgpu_ioctl(virtgpu * gpu, unsigned long request, void * args) {
    return drmIoctl(gpu->fd, request, args);
}

/**
 * @brief 创建 VirtGPU 设备
 * @return VirtGPU 设备指针
 */
virtgpu * create_virtgpu();

/**
 * @brief 准备远程调用
 * @param gpu VirtGPU 设备指针
 * @param apir_cmd_type APIR 命令类型
 * @param cmd_flags 命令标志
 * @return APIR 编码器指针
 */
apir_encoder * remote_call_prepare(virtgpu * gpu, ApirCommandType apir_cmd_type, int32_t cmd_flags);

/**
 * @brief 执行远程调用
 * @param gpu VirtGPU 设备指针
 * @param enc APIR 编码器
 * @param dec 输出的 APIR 解码器指针
 * @param max_wait_ms 最大等待时间（毫秒）
 * @param call_duration_ns 输出的调用持续时间（纳秒）
 * @return 命令 ID
 */
uint32_t remote_call(virtgpu *       gpu,
                     apir_encoder *  enc,
                     apir_decoder ** dec,
                     float           max_wait_ms,
                     long long *     call_duration_ns);

/**
 * @brief 完成远程调用，清理资源
 * @param gpu VirtGPU 设备指针
 * @param enc APIR 编码器
 * @param dec APIR 解码器
 */
void remote_call_finish(virtgpu * gpu, apir_encoder * enc, apir_decoder * dec);
