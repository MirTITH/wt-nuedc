#include "lvgl_ttf.h"
#include "fonts/ttf_font.h"

static lv_ft_info_t kTtfInfo;
static lv_ft_info_t kTtfInfoLarge;

void LvglTtf_Init()
{
    kTtfInfo.name     = NULL;
    kTtfInfo.weight   = 20;                   // 字体大小
    kTtfInfo.style    = FT_FONT_STYLE_NORMAL; // 字体风格
    kTtfInfo.mem      = TTF_MEM_START;
    kTtfInfo.mem_size = TTF_MEM_SIZE;

    lv_ft_font_init(&kTtfInfo);

    kTtfInfoLarge.name     = NULL;
    kTtfInfoLarge.weight   = 48;                   // 字体大小
    kTtfInfoLarge.style    = FT_FONT_STYLE_NORMAL; // 字体风格
    kTtfInfoLarge.mem      = TTF_MEM_START;
    kTtfInfoLarge.mem_size = TTF_MEM_SIZE;

    lv_ft_font_init(&kTtfInfoLarge);
}

lv_font_t *LvglTtf_GetFont()
{
    return kTtfInfo.font;
}

lv_font_t *LvglTtf_GetLargeFont()
{
    return kTtfInfoLarge.font;
}
