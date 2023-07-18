#include "lvgl_ttf.h"
#include "fonts/ttf_font.h"

static lv_ft_info_t kTtfInfo;

void LvglTtf_Init()
{
    kTtfInfo.name     = NULL;
    kTtfInfo.weight   = 20;                   // 字体大小
    kTtfInfo.style    = FT_FONT_STYLE_NORMAL; // 字体风格
    kTtfInfo.mem      = TTF_MEM_START;
    kTtfInfo.mem_size = TTF_MEM_SIZE;

    lv_ft_font_init(&kTtfInfo);
}

lv_font_t *LvglTtf_GetFont()
{
    return kTtfInfo.font;
}
