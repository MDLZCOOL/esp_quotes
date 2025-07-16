#include "quotes.h"
#include "cJSON.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>

static const char* TAG = "QUOTES";
#define MAX_HTTP_RECV_BUFFER 8192

#define SAFE_FREE(p)                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        if (p)                                                                                                         \
        {                                                                                                              \
            free(p);                                                                                                   \
            (p) = NULL;                                                                                                \
        }                                                                                                              \
    } while (0)
#define PARSE_JSON(json_str, root_var)   \
do {                                     \
root_var = cJSON_Parse(json_str);    \
if (!root_var)                       \
{                                    \
SAFE_FREE(json_str);             \
return -1;                  \
}                                    \
} while(0)

typedef struct
{
    char* buf;
    size_t len;
    size_t max_len;
} http_buf_t;

// HTTP callback handler
static esp_err_t _http_event_handler(esp_http_client_event_t* evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA)
    {
        http_buf_t* ctx = (http_buf_t*)evt->user_data;
        if (ctx && ctx->buf && ctx->len + evt->data_len < ctx->max_len)
        {
            memcpy(ctx->buf + ctx->len, evt->data, evt->data_len);
            ctx->len += evt->data_len;
        }
    }
    return ESP_OK;
}

// execute HTTP request
static char* http_fetch(const char* url)
{
    http_buf_t recv_ctx = {.buf = calloc(1, MAX_HTTP_RECV_BUFFER), .len = 0, .max_len = MAX_HTTP_RECV_BUFFER};
    if (!recv_ctx.buf)
        return NULL;

    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        .user_data = &recv_ctx,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .timeout_ms = 10000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client)
    {
        SAFE_FREE(recv_ctx.buf);
        return NULL;
    }

#ifdef CONFIG_USE_QUOTE_API_JINRISHICI
    esp_http_client_set_header(client, "X-User-Token", "mzoL+1kXlv5PO9gC6ZDpU4DFr5e8FRfh");
#endif

    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);

    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "HTTP perform error: %s", esp_err_to_name(err));
        SAFE_FREE(recv_ctx.buf);
        return NULL;
    }

    char* result = strndup(recv_ctx.buf, recv_ctx.len);
    SAFE_FREE(recv_ctx.buf);
    return result;
}

// 金山词霸 iciba
// https://open.iciba.com/dsapi/
static int fetch_iciba(quote_info_t* out)
{
    char* json = http_fetch("https://open.iciba.com/dsapi/");
    if (!json)
        return -1;

    cJSON* root = NULL;
    PARSE_JSON(json, root);

    cJSON* content = cJSON_GetObjectItem(root, "content");
    cJSON* note = cJSON_GetObjectItem(root, "note");
    cJSON* pic = cJSON_GetObjectItem(root, "picture");

    if (content && content->valuestring)
        out->content = strdup(content->valuestring);
    if (note && note->valuestring)
        out->translation = strdup(note->valuestring);
    if (pic && pic->valuestring)
        out->image = strdup(pic->valuestring);

    out->source = strdup("iciba");

    cJSON_Delete(root);
    SAFE_FREE(json);
    return 0;
}

// 一言 hitokoto
// https://v1.hitokoto.cn/?encode=json
static int fetch_hitokoto(quote_info_t* out)
{
    char* json = http_fetch("https://v1.hitokoto.cn/?encode=json");
    if (!json)
        return -1;

    cJSON* root = NULL;
    PARSE_JSON(json, root);

    cJSON* hitokoto = cJSON_GetObjectItem(root, "hitokoto");
    cJSON* from = cJSON_GetObjectItem(root, "from");

    if (hitokoto && hitokoto->valuestring)
        out->content = strdup(hitokoto->valuestring);
    if (from && from->valuestring)
        out->source = strdup(from->valuestring);

    cJSON_Delete(root);
    SAFE_FREE(json);
    return 0;
}

// 今日诗词 jinrishici
// https://v2.jinrishici.com/sentence
static int fetch_jinrishici(quote_info_t* out)
{
    char* json = http_fetch("https://v2.jinrishici.com/sentence");
    if (!json)
        return -1;

    cJSON* root = NULL;
    PARSE_JSON(json, root);

    cJSON* data = cJSON_GetObjectItem(root, "data");
    if (data)
    {
        cJSON* content = cJSON_GetObjectItem(data, "content");
        if (content && content->valuestring)
            out->content = strdup(content->valuestring);
        cJSON* origin = cJSON_GetObjectItem(data, "origin");
        if (origin)
        {
            cJSON* author = cJSON_GetObjectItem(origin, "author");
            if (author && author->valuestring)
                out->source = strdup(author->valuestring);
        }
    }

    cJSON_Delete(root);
    SAFE_FREE(json);
    return 0;
}

// 今日诗词 jinrishici token
// https://v2.jinrishici.com/token
static int fetch_jinrishici_token(quote_info_t* out)
{
    char* json = http_fetch("https://v2.jinrishici.com/token");
    if (!json)
        return -1;

    cJSON* root = NULL;
    PARSE_JSON(json, root);

    cJSON* data = cJSON_GetObjectItem(root, "data");
    if (data && data->valuestring)
        ESP_LOGI(TAG, "Jinrishici token: %s", data->valuestring);

    cJSON_Delete(root);
    SAFE_FREE(json);
    return 0;
}

// 扇贝单词 shanbay
// https://apiv3.shanbay.com/weapps/dailyquote/quote
static int fetch_shanbay(quote_info_t* out)
{
    char* json = http_fetch("https://apiv3.shanbay.com/weapps/dailyquote/quote");
    if (!json)
        return -1;

    cJSON* root = NULL;
    PARSE_JSON(json, root);

    cJSON* content = cJSON_GetObjectItem(root, "content");
    cJSON* author = cJSON_GetObjectItem(root, "author");
    cJSON* translation = cJSON_GetObjectItem(root, "translation");
    cJSON* picture = cJSON_GetObjectItem(root, "poster_img_urls");

    if (content && content->valuestring)
        out->content = strdup(content->valuestring);
    if (author && author->valuestring)
        out->source = strdup(author->valuestring);
    if (translation && translation->valuestring)
        out->translation = strdup(translation->valuestring);
    cJSON* url_item = cJSON_GetArrayItem(picture, 0);
    if (cJSON_IsString(url_item) && url_item->valuestring)
        out->image = strdup(url_item->valuestring);

    cJSON_Delete(root);
    SAFE_FREE(json);
    return 0;
}

__attribute__((unused)) void quotes_init(void)
{
    // reserve for future use
}

int quotes_fetch(quote_info_t* out)
{
    if (!out)
        return -1;
    memset(out, 0, sizeof(quote_info_t));
#if CONFIG_USE_QUOTE_API_NONE
#warning "Please implement your api interface"
    return -1;
#elif CONFIG_USE_QUOTE_API_ICIBA
    return fetch_iciba(out);
#elif CONFIG_USE_QUOTE_API_HITOKOTO
    return fetch_hitokoto(out);
#elif CONFIG_USE_QUOTE_API_JINRISHICI
    return fetch_jinrishici(out);
#elif CONFIG_USE_QUOTE_API_SHANBAY
    return fetch_shanbay(out);
#else
#error "Unknown quote API"
#endif
}

void quotes_print(const quote_info_t* quote)
{
    if (!quote)
        return;

    printf("\n🎯 Quote of the Day:\n");
    if (quote->content)
        printf("📜  %s\n", quote->content);
    if (quote->translation)
        printf("🌍  %s\n", quote->translation);
    if (quote->source)
        printf("🔗  Source: %s\n", quote->source);
    if (quote->image)
        printf("🖼️  Image: %s\n", quote->image);
    printf("\n");
}

void quotes_info_free(quote_info_t* quote)
{
    if (!quote)
        return;

    SAFE_FREE(quote->content);
    SAFE_FREE(quote->translation);
    SAFE_FREE(quote->source);
    SAFE_FREE(quote->image);
}
