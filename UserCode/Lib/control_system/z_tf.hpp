/**
 * @file z_tf.hpp
 * @author X. Y.
 * @brief Z 传递函数
 * @version 0.2
 * @date 2023-06-26
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "discrete_controller_base.hpp"
#include <vector>
#include <cassert>

namespace control_system
{

/**
 * @brief Z传递函数
 *
 * @tparam T 数据类型，例如 float 或 double
 */
template <typename T>
class ZTf : public DiscreteControllerBase<T>
{
private:
    std::vector<T> input_c_, output_c_; // 输入系数 i1, i2, ... 和输出系数 o1, o2, ...
    T input_c0_;                        // 输入系数 i0

    std::vector<T> inputs_, outputs_; // 输入和输出队列
    size_t index_ = 0;                // 上一次输入和输出在队列中的位置

    size_t order_ = 0; // 系统维数（等于分母维数）
    size_t order_m1_;  // 系统维数 - 1（提前算好，加快运算速度）

public:
    /**
     * @brief 创建空的 Z 传函
     * @note 由于没有分子和分母，之后必须调用 Init() 指定分子和分母才能调用 Step()
     */
    ZTf(){};

    /**
     * @brief 创建一个 Z 传函
     *
     * @param num 分子
     * @param den 分母
     * @note 分子阶数不能大于分母，否则是非因果系统
     */
    ZTf(const std::vector<T> &num, const std::vector<T> &den)
    {
        Init(num, den);
    }

    /**
     * @brief 初始化 Z 传函或重新指定 Z 传函的表达式
     *
     * @param num 分子
     * @param den 分母
     * @note 分子阶数不能大于分母，否则是非因果系统
     */
    void Init(const std::vector<T> &num, const std::vector<T> &den)
    {
        int size_diff = den.size() - num.size(); // 分母维数与分子维数之差

        assert(size_diff >= 0); // 分子阶数不能大于分母，否则是非因果系统

        order_    = den.size();
        order_m1_ = order_ - 1;

        input_c_.resize(order_m1_);
        output_c_.resize(order_m1_);

        assert(den.at(0) != 0);

        if (size_diff == 0) {
            // 输入系数 i0
            input_c0_ = num.at(0) / den.at(0);

            // 其他输入系数 i1, i2, ...
            for (size_t i = 0; i < order_m1_; i++) {
                input_c_.at(i) = num.at(i + 1) / den.at(0);
            }
        } else {
            // 如果分子阶数小于分母，i0 一定为 0
            input_c0_ = 0;

            // 其他输入系数 i1, i2, ...
            // 如果分子阶数小于分母，就往前面补一些 0
            for (int i = 0; i < size_diff - 1; i++) {
                input_c_.at(i) = 0;
            }

            // 剩下的输入系数
            for (size_t i = size_diff - 1; i < order_m1_; i++) {
                input_c_.at(i) = num.at(i + 1 - size_diff) / den.at(0);
            }
        }

        // 输出系数
        for (size_t i = 0; i < order_m1_; i++) {
            output_c_.at(i) = -den.at(i + 1) / den.at(0);
        }

        inputs_.resize(order_);
        outputs_.resize(order_);
        ResetState();
    }

    /**
     * @brief 走一个周期
     *
     * @param input 输入
     * @return 输出
     */
    T Step(T input) override
    {
        assert(order_ != 0);

        T output = input_c0_ * input;
        for (size_t i = 0; i < order_m1_; i++) {
            output += input_c_[i] * inputs_[index_];
            output += output_c_[i] * outputs_[index_];
            if (index_ < order_m1_) {
                index_++;
            } else {
                index_ = 0;
            }
        }

        inputs_[index_]  = input;
        outputs_[index_] = output;

        return output;
    }

    /**
     * @brief 重置内部状态
     *
     */
    void ResetState() override
    {
        std::fill(inputs_.begin(), inputs_.end(), 0);
        std::fill(outputs_.begin(), outputs_.end(), 0);
        index_ = 0;
    }
};

} // namespace control_system
