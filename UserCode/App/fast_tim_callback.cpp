#include "fast_tim_callback.hpp"
#include <atomic>
#include "Vofa/just_float.hpp"
#include "HighPrecisionTime/stat.hpp"
#include "control_system/pll.hpp"
#include "Adc/adc_class_device.hpp"

uint32_t kFastTimCallbackDuration;
uint32_t kFastTimCallbackCount = 0;

void FastTimCallback()
{
    // 频率和时间统计
    TimeMeter time_meter(&kFastTimCallbackDuration);
    kFastTimCallbackCount++;

}
