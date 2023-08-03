#include "user_app.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "thread_priority_def.h"
#include "ads1256/ads1256_device.hpp"
#include "freertos_io/os_printf.h"
#include "Vofa/just_float.hpp"
#include "Led//led_device.hpp"
#include <atomic>
#include "Filters/butterworth.hpp"
#include "HighPrecisionTime/stat.hpp"
#include "lv_app/main_page.hpp"
#include "lv_app/screeen_console.h"
#include "line_calis.hpp"
#include "WatchDog/watchdog.hpp"
#include "Relay/relay_device.hpp"

std::atomic<bool> kUserAppPrint = true;

static TaskHandle_t UserAppEntry_handle;

static void UserAppEntry(void *argument)
{
    (void)argument;

    KeyboardLed.Power(true);
    KeyboardLed.SetColor(0, 1, 0);

    while (true) {
        // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        vTaskDelay(1);

        if (kUserAppPrint) {
            // JFStream << volt << average << filter_result << butt_    duration << EndJFStream;
            JFStream << kIAdsFilterResult << kVAdsFilterResult << EndJFStream;
        }
    }
}

void StartUserApp()
{
    xTaskCreate(UserAppEntry, "user_app", 1024, nullptr, PriorityNormal, &UserAppEntry_handle);
}
