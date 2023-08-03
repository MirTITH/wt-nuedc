#include "main_page.hpp"
#include "screeen_console.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ads1256/ads1256_device.hpp"
#include <string>
#include "thread_priority_def.h"
#include "lvgl/lvgl_thread.h"
#include "lv_app_text_field.hpp"
#include "Adc/adc_class_device.hpp"
#include "fast_tim_callback.hpp"
#include "TouchScreen/GT911/gt911_device.hpp"
#include "freertos_io/uart_device.hpp"
#include <cmath>
#include "Keyboard/keyboard_device.hpp"
#include "HighPrecisionTime/stat.hpp"
#include "fast_tim_callback.hpp"
#include "states.hpp"
#include "common_objs.hpp"

using namespace std;

static const lv_color_t kAColor = lv_color_make(39, 122, 240);
static const lv_color_t kBColor = lv_color_make(193, 24, 217);

static lv_obj_t *kMainTab;
static lv_obj_t *kConsoleTab;
lv_obj_t *kTextAreaConsole;

static lv_coord_t kContentWidth;

void ScreenConsole_AddText(const char *txt)
{
    if (kTextAreaConsole != nullptr) {
        LvglLock();
        lv_textarea_add_text(kTextAreaConsole, txt);
        LvglUnlock();
    }
}

static void MainPage_ThreadFastLoop(void *)
{
    const uint32_t period = 100;

    LvglLock();
    LvSimpleTextField tf_kmod(kMainTab, "期望正弦幅值", kContentWidth * 2 / 3);
    // LvSimpleTextField tf_encoder(kMainTab, "Enc,Sw", kContentWidth / 2);
    LvglUnlock();

    uint32_t PreviousWakeTime = xTaskGetTickCount();
    while (1) {
        // tf_kmod
        LvglLock();
        extern std::atomic<float> kAcReference;
        lv_label_set_text_fmt(tf_kmod.GetMsgLabel(), "%f", kAcReference.load());

        // tf_encoder
        // lv_label_set_text_fmt(tf_encoder.GetMsgLabel(), "%ld,%d,%d", KeyboardEncoder.Count(),
        //                       Keyboard_Read(Keys::kSwitch),
        //                       Keyboard_Read(Keys::kKnobBtn));

        LvglUnlock();
        vTaskDelayUntil(&PreviousWakeTime, period);
    }
}

static void MainPage_Thread(void *)
{
    const uint32_t period = 1000;

    LvglLock();
    LvTextField tf_states(kMainTab, "Untitled", kContentWidth);

    switch (kWhoAmI) {
        case BoardSelector::A:
            tf_states.SetTitle("A板状态");
            tf_states.SetTextColor(kAColor);
            break;
        case BoardSelector::B:
            tf_states.SetTitle("B板状态");
            tf_states.SetTextColor(kBColor);
            break;
        default:
            tf_states.SetTitle("我不知道我是谁！");
            break;
    }

    LvTextField tf_drdy(kMainTab, "VAds,IAds", kContentWidth / 2, 70, LvglTTF_GetFont());
    LvTextField tf_adc_rate(kMainTab, "ADC1,3速率", kContentWidth / 2, 70, LvglTTF_GetFont());
    LvTextField tf_fast_tim(kMainTab, "FastTim", kContentWidth / 2, 70, LvglTTF_GetFont());
    LvTextField tf_main_uart(kMainTab, "UART发送速率", kContentWidth / 2);
    LvglUnlock();

    CounterFreqMeter vads_drdy_meter(&VAds.drdy_count_);
    CounterFreqMeter iads_drdy_meter(&IAds.drdy_count_);
    CounterFreqMeter vads_sample_rate_meter(&VAds.data_sample_count_);
    CounterFreqMeter iads_sample_rate_meter(&IAds.data_sample_count_);

    CounterFreqMeter adc_interrupt_meter1(&Adc1.conv_cplt_count);
    CounterFreqMeter adc_interrupt_meter3(&Adc3.conv_cplt_count);

    CounterFreqMeter fast_tim_meter(&kFastTimCallbackCount);
    CounterFreqMeter main_uart_meter(&MainUart.uart_device.total_tx_size_);

    // char str_buffer[20];

    uint32_t PreviousWakeTime = xTaskGetTickCount();
    while (1) {
        LvglLock();
        // ADS1256
        lv_label_set_text_fmt(tf_drdy.GetMsgLabel(),
                              "采样:%lu,%lu\nbusy:%lu, %lu",
                              //   vads_drdy_meter.MeasureFreq(),
                              //   iads_drdy_meter.MeasureFreq(),
                              vads_sample_rate_meter.MeasureFreq(),
                              iads_sample_rate_meter.MeasureFreq(),
                              VAds.dma_busy_count_,
                              IAds.dma_busy_count_);

        // ADC
        lv_label_set_text_fmt(tf_adc_rate.GetMsgLabel(), "%lu,%lu",
                              adc_interrupt_meter1.MeasureFreq(),
                              adc_interrupt_meter3.MeasureFreq());

        // FastTim
        lv_label_set_text_fmt(tf_fast_tim.GetMsgLabel(),
                              "%lu Hz\n%lu us",
                              fast_tim_meter.MeasureFreq(), kFastTimCallbackDuration);

        // MainUart
        lv_label_set_text_fmt(tf_main_uart.GetMsgLabel(), "%lu", main_uart_meter.MeasureFreq());

        // 状态
        switch (kAppState.GetState()) {
            case AppState_t::Stop:
                tf_states.SetMsg("停止模式");
                break;
            case AppState_t::ActiveInv:
                tf_states.SetMsg("主动逆变");
                break;
            case AppState_t::PassiveInv:
                tf_states.SetMsg("被动逆变");
                break;
            case AppState_t::OnGridInv:
                tf_states.SetMsg("并网逆变");
                break;
            default:
                tf_states.SetMsg("未知模式");
                break;
        }

        LvglUnlock();
        vTaskDelayUntil(&PreviousWakeTime, period);
    }
}

void MainPage_Init()
{
    LvglLock();
    /*Create a Tab view object*/
    auto tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);
    lv_obj_set_height(tabview, 440); // 除去底部 monitor
    // lv_obj_set_style_text_font(tabview, LvglTTF_GetFont(), 0);

    kMainTab    = lv_tabview_add_tab(tabview, "信息");
    kConsoleTab = lv_tabview_add_tab(tabview, "终端");

    lv_obj_set_style_pad_hor(kConsoleTab, 0, 0);
    lv_obj_set_style_pad_ver(kConsoleTab, 0, 0);
    kTextAreaConsole = lv_textarea_create(kConsoleTab);
    lv_obj_set_style_text_font(kTextAreaConsole, LvglTTF_GetSmallFont(), 0);
    lv_obj_set_size(kTextAreaConsole, lv_pct(100), lv_pct(100));
    lv_textarea_set_placeholder_text(kTextAreaConsole, "Empty here");

    switch (kWhoAmI) {
        case BoardSelector::A:
            lv_obj_set_style_text_color(kTextAreaConsole, kAColor, 0);
            ScreenConsole_AddText("I am A board!\n");
            break;
        case BoardSelector::B:
            lv_obj_set_style_text_color(kTextAreaConsole, kBColor, 0);
            ScreenConsole_AddText("I am B board!\n");
            break;
        default:
            lv_obj_set_style_text_color(kTextAreaConsole, lv_color_make(200, 0, 0), 0);
            ScreenConsole_AddText("I don't known who I am!\n");
            break;
    }

    lv_obj_set_style_border_side(kMainTab, LV_BORDER_SIDE_NONE, 0);
    // lv_obj_set_size(kMainTab, lv_pct(100), 440); // 除去底部 monitor
    lv_obj_set_style_pad_hor(kMainTab, 4, 0);
    lv_obj_set_style_pad_ver(kMainTab, 0, 0);
    // lv_obj_set_style_pad_row(kMainTab, 4, 0);
    lv_obj_set_style_pad_column(kMainTab, 0, 0);
    lv_obj_set_flex_flow(kMainTab, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(kMainTab, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_update_layout(kMainTab);
    kContentWidth = lv_obj_get_content_width(kMainTab);

    // 切换到显示终端
    lv_obj_scroll_to_view_recursive(kTextAreaConsole, LV_ANIM_OFF);

    LvglUnlock();

    xTaskCreate(MainPage_ThreadFastLoop, "main_page_fast", 1024 * 2, nullptr, PriorityBelowNormal, nullptr);
    vTaskDelay(100);
    xTaskCreate(MainPage_Thread, "main_page", 1024 * 2, nullptr, PriorityBelowNormal, nullptr);
}
