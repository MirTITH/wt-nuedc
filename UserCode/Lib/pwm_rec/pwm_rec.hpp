/**
 * @file pwm_rec.hpp
 * @author X. Y.
 * @brief
 * @version 0.1
 * @date 2023-07-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "control_system/z_tf.hpp"
#include "control_system/pid_controller.hpp"
#include "arm_math.h"
#include "control_system/spwm.hpp"

template <typename T>
class PwmRec
{
public:
    T in_Udcref_; // 直流侧期望电压
    T in_Udc_;    // 直流侧反馈电压
    T in_phase_;  // 相角 (rad)

public:
    T Ts_;
    control_system::pid::PIController<T> pi_controller;

public:
    PwmRec(T Ts)
        : Ts_(Ts), pi_controller(1, 10, Ts)
    {
    }
};
