#include "ads_watchdog.hpp"
#include "thread_priority_def.h"

void AdsWatchDogEntry(Ads1256 *ads)
{
    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (ads->is_in_rdatac_mode_ == false && ads->use_conv_queue_ == true) {
            ads->use_conv_queue_ = false;
            ads->WaitForDmaCplt();
            if (ads->CheckForConfig() == false) {
                ads->ads_err_count_++;
                if (ads->kAllowReInit) {
                    ads->ReInit();
                }
            }
            ads->use_conv_queue_ = true;
        }
    }
}

void StartAdsWatchDog(Ads1256 *ads, TaskHandle_t *pxCreatedTask)
{
    xTaskCreate((TaskFunction_t)AdsWatchDogEntry, "AdsWatchDog", 256, (void *)ads, PriorityLow, pxCreatedTask);
}
