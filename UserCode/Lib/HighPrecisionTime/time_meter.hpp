#pragma once

#include "high_precision_time.h"
#include <cstdint>

class TimeMeter
{
public:
    uint32_t start_time_ = 0;

    /**
     * @brief 创建时是否开始测量
     */
    TimeMeter(bool start_measure = true)
    {
        if (start_measure) {
            start_time_ = HPT_GetUs();
        }
    }

    /**
     * @brief 开始测量
     * @note 这会重新开始记录时间
     *
     */
    void StartMeasure()
    {
        start_time_ = HPT_GetUs();
    }

    /**
     * @brief 获取上次开始测量到当前时刻的时间间隔
     *
     * @return uint32_t 微秒
     */
    uint32_t GetDuration() const
    {
        return HPT_GetUs() - start_time_;
    }
};
