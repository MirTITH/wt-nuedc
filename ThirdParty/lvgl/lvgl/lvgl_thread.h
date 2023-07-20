#pragma once

#include "lvgl/lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void StartLvglThread();
void LvglLock();
void LvglUnlock();

extern lv_style_t Style_NormalFont;
extern lv_style_t Style_LargeFont;
extern lv_style_t Style_SmallFont;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_lock/freertos_lock.hpp"
#include <mutex>

extern freertos_lock::RecursiveMutex LvglMutex;
#define LvglLockGuard() std::lock_guard<freertos_lock::RecursiveMutex> LvglThreadLock(LvglMutex)

#endif
