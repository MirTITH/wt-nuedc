#include "lv_app.hpp"

namespace lv_app
{

lv_style_t kStyleNoBorder; // 没有边框的框

void LvApp_Init()
{
    lv_style_init(&kStyleNoBorder);
    lv_style_set_border_side(&kStyleNoBorder, LV_BORDER_SIDE_NONE);
    lv_style_set_radius(&kStyleNoBorder, 0);
}
} // namespace lv_app
