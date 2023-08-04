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
#include "freertos_lock/freertos_lock.hpp"
#include <cstring>
#include "DelayHolder/delay_holder.hpp"

using namespace std;

static const lv_color_t kAColor = lv_color_make(39, 122, 240);
static const lv_color_t kBColor = lv_color_make(193, 24, 217);

/**
 * @brief Tabs
 *
 */
static lv_obj_t *kMainTab;
static lv_obj_t *kConsoleTab;
static lv_obj_t *kLvScreenConsole;
static lv_coord_t kContentWidth;

/**
 * @brief ScreenConsole
 *
 */
static constexpr size_t kSCREEN_CONSOLE_SIZE        = 512;
static constexpr size_t kSCREEN_CONSOLE_DEL_SIZE    = kSCREEN_CONSOLE_SIZE / 4;
static constexpr size_t kSCREEN_CONSOLE_BUFFER_SIZE = 128;
static char kScreenConsoleBuffer[kSCREEN_CONSOLE_BUFFER_SIZE]; // 输入缓冲区
static freertos_lock::BinarySemphr kScreenConsoleLock;
static TaskHandle_t kScreenConsoleHandle;

void ScreenConsole_AddText(const char *txt)
{
    if (kLvScreenConsole != nullptr && kScreenConsoleHandle != nullptr) {
        if (InHandlerMode()) {
            if (kScreenConsoleLock.lock_from_isr()) {
                strncpy(kScreenConsoleBuffer, txt, sizeof(kScreenConsoleBuffer));

                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                vTaskNotifyGiveFromISR(kScreenConsoleHandle, &xHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            }
        } else {
            kScreenConsoleLock.lock_from_thread();

            strncpy(kScreenConsoleBuffer, txt, sizeof(kScreenConsoleBuffer));

            xTaskNotifyGive(kScreenConsoleHandle);
        }
    }
}

static void ScreenConsole_Entry(void *)
{
    auto text_ptr = (char *)pvPortMalloc(kSCREEN_CONSOLE_SIZE);
    assert(text_ptr != nullptr);
    kScreenConsoleLock.unlock_from_thread();
    size_t text_size = 0;

    lv_label_set_text_static(kLvScreenConsole, text_ptr);

    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        auto new_text_size = strnlen(kScreenConsoleBuffer, sizeof(kScreenConsoleBuffer) - 1);
        if (new_text_size > kSCREEN_CONSOLE_SIZE - 1) {
            new_text_size = kSCREEN_CONSOLE_SIZE - 1;
        }

        // 删除旧文本
        if (new_text_size > kSCREEN_CONSOLE_SIZE - text_size - 1) {
            size_t del_size = kSCREEN_CONSOLE_DEL_SIZE < text_size ? kSCREEN_CONSOLE_DEL_SIZE : text_size;
            text_size -= del_size;
            memmove(text_ptr, text_ptr + del_size, text_size);
        }

        memcpy(text_ptr + text_size, kScreenConsoleBuffer, new_text_size);
        text_size += new_text_size;
        *(text_ptr + text_size + 1) = '\0';

        LvglLock();
        lv_label_set_text_static(kLvScreenConsole, nullptr);
        LvglUnlock();

        kScreenConsoleLock.unlock_from_thread();
    }
}

static void MainPage_Thread(void *)
{
    const uint32_t period = 100;

    LvglLock();
    // 任务状态和板子信息
    LvTextField tf_states(kMainTab, "Untitled", kContentWidth);
    lv_obj_set_style_text_align(tf_states.GetMsgLabel(), LV_TEXT_ALIGN_CENTER, 0);

    switch (kWhoAmI) {
        case BoardSelector::A:
            tf_states.SetTitle("A 板");
            tf_states.SetTextColor(kAColor);
            break;
        case BoardSelector::B:
            tf_states.SetTitle("B 板");
            tf_states.SetTextColor(kBColor);
            break;
        default:
            tf_states.SetTitle("我不知道我是谁！");
            break;
    }

    LvTextField tf_kmod(kMainTab, "期望正弦幅值", kContentWidth * 2 / 3);
    LvTextField tf_grid_status(kMainTab, "电网状态", kContentWidth / 3);
    LvTextField tf_drdy(kMainTab, "VAds,IAds", kContentWidth / 2, 70, LvglTTF_GetFont());
    LvTextField tf_adc_rate(kMainTab, "ADC1,2速率", kContentWidth / 2, 70, LvglTTF_GetFont());
    LvTextField tf_fast_tim(kMainTab, "FastTim", kContentWidth / 2, 70, LvglTTF_GetFont());
    LvTextField tf_main_uart(kMainTab, "UART发送速率", kContentWidth / 2);
    LvglUnlock();

    CounterFreqMeter vads_drdy_meter(&VAds.drdy_count_);
    CounterFreqMeter iads_drdy_meter(&IAds.drdy_count_);
    CounterFreqMeter vads_sample_rate_meter(&VAds.data_sample_count_);
    CounterFreqMeter iads_sample_rate_meter(&IAds.data_sample_count_);

    CounterFreqMeter adc_interrupt_meter1(&Adc1.conv_cplt_count);
    CounterFreqMeter adc_interrupt_meter2(&Adc2.conv_cplt_count);

    CounterFreqMeter fast_tim_meter(&kFastTimCallbackCount);
    CounterFreqMeter main_uart_meter(&MainUart.uart_device.total_tx_size_);

    DelayHolder grid_checker(500);

    // char str_buffer[20];
    uint32_t loop_count       = 0;
    uint32_t PreviousWakeTime = xTaskGetTickCount();
    while (true) {
        LvglLock();

        extern std::atomic<float> kAcReference;
        lv_label_set_text_fmt(tf_kmod.GetMsgLabel(), "%f", kAcReference.load());

        if (loop_count % 5 == 0) {
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

            if (grid_checker.Exam(std::abs(kGridPll.d_) > 5.0f)) {
                tf_grid_status.SetMsg("连接");
            } else {
                tf_grid_status.SetMsg("断开");
            }
        }

        if (loop_count % 10 == 0) {
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
                                  adc_interrupt_meter2.MeasureFreq());

            // FastTim
            lv_label_set_text_fmt(tf_fast_tim.GetMsgLabel(),
                                  "%lu Hz\n%lu us",
                                  fast_tim_meter.MeasureFreq(), kFastTimCallbackDuration);

            // MainUart
            lv_label_set_text_fmt(tf_main_uart.GetMsgLabel(), "%lu", main_uart_meter.MeasureFreq());
        }

        LvglUnlock();
        loop_count++;
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
    kLvScreenConsole = lv_label_create(kConsoleTab);
    lv_obj_set_style_text_font(kLvScreenConsole, LvglTTF_GetSmallFont(), 0);
    lv_obj_set_width(kLvScreenConsole, lv_pct(100));

    switch (kWhoAmI) {
        case BoardSelector::A:
            lv_obj_set_style_text_color(kLvScreenConsole, kAColor, 0);
            ScreenConsole_AddText("I am A board!\n");
            break;
        case BoardSelector::B:
            lv_obj_set_style_text_color(kLvScreenConsole, kBColor, 0);
            ScreenConsole_AddText("I am B board!\n");
            break;
        default:
            lv_obj_set_style_text_color(kLvScreenConsole, lv_color_make(200, 0, 0), 0);
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
    lv_obj_scroll_to_view_recursive(kLvScreenConsole, LV_ANIM_OFF);

    LvglUnlock();

    xTaskCreate(ScreenConsole_Entry, "ScreenConsole", 1024 * 2, nullptr, PriorityBelowNormal, &kScreenConsoleHandle);
    xTaskCreate(MainPage_Thread, "main_page", 1024 * 2, nullptr, PriorityBelowNormal, nullptr);
}
