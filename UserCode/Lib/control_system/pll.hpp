#pragma once
#include "discrete_controller_base.hpp"
#include "z_tf.hpp"
#include "pid_controller.hpp"
#include "arm_math.h"

namespace control_system
{
template <typename T>
class Pll : public DiscreteControllerBase<T>
{
private:
    ZTf<T> Gq, Gd; // 改进III型二阶广义积分器
    pid::PIController<float, DiscreteIntegrator<float>> pi_controller;

public:
    T sin_value_, cos_value_;

    float d_, q_;

    Pll()
    {
        // 在 50 hz 处相位 -90°
        Gq.Init({-0.0555, 0.0591, 0.0555, -0.0591},
                {1, -2.8172, 2.6456, -0.8282});

        // 在 50 hz 处相位 0°
        Gd.Init({0.0591, 0, -0.0591},
                {1, -1.8782, 0.8819});

                
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

        arm_park_f32(alpha, beta, &d_, &q_, sin_value_, cos_value_);
    }

    /**
     * @brief 重置控制器内部状态
     *
     */
    void ResetState() override
    {
        Gq.ResetState();
        Gd.ResetState();
    }
};

} // namespace control_system
