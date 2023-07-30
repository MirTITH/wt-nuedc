#include "lvgl_ttf.h"
#include <stdint.h>

#define TTF_MEM_BASE  0x8080000                                   // 字体文件信息的地址
#define TTF_MEM_SIZE  (*(uint32_t *)TTF_MEM_BASE)                 // ttf 文件大小
#define TTF_MEM_START ((void *)(TTF_MEM_BASE + sizeof(uint32_t))) // ttf 文件起始地址

// 备用字模, TTF 加载失败时自动回退到这个
#define FALLBACK_FONT       LXWKMonoEn_16
#define FALLBACK_LARGE_FONT LXWKMonoEn_16
#define FALLBACK_SMALL_FONT LXWKMonoEn_12

LV_FONT_DECLARE(FALLBACK_FONT)
LV_FONT_DECLARE(FALLBACK_LARGE_FONT)
LV_FONT_DECLARE(FALLBACK_SMALL_FONT)

static lv_ft_info_t kTtfInfo      = {};
static lv_ft_info_t kTtfInfoLarge = {};
static lv_ft_info_t kTtfInfoSmall = {};

void LvglTTF_Init()
{
    kTtfInfo.name     = NULL;
    kTtfInfo.weight   = 18;                   // 字体大小
    kTtfInfo.style    = FT_FONT_STYLE_NORMAL; // 字体风格
    kTtfInfo.mem      = TTF_MEM_START;
    kTtfInfo.mem_size = TTF_MEM_SIZE;
    lv_ft_font_init(&kTtfInfo);

    kTtfInfoLarge.name     = NULL;
    kTtfInfoLarge.weight   = 40;                   // 字体大小
    kTtfInfoLarge.style    = FT_FONT_STYLE_NORMAL; // 字体风格
    kTtfInfoLarge.mem      = TTF_MEM_START;
    kTtfInfoLarge.mem_size = TTF_MEM_SIZE;
    lv_ft_font_init(&kTtfInfoLarge);

    kTtfInfoSmall.name     = NULL;
    kTtfInfoSmall.weight   = 12;                   // 字体大小
    kTtfInfoSmall.style    = FT_FONT_STYLE_NORMAL; // 字体风格
    kTtfInfoSmall.mem      = TTF_MEM_START;
    kTtfInfoSmall.mem_size = TTF_MEM_SIZE;
    lv_ft_font_init(&kTtfInfoSmall);
}

lv_font_t *LvglTTF_GetFont()
{
    if (kTtfInfo.font != NULL) {
        return kTtfInfo.font;
    } else {
        return (lv_font_t *)&FALLBACK_FONT;
    }
}

lv_font_t *LvglTTF_GetLargeFont()
{
    if (kTtfInfoLarge.font != NULL) {
        return kTtfInfoLarge.font;
    } else {
        return (lv_font_t *)&FALLBACK_LARGE_FONT;
    }
}

lv_font_t *LvglTTF_GetSmallFont()
{
    if (kTtfInfoSmall.font != NULL) {
        return kTtfInfoSmall.font;
    }

    return (lv_font_t *)&FALLBACK_SMALL_FONT;
}
