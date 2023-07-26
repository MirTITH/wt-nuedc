/**
 * @file svpwm.hpp
 * @author X. Y.
 * @brief
 * @version 0.1
 * @date 2023-07-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <cmath>
#include <array>

namespace control_system
{

template <typename T>
class Sypwm
{
private:
    constexpr static T PI_    = M_PI;
    constexpr static T kGain_ = 1.0 / std::sqrt(3.0);

public:
    std::array<T, 3> duty_; // 占空比

    void Calc(T phase)
    {
        std::array<T, 3> volt;
        volt[0] = std::cos(phase);
        volt[1] = std::cos(phase + 2 * PI_ / 3);
        volt[2] = std::cos(phase + 4 * PI_ / 3);

        // 找最大值和最小值
        T min = volt[0];
        T max = volt[0];
        for (size_t i = 1; i < 3; i++) {
            if (volt[i] < min) {
                min = volt[i];
            }
            if (volt[i] > max) {
                max = volt[i];
            }
        }

        T e = (max + min) / 2; // 注入的谐波

        for (size_t i = 0; i < 3; i++) {
            duty_[i] = kGain_ * (volt[i] - e) + 0.5f;
        }
    }
};

} // namespace control_system
