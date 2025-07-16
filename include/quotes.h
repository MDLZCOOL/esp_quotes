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
        char *content;     ///< quote content
        char *translation; ///< translations (if available)
        char *source;      ///< source
        char *image;       ///< picture URL (if available)
    } quote_info_t;

    /**
     * @brief Initialize quote
     */
    void quotes_init(void);

    /**
     * @brief Fetch quote data
     *
     * @param out quote struct
     * @return int 0 success, -1 failure
     */
    int quotes_fetch(quote_info_t *out);

    /**
     * @brief Print available quote contents
     *
     * @param quote quote struct
     */
    void quotes_print(const quote_info_t *quote);

    /**
     * @brief Free quote memory
     *
     * @param quote quote struct
     */
    void quotes_info_free(quote_info_t *quote);

#ifdef __cplusplus
}
#endif
