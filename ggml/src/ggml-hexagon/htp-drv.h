/**
 * @file htp-drv.h
 * @brief Hexagon 张量处理器 (HTP) 驱动接口头文件
 *
 * 本文件定义了与 Qualcomm Hexagon DSP HTP 驱动交互的接口，包括：
 * - 驱动初始化和域管理
 * - 架构版本查询
 * - 跨平台兼容性定义（Windows/Linux/Hexagon）
 * - 错误码和超时设置
 *
 * @author llama.cpp contributors
 * @copyright MIT License
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#    pragma clang diagnostic ignored "-Wignored-attributes"
#endif

#include <AEEStdErr.h>
#include <rpcmem.h>
#include <remote.h>
#include <dspqueue.h>

#if defined(_WIN32) && !defined(__MINGW32__)
#    ifdef GGML_BACKEND_BUILD
#        define HTPDRV_API __declspec(dllexport) extern
#    else
#        define HTPDRV_API __declspec(dllimport) extern
#    endif
#else
#    define HTPDRV_API __attribute__ ((visibility ("default"))) extern
#endif

/* 偏移量用于区分 HLOS 和 Hexagon 错误码。
   存储 Hexagon 的 AEE_EOFFSET 值。 */
#ifndef DSP_OFFSET
#    define DSP_OFFSET 0x80000400
#endif

/* 连接被对端重置的错误码。 */
#ifndef ECONNRESET
#    ifdef __hexagon__
#        define ECONNRESET 104
#    endif
#endif

/* 不同操作系统特定睡眠 API 的抽象。
   SLEEP 接受以秒为单位的输入。 */
#ifndef SLEEP
#    ifdef __hexagon__
#        define SLEEP(x)                      \
            { /* 模拟器不执行任何操作。 */ \
            }
#    else
#        ifdef _WIN32
#            define SLEEP(x) Sleep(1000 * x) /* Sleep 接受以毫秒为单位的输入。 */
#        else
#            define SLEEP(x) sleep(x)        /* sleep 接受以秒为单位的输入。 */
#        endif
#    endif
#endif

/* 包含 Windows 特定的头文件。 */
#ifdef _WIN32
#    include <windows.h>
#    include <sysinfoapi.h>
#    define _CRT_SECURE_NO_WARNINGS         1
#    define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#endif

/* 除 Windows 外所有 HLOS 的包含和定义 */
#if !defined(__hexagon__) && !defined(_WIN32)
#    include "unistd.h"

#    include <sys/time.h>
#endif

/* Hexagon 和除 Windows 外所有 HLOS 的包含和定义。 */
#if !defined(_WIN32)
/* 远程符号的弱引用用于编译。 */
#    pragma weak remote_session_control
#    pragma weak remote_handle_control
#    pragma weak remote_handle64_control
#    pragma weak fastrpc_mmap
#    pragma weak fastrpc_munmap
#    pragma weak rpcmem_alloc2
#endif

#if !defined(_WIN32)
#    pragma weak remote_system_request
#endif

#ifdef _WIN32
#     define DSPQUEUE_TIMEOUT DSPQUEUE_TIMEOUT_NONE
#else
#     define DSPQUEUE_TIMEOUT 1000000
#endif

/**
 * @brief htpdrv_init API: 驱动接口入口点
 *
 * @return      返回 Hexagon SDK 中定义的 AEE 错误码。
 */
HTPDRV_API int htpdrv_init(void);

/**
 * @brief get_domain API: 从域值获取域结构体。
 *
 * @param[in]  domain 一个域的值
 * @return     如果域受支持则返回域的结构体指针，否则返回 NULL。
 *
 */
HTPDRV_API domain * get_domain(int domain_id);

/**
 * @brief get_hex_arch_ver API: 查询 Hexagon 处理器架构版本信息
 *
 * @param[in]   domain_id 域的值
 * @param[out]  Arch 版本（73, 75, ...）
 * @return      查询成功返回 0。
 *              出错时返回非零值，返回值指向错误。
 *
 */
HTPDRV_API int get_hex_arch_ver(int domain, int * arch);

#ifdef __cplusplus
}
#endif
