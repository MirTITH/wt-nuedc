#include "main_page.hpp"
#include "lvgl/lvgl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ads1256/ads1256_device.hpp"
#include <string>
#include "thread_priority_def.h"
#include "lvgl/lvgl_thread.h"
#include "lv_app_text_field.hpp"
#include "Adc/adc_class_device.hpp"

namespace lv_app
{

using namespace std;

static void MainPage_Thread(void *)
{
    const uint32_t period = 500;

    LvglLock();
    lv_obj_t *cont_col = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont_col, lv_pct(100), 440); // 除去底部 monitor
    lv_obj_set_style_pad_all(cont_col, 5, 0);
    lv_obj_set_flex_flow(cont_col, LV_FLEX_FLOW_COLUMN);

    LvTextField tf_drdy(cont_col, "VAds, IAds DRDY 频率");
    LvTextField tf_adc_rate(cont_col, "内置 ADC 1,2,3 速率");

    LvglUnlock();

    auto vads_last_count = VAds.drdy_count_;
    auto iads_last_count = IAds.drdy_count_;
    uint32_t adc_now_count[3];
    uint32_t adc_last_count[3] = {};

    // std::stringstream sstr;

    std::string str;

    uint32_t PreviousWakeTime = xTaskGetTickCount();
    while (1) {
        auto vads_now_count = VAds.drdy_count_;
        auto iads_now_count = IAds.drdy_count_;

        str = to_string((vads_now_count - vads_last_count) * 1000 / period).append(",") +
              to_string((vads_now_count - iads_last_count) * 1000 / period);

        vads_last_count = vads_now_count;
        iads_last_count = iads_now_count;
        LvglLock();
        tf_drdy.SetMsg(str);
        LvglUnlock();

        adc_now_count[0] = Adc1.conv_cplt_count;
        adc_now_count[1] = Adc2.conv_cplt_count;
        adc_now_count[2] = Adc3.conv_cplt_count;

        str = to_string((adc_now_count[0] - adc_last_count[0]) * 1000 / period).append(",") +
              to_string((adc_now_count[1] - adc_last_count[1]) * 1000 / period).append(",") +
              to_string((adc_now_count[2] - adc_last_count[2]) * 1000 / period);

        for (size_t i = 0; i < 3; i++) {
            adc_last_count[i] = adc_now_count[i];
        }
        LvglLock();
        tf_adc_rate.SetMsg(str);
        LvglUnlock();

        vTaskDelayUntil(&PreviousWakeTime, period);
    }
}

void MainPage_Init()
{
    xTaskCreate(MainPage_Thread, "main_page", 1024 * 2, nullptr, PriorityBelowNormal, nullptr);
}

} // namespace lv_app
