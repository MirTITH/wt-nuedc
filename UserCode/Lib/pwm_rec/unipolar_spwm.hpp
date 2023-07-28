/**
 * @file unipolar_spwm.hpp
 * @author X. Y.
 * @brief
 * @version 0.1
 * @date 2023-07-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once
#include "main.h"
#include <array>
#include "arm_math.h"

/**
 * @brief 单极性 SPWM
 *
 */
class UnipolarSpwm
{
    typedef struct
    {
        TIM_HandleTypeDef *htim;
        uint32_t channel;
    } SpwmChannel_t;

private:
    std::array<SpwmChannel_t, 2> spwm_channels_;
    void SetChannelDuty(const SpwmChannel_t &channel, float duty)
    {
        duty = (duty < 0) ? 0 : duty;
        duty = (duty > 1) ? 1 : duty;

        auto max_compare = __HAL_TIM_GET_AUTORELOAD(channel.htim) + 1;
        uint32_t compare = std::lround(max_compare * duty);
        __HAL_TIM_SET_COMPARE(channel.htim, channel.channel, compare);
    }

public:
    UnipolarSpwm(const std::array<SpwmChannel_t, 2> &spwm_channels)
        : spwm_channels_(spwm_channels){};

    void StartPwm()
    {
        for (auto &var : spwm_channels_) {
            HAL_TIM_PWM_Start(var.htim, var.channel);
        }
    }

    void StopPwm()
    {
        for (auto &var : spwm_channels_) {
            HAL_TIM_PWM_Stop(var.htim, var.channel);
        }
    }

    void SetDuty(float duty)
    {
        SetChannelDuty(spwm_channels_[0], duty);
        SetChannelDuty(spwm_channels_[1], 1 - duty);
    }
};
