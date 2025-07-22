<!-- markdownlint-disable MD033 MD036 MD041 -->

<h1 align="center">ESP-IDF Quotes Component</h1>

<p align="center">
简体中文
· <a href="./CHANGELOG.txt">更新日志</a>
· <a href="https://github.com/MDLZCOOL/esp_quotes/issues">反馈问题</a>
</p>

<p align="center">
  <a href="./LICENSE.txt">
    <img alt="License" src="https://img.shields.io/badge/License-MIT-blue.svg" />
  </a>
  <a href="https://docs.espressif.com/projects/esp-idf/">
    <img alt="ESP-IDF" src="https://img.shields.io/badge/ESP--IDF-v5.3+-orange.svg" />
  </a>
  <a href="https://www.espressif.com/">
    <img alt="ESP-IDF" src="https://img.shields.io/badge/Platform-ESP--IDF-green.svg" />
  </a>
  <a href="">
    <img alt="Version" src="https://img.shields.io/badge/Version-v0.0.3-brightgreen.svg" />
  </a>
  <a href="https://github.com/MDLZCOOL/esp_quotes/stargazers">
    <img alt="GitHub Stars" src="https://img.shields.io/github/stars/MDLZCOOL/esp_quotes.svg?style=social&label=Stars" />
  </a>
</p>


---

## 📖 介绍

`esp_quotes` 是服务于 ESP-IDF 的组件，用于获取随机的名人名言、古诗词、每日一句、今日新闻等，提供多个内置 API 源的选择。

**已支持的 API：**

| API 名称 | 来源 | 说明 |
| :--- | :--- | :--- |
| `iciba` | [金山词霸](https://www.iciba.com/) | 提供每日一句英文以及对应的中文翻译和图片。 |
| `hitokoto` | [一言](https://hitokoto.cn/) | 提供一句源自于 ACG（动漫、漫画、游戏）文化等的句子。 |
| `jinrishici` | [今日诗词](https://www.jinrishici.com/) | 提供一句优美的中国古诗词。 |
| `shanbay` | [扇贝单词](https://web.shanbay.com/web/main) | 提供每日一句英文以及对应的中文翻译和图片。 |
| `xygeng-one` | [XY-API](https://docs.xygeng.cn/) | 提供一句源自于漫画、歌曲评论、影视剧等的句子 |
| `keai-wangyiyun` | [网易云热评](https://keai.icu/apiwyy/) | 提供一句随机的网易云热评 |

该组件封装了 HTTP 请求、JSON 解析和内存管理等复杂操作，提供了简洁的接口供上层应用调用。

## 🔔 安装

要将组件添加到项目中，请在 ESP-IDF 终端执行下方命令：

```bash
idf.py add-dependency "mdlzcool/esp_quotes*"
```

或者直接克隆本仓库到项目 `components` 目录下：

```bash
git clone https://github.com/MDLZCOOL/esp_quotes
```

## 📣 配置

在您的 ESP-IDF 项目根目录下，运行 `idf.py menuconfig` 命令来配置组件。

1. 进入 `Component config --->`
2. 找到 `Quotes Configuration --->`
3. 根据您的需求选择一个 API 来源。

```
(Top) → Component config → Quotes Configuration
[ ] none (Implement it yourself)
[ ] Use iciba API
[ ] Use hitokoto API
[ ] Use jinrishici API
[ ] Use shanbay API
```

> [!NOTE]  
> 如果您选择 `none (Implement yourself)`，`quotes_fetch` 函数将不会执行任何操作。您需要自行实现数据获取逻辑。

## 🎉 使用

以下是一个在您的项目种使用该组件的简单示例：

```c
#include "quotes.h"

void app_main(void)
{
    /* 联网部分省略 */
    
    /* 存储 quote 信息 */
    quote_info_t quote;
    
    /* quotes_fetch 会根据 menuconfig 中的配置自动选择 API */
    if (quotes_fetch(&quote) == 0)
    {
        /* 打印获取到的信息 */
        /* 获取到的信息存储在quote_info_t里，亦可直接调用 */
        quotes_print(&quote);
    }
    
    /* 用完后释放，防止内存泄漏 */
    quotes_info_free(&quote);
}
```

## 🤝 贡献

本项目采用 MIT 许可证，详情请参阅 [LICENSE.txt](./LICENSE.txt) 文件，本项目仍在完善中，可能会有一些功能尚未完善或存在 Bug。如果您在使用过程中遇到任何问题，请随时联系作者或提交 Issue 来帮助改进本项目！同时欢迎提供新的 API，我会审查后第一时间更新支持。

## 如果您喜欢本项目，请给我点个⭐吧(๑>◡<๑)！