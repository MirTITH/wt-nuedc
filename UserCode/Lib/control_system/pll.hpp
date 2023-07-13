#pragma once
#include "discrete_controller_base.hpp"
#include "z_tf.hpp"
#include "pid_controller.hpp"
#include <cmath>

namespace control_system
{
template <typename T>
class Pll : public DiscreteControllerBase<T>
{
private:
    constexpr static T PI_ = M_PI;
    pid::PIController<T, DiscreteIntegrator<T>> pi_controller;
    DiscreteIntegrator<T> integrator;
    T wc_; // 初始角频率

    // 改进III型二阶广义积分器
    ZTf<T> Gq, Gd;
    std::vector<T> Gd_num, Gd_den;
    std::vector<T> Gq_num, Gq_den;
    const T Ts;

    void UpdateGqGd(T k, T w)
    {
        T ts_w      = Ts * w;
        T ts2_w2    = ts_w * ts_w;
        T ts3_w3    = ts2_w2 * ts_w;
        T _2_k_ts_w = 2 * k * ts_w;
        T _2_ts2_w2 = 2 * ts2_w2;

        Gd_num.at(0) = _2_k_ts_w;
        Gd_num.at(1) = 0;
        Gd_num.at(2) = -_2_k_ts_w;

        Gd_den.at(0) = 4 + _2_k_ts_w + ts2_w2;
        Gd_den.at(1) = -8 + _2_ts2_w2;
        Gd_den.at(2) = 4 - _2_k_ts_w + ts2_w2;

        T _4_k_ts_w   = 2 * _2_k_ts_w;
        T _2_k_ts2_w2 = k * _2_ts2_w2;
        T _4_ts_w     = 4 * ts_w;

        Gq_num.at(0) = -_4_k_ts_w + _2_k_ts2_w2;
        Gq_num.at(1) = _4_k_ts_w + _2_k_ts2_w2;
        Gq_num.at(2) = _4_k_ts_w - _2_k_ts2_w2;
        Gq_num.at(3) = -_4_k_ts_w - _2_k_ts2_w2;

        Gq_den.at(0) = 8 + _4_ts_w + _4_k_ts_w + _2_ts2_w2 + _2_k_ts2_w2 + ts3_w3;
        Gq_den.at(1) = -24 - _4_ts_w - _4_k_ts_w + _2_ts2_w2 + _2_k_ts2_w2 + 3 * ts3_w3;
        Gq_den.at(2) = 24 - _4_ts_w - _4_k_ts_w - _2_ts2_w2 - _2_k_ts2_w2 + 3 * ts3_w3;
        Gq_den.at(3) = -8 + _4_ts_w + _4_k_ts_w - _2_ts2_w2 - _2_k_ts2_w2 + ts3_w3;

        Gq.Init(Gq_num, Gq_den);
        Gd.Init(Gd_num, Gd_den);
    }

public:
    T omega_; // 角速度 (rad/s)
    T phase_; // 相位 (rad)，范围 [0, 2*PI). 注：相位值为输入信号的矢量与 x 轴正方向的夹角（即余弦信号的相角）
    T d_, q_;

    Pll(T Ts, T wc = 2 * PI_ * 50, T k = 2, T Kp = 100, T Ki = 2000)
        : pi_controller(Kp, Ki, Ts),
          integrator(1, Ts),
          wc_(wc), Ts(Ts)
    {
        Gd_num.resize(3);
        Gd_den.resize(3);
        Gq_num.resize(4);
        Gq_den.resize(4);
        UpdateGqGd(k, wc);
        // // 改进III型二阶广义积分器
        // // 在 50 hz 处相位 -90°
        // Gq.Init({-0.0554645753435237, 0.0590625511353515, 0.0554645753435237, -0.0590625511353515},
        //         {1, -2.81724583229147, 2.64562469318789, -0.828152793410110});

        // // 在 50 hz 处相位 0°
        // Gd.Init({0.0590625511353515, 0, -0.0590625511353515},
        //         {1, -1.87816388819432, 0.881874897729297});

        ResetState();
    }

    /**
     * @brief 走一个周期
     *
     * @param input 输入
     * @return  输出
     */
    T Step(T input) override
    {
        auto alpha = Gd.Step(input);
        auto beta  = Gq.Step(input);

        auto integrate_state = integrator.GetStateOutput(); // 积分器的内部状态，作为下一次积分的预估值
        T sin_value          = sin(integrate_state);
        T cos_value          = cos(integrate_state);

        // park
        d_ = alpha * cos_value + beta * sin_value;
        q_ = -alpha * sin_value + beta * cos_value;

        omega_ = pi_controller.Step(q_) + wc_;

        auto integrate_result = integrator.Step(omega_);

        // 循环清零积分值，防止积分累计误差
        if (integrate_result > 2 * PI_) {
            phase_ = integrate_result - 2 * PI_;
            integrator.SetStateValue(integrator.GetStateOutput() - 2 * PI_);
        } else {
            phase_ = integrate_result;
        }

        return omega_;
    }

    /**
     * @brief 重置控制器内部状态
     *
     */
    void ResetState() override
    {
        Gq.ResetState();
        Gd.ResetState();
        pi_controller.ResetState();
        integrator.ResetState();
        phase_ = 0;
    }
};

} // namespace control_system
