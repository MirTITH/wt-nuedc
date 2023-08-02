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
#include "control_system/spwm.hpp"
#include "unipolar_spwm.hpp"

template <typename T>
class PwmRec
{
public:
    T in_udcref_ = 0; // 直流侧期望电压

    T in_udc_   = 0; // 直流侧反馈电压
    T in_us_    = 0; // 交流侧反馈电压
    T in_is_    = 0; // 交流侧反馈电流
    T in_phase_ = 0; // 相角 (rad)

    control_system::pid::PIController<T> pi_controller;
    control_system::ZTf<T> c_controller; // 比例谐振滤波器

private:
    T Ts;
    UnipolarSpwm spwm_;

    std::vector<T> c_controller_num_, c_controller_den_;

public:
    PwmRec(T Ts, std::array<UnipolarSpwm::SpwmChannel_t, 2> tim_channel, T Kp = 1, T Ki = 10)
        : pi_controller(Kp, Ki, Ts), Ts(Ts),
          spwm_(tim_channel){};

    void StartPwm()
    {
        spwm_.StartPwm();
    }

    void StopPwm()
    {
        spwm_.StopPwm();
    }

    /**
     * @brief 初始化比例谐振滤波器
     *
     * @param Kpc 在非谐振处增益大小
     * @param Kr 在谐振处增益
     * @param Wc 谐振带宽
     * @param W0 谐振峰值频率
     */
    void Init(T Kpc = 1, T Kr = 100, T Wc = 10, T W0 = 2 * 50 * M_PI)
    {
        c_controller_num_.resize(3);
        c_controller_den_.resize(3);

        auto W0_2_Ts_2 = W0 * W0 * Ts * Ts;

        c_controller_num_.at(0) = (4 + 4 * Ts * Wc + W0_2_Ts_2) * Kpc + 4 * Kr * Ts * Wc;
        c_controller_num_.at(1) = (2 * W0_2_Ts_2 - 8) * Kpc;
        c_controller_num_.at(2) = (4 - 4 * Ts * Wc + W0_2_Ts_2) * Kpc - 4 * Kr * Ts * Wc;

        c_controller_den_.at(0) = 4 + 4 * Ts * Wc + W0_2_Ts_2;
        c_controller_den_.at(1) = 2 * W0_2_Ts_2 - 8;
        c_controller_den_.at(2) = 4 - 4 * Ts * Wc + W0_2_Ts_2;

        c_controller.Init(c_controller_num_, c_controller_den_);
    }

    auto Step()
    {
        auto Is_star  = pi_controller.Step(in_udcref_ - in_udc_);
        auto is_star  = Is_star * std::cos(in_phase_);
        auto uL_star  = c_controller.Step(is_star - in_is_);
        auto uab_star = in_us_ - uL_star;
        auto duty     = (uab_star / in_udcref_ + 1) / 2;
        spwm_.SetDuty(duty);
        return duty;
    }
};
