#pragma once

#include "high_precision_time.h"
#include <cstdint>

class TimeMeter
{
private:
    uint32_t *duration_;

public:
    uint32_t start_time_ = 0;

    /**
     * @brief 创建时是否开始测量
     */
    TimeMeter(uint32_t *duration, bool start_measure = true)
        : duration_(duration)
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

    ~TimeMeter()
    {
        *duration_ = HPT_GetUs() - start_time_;
    }
};
