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
    ZTf<T> Gq, Gd; // 改进III型二阶广义积分器
    pid::PIController<T, DiscreteIntegrator<T>> pi_controller;
    T wc_ = 2 * PI_ * 50; // 初始角频率
    DiscreteIntegrator<T> integrator;

public:
    T omega_, theta_;
    T sin_value, cos_value;

    float d_, q_;

    Pll()
        : pi_controller(100, 2000, 1.0 / 5000),
          integrator(1, 1.0 / 5000)
    {
        // 在 50 hz 处相位 -90°
        Gq.Init({-0.0554645753435237, 0.0590625511353515, 0.0554645753435237, -0.0590625511353515},
                {1, -2.81724583229147, 2.64562469318789, -0.828152793410110});

        // 在 50 hz 处相位 0°
        Gd.Init({0.0590625511353515, 0, -0.0590625511353515},
                {1, -1.87816388819432, 0.881874897729297});

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
        sin_value            = sin(integrate_state);
        cos_value            = cos(integrate_state);

        // park
        d_ = alpha * cos_value + beta * sin_value;
        q_ = -alpha * sin_value + beta * cos_value;

        omega_ = pi_controller.Step(q_) + wc_;
        // omega_ = wc_;

        auto integrate_result = integrator.Step(omega_);

        // 循环清零积分值，防止积分累计误差
        if (integrate_result > 2 * PI_) {
            theta_ = integrate_result - 2 * PI_;
            integrator.SetInitialValue(integrator.GetStateOutput() - 2 * PI_);
        } else {
            theta_ = integrate_result;
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
        theta_ = 0;
    }
};

} // namespace control_system
