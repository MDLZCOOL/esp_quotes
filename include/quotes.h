#pragma once

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Quote result structure
     */
    typedef struct
    {
        char *content;     ///< 原文
        char *translation; ///< 译文（如果有）
        char *source;      ///< 来源
        char *image;       ///< 图片 URL（如果有）
    } quote_info_t;

    /**
     * @brief 初始化 quote
     */
    void quotes_init(void);

    /**
     * @brief 获取 quote 数据
     *
     * @param out 填充的 quote 信息
     * @return int 0 成功，-1 失败
     */
    int quotes_fetch(quote_info_t *out);

    /**
     * @brief 打印 quote 内容
     *
     * @param quote 待输出的 quote 信息
     */
    void quotes_print(const quote_info_t *quote);

    /**
     * @brief 释放 quote 动态内存
     *
     * @param quote 待释放的 quote 信息
     */
    void quotes_info_free(quote_info_t *quote);

#ifdef __cplusplus
}
#endif
