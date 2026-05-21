/**
 * @file op-desc.h
 * @brief 操作描述符 - 用于格式化张量操作信息
 *
 * 本文件定义了 op_desc 结构体，用于格式化和显示张量操作的各种属性：
 * - 张量维度
 * - 步长
 * - 数据类型
 * - 缓冲区信息
 * - 张量名称
 *
 * @author llama.cpp contributors
 * @copyright MIT License
 */

#ifndef OP_DESC_H
#define OP_DESC_H

#define GGML_COMMON_IMPL_CPP
#include "ggml-backend-impl.h"
#include "ggml-common.h"

#include <string>
#include <stdio.h>

/**
 * @brief 操作描述符结构体
 *
 * 用于存储和格式化张量操作的详细信息，包括输入输出张量的
 * 维度、步长、类型、缓冲区和名称等信息。
 */
struct op_desc {
    char strides[64 * GGML_MAX_SRC];  // 步长信息字符串
    char dims[64 * GGML_MAX_SRC];     // 维度信息字符串
    char types[16 * GGML_MAX_SRC];    // 数据类型字符串
    char buffs[64 * GGML_MAX_SRC];    // 缓冲区信息字符串
    char names[64 * GGML_MAX_SRC];    // 张量名称字符串

    /**
     * @brief 格式化张量的维度信息
     * @param str 输出字符串缓冲区
     * @param t 张量指针
     * @return 写入的字符数
     */
    int format_tensor_dims(char * str, const struct ggml_tensor * t) {
        if (t->ne[2] == 1 && t->ne[3] == 1) {
            return sprintf(str, "%d:%d", (int) t->ne[0], (int) t->ne[1]);
        } else {
            return sprintf(str, "%d:%d:%d:%d", (int) t->ne[0], (int) t->ne[1], (int) t->ne[2], (int) t->ne[3]);
        }
    }

    /**
     * @brief 格式化操作的输入和输出维度信息
     * 格式为 "src0 x src1 -> dst"
     * @param str 输出字符串缓冲区
     * @param t 张量指针
     */
    void format_op_dims(char * str, const struct ggml_tensor * t) {
        char * p = str;

        // append src0 and src1 (if any)
        if (t->src[0]) {
            p += format_tensor_dims(p, t->src[0]);

            for (int i = 1; i < GGML_MAX_SRC && t->src[i]; i++) {
                p += sprintf(p, " x ");
                p += format_tensor_dims(p, t->src[i]);
            }

            p += sprintf(p, " -> ");
        }

        // format self dims separately for better visual alignment
        char self[64];
        format_tensor_dims(self, t);

        p += sprintf(p, "%s", self);
    }

    /**
     * @brief 格式化张量的步长信息
     * @param str 输出字符串缓冲区
     * @param t 张量指针
     * @return 写入的字符数
     */
    int format_tensor_strides(char * str, const struct ggml_tensor * t) {
        const char * c = ggml_is_contiguous(t) ? "" : "!";

        if (t->ne[2] == 1 && t->ne[3] == 1) {
            return sprintf(str, "%zu:%zu%s", (size_t) t->nb[0], (size_t) t->nb[1], c);
        } else {
            return sprintf(str, "%zu:%zu:%zu:%zu%s", (size_t) t->nb[0], (size_t) t->nb[1], (size_t) t->nb[2], (size_t) t->nb[3], c);
        }
    }

    /**
     * @brief 格式化操作的输入和输出步长信息
     * @param str 输出字符串缓冲区
     * @param t 张量指针
     */
    void format_op_strides(char * str, const struct ggml_tensor * t) {
        char * p = str;

        // append src0 and src1 (if any)
        if (t->src[0]) {
            p += format_tensor_strides(p, t->src[0]);

            for (int i = 1; i < GGML_MAX_SRC && t->src[i]; i++) {
                p += sprintf(p, " x ");
                p += format_tensor_strides(p, t->src[i]);
            }

            p += sprintf(p, " -> ");
        }

        // format self dims separately for better visual alignment
        char self[64];
        format_tensor_strides(self, t);

        p += sprintf(p, "%s", self);
    }

    /**
     * @brief 格式化操作的输入和输出数据类型信息
     * @param str 输出字符串缓冲区
     * @param t 张量指针
     */
    void format_op_types(char * str, const struct ggml_tensor * t) {
        char * p = str;

        // append src0 and src1 (if any)
        if (t->src[0]) {
            p += sprintf(p, "%s", ggml_type_name(t->src[0]->type));

            for (int i = 1; i < GGML_MAX_SRC && t->src[i]; i++) {
                p += sprintf(p, " x ");
                p += sprintf(p, "%s", ggml_type_name(t->src[i]->type));
            }

            p += sprintf(p, " -> ");
        }

        p += sprintf(p, "%s", ggml_type_name(t->type));
    }

    /**
     * @brief 获取张量的缓冲区名称
     * @param t 张量指针
     * @return 缓冲区名称，如果没有缓冲区则返回 "NONE"
     */
    const char * tensor_buff_name(const struct ggml_tensor * t) {
        if (t->buffer) {
            return ggml_backend_buffer_name(t->buffer);
        }
        return "NONE";
    }

    /**
     * @brief 格式化操作的输入和输出缓冲区信息
     * @param str 输出字符串缓冲区
     * @param t 张量指针
     */
    void format_op_buffs(char * str, const struct ggml_tensor * t) {
        char * p = str;

        // append src0 and src1 (if any)
        if (t->src[0]) {
            p += sprintf(p, "%s", tensor_buff_name(t->src[0]));

            for (int i = 1; i < GGML_MAX_SRC && t->src[i]; i++) {
                p += sprintf(p, " x ");
                p += sprintf(p, "%s", tensor_buff_name(t->src[i]));
            }

            p += sprintf(p, " -> ");
        }

        p += sprintf(p, "%s", tensor_buff_name(t));
    }

    /**
     * @brief 格式化操作的输入和输出张量名称信息
     * @param str 输出字符串缓冲区
     * @param t 张量指针
     */
    void format_op_names(char * str, const struct ggml_tensor * t) {
        char * p = str;

        // append src0 and src1 (if any)
        if (t->src[0]) {
            p += sprintf(p, "%s", t->src[0]->name);

            for (int i = 1; i < GGML_MAX_SRC && t->src[i]; i++) {
                p += sprintf(p, " x ");
                p += sprintf(p, "%s", t->src[i]->name);
            }

            p += sprintf(p, " -> ");
        }

        p += sprintf(p, "%s", t->name);
    }

    /**
     * @brief 格式化所有操作信息
     * @param op 张量操作
     */
    void format(const ggml_tensor * op) {
        format_op_dims(dims, op);
        format_op_strides(strides, op);
        format_op_types(types, op);
        format_op_buffs(buffs, op);
        format_op_names(names, op);
    }

    /**
     * @brief 默认构造函数
     */
    op_desc() {}
    /**
     * @brief 从张量操作构造描述符
     * @param op 张量操作
     */
    op_desc(const ggml_tensor * op) { format(op); }
};

#endif // OP_DESC_H
