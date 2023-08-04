#pragma once

#include <cstdint>
#include "HighPrecisionTime/high_precision_time.h"
#include "main.h"

class DelayHolder
{
private:
    bool value;
    uint32_t delay_time_ms_;
    uint32_t last_time_;

public:
    DelayHolder(uint32_t delay_time_ms)
        : delay_time_ms_(delay_time_ms){};

    bool Exam(bool now_value)
    {
        if (now_value == value) {
            last_time_ = HAL_GetTick();
        } else if (HAL_GetTick() - last_time_ > delay_time_ms_) {
            value = now_value;
        }
        return value;
    }
};
