/**
 * @file libdl.h
 * @brief 跨平台动态库加载抽象层
 *
 * 本文件提供了跨平台的动态库加载接口，统一了 Windows 和 Unix/Linux
 * 系统上的动态库操作：
 * - Windows: 使用 LoadLibrary/GetProcAddress/FreeLibrary
 * - Unix/Linux: 使用 dlopen/dlsym/dlclose
 *
 * @author llama.cpp contributors
 * @copyright MIT License
 */

#pragma once

#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN
#   ifndef NOMINMAX
#       define NOMINMAX
#   endif
#   include <windows.h>
#   include <winevt.h>
#else
#    include <dlfcn.h>
#    include <unistd.h>
#endif
#include <filesystem>

namespace fs = std::filesystem;

#ifdef _WIN32

// Windows 平台实现
using dl_handle = std::remove_pointer_t<HMODULE>;

/**
 * @brief Windows 动态库句柄删除器
 * 用于智能指针自动释放库句柄
 */
struct dl_handle_deleter {
    void operator()(HMODULE handle) {
        FreeLibrary(handle);
    }
};

/**
 * @brief 加载 Windows 动态链接库
 * @param path 库文件路径
 * @return 库句柄
 */
static inline dl_handle * dl_load_library(const fs::path & path) {
    // 抑制缺失 DLL 的错误对话框
    DWORD old_mode = SetErrorMode(SEM_FAILCRITICALERRORS);
    SetErrorMode(old_mode | SEM_FAILCRITICALERRORS);

    HMODULE handle = LoadLibraryW(path.wstring().c_str());

    SetErrorMode(old_mode);

    return handle;
}

/**
 * @brief 从 Windows DLL 获取函数地址
 * @param handle 库句柄
 * @param name 函数名称
 * @return 函数指针
 */
static inline void * dl_get_sym(dl_handle * handle, const char * name) {
    DWORD old_mode = SetErrorMode(SEM_FAILCRITICALERRORS);
    SetErrorMode(old_mode | SEM_FAILCRITICALERRORS);

    void * p = (void *) GetProcAddress(handle, name);

    SetErrorMode(old_mode);

    return p;
}

/**
 * @brief 获取 Windows 平台错误信息
 * @return 错误字符串（Windows 平台暂不实现）
 */
static inline const char * dl_error() {
    return "";
}

#else

// Unix/Linux 平台实现
using dl_handle = void;

/**
 * @brief Unix 动态库句柄删除器
 */
struct dl_handle_deleter {
    void operator()(void * handle) {
        dlclose(handle);
    }
};

/**
 * @brief 加载 Unix/Linux 共享库
 * @param path 库文件路径
 * @return 库句柄
 */
static inline dl_handle * dl_load_library(const fs::path & path) {
    dl_handle * handle = dlopen(path.string().c_str(), RTLD_NOW | RTLD_LOCAL);
    return handle;
}

/**
 * @brief 从共享库获取符号地址
 * @param handle 库句柄
 * @param name 符号名称
 * @return 符号地址
 */
static inline void * dl_get_sym(dl_handle * handle, const char * name) {
    return dlsym(handle, name);
}

/**
 * @brief 获取 Unix/Linux 平台错误信息
 * @return 错误字符串
 */
static inline const char * dl_error() {
    const char *rslt = dlerror();
    return rslt != nullptr ? rslt : "";
}

#endif
