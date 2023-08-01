#pragma once

#include "ads1256.hpp"
#include "FreeRTOS.h"
#include "task.h"

void StartAdsWatchDog(Ads1256 *ads, TaskHandle_t *const pxCreatedTask);
