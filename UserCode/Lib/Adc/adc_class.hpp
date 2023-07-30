/**
 * @file adc_class.hpp
 * @author X. Y.
 * @brief
 * @version 0.3
 * @date 2023-07-09
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include "adc.h"
#include <vector>
#include <cassert>

/**
 * ADC 需要在 CubeMX 里先配置好，以下参数需要配置：（其他随意）
 * 单端
 * Continuous Conversion Mode: Enable
 * End Of Conversion Selection: End of sequence of conversion
 * Overrun behaviour: Overrun data overwritten
 * Conversion Data Management Mode: DMA Circular Mode（需要先在 DMA Settings 里添加 DMA 后才能选择该项）
 *
 * DMA Setting 配置：
 * Circular, Half Word
 * 推荐：Use Fifo, Threshold 根据 ADC 的 Number of Conversion 选择，使得 Fifo 中的大小刚好是一个转换序列的大小。
 *   如果 Resolution 是 12 bit，则数据为 uint16_t （因为 uint8_t 放不下，但又没有 uint12_t）
 *   DMA Fifo 总容量为 4 Words (即 16 Bytes, 8 个 uint16_t), 因此 Half Full = 4 个 uint16_t
 */

class Adc
{
private:
    using adc_data_t = uint16_t;

    bool is_inited_ = false;
    const float vref_;
    uint8_t number_of_conversion_ = 0;
    std::vector<adc_data_t> adc_data_;
    uint16_t max_range_;

    uint16_t CalcMaxRange() const;

    void InvalidateDCache() const
    {
        // SCB_InvalidateDCache_by_Addr(adc_data_, sizeof(*adc_data_) * number_of_conversion_);
    }

public:
    ADC_HandleTypeDef *hadc_;
    uint32_t conv_cplt_count = 0; // 转换完成的次数

    /**
     *
     * @param hadc
     * @param vref 参考电压 (V)
     */
    Adc(ADC_HandleTypeDef *hadc, float vref = 3.3)
        : vref_(vref), hadc_(hadc){};

    void ConvCpltCallback()
    {
        conv_cplt_count++;
    }

    /**
     * @brief 初始化 ADC
     * @note 该函数内部会调用一次 Calibrate() 来校准 ADC
     */
    void Init();

    /**
     * @brief 校准 ADC
     *
     */
    void Calibrate()
    {
        // 校准 ADC （不同 stm32 型号该函数可能不一样或不存在，注意修改）
        // HAL_ADCEx_Calibration_Start(hadc_, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED); // For STM32H7
    }

    void StartDma()
    {
        assert(is_inited_); // 需要先初始化才能调用该函数
        HAL_ADC_Start_DMA(hadc_, (uint32_t *)adc_data_.data(), number_of_conversion_);
    }

    void StopDma()
    {
        HAL_ADC_Stop_DMA(hadc_);
    }

    /**
     * @brief 获取 ADC 读数取值范围的最大值
     */
    uint16_t GetMaxRange() const
    {
        return max_range_;
    }

    /**
     * @brief 获取 ADC 原始数据
     *
     * @param index [0, NbrOfConversion - 1]
     */
    adc_data_t GetData(size_t index) const
    {
        InvalidateDCache();
        return adc_data_.at(index);
    }

    std::vector<adc_data_t> GetAllData() const
    {
        InvalidateDCache();
        return adc_data_;
    }

    /**
     * @brief 获取归一化数据
     *
     * @param index 范围：[0, NbrOfConversion - 1]
     * @return float 归一化数据，范围：[0, 1]
     */
    float GetNormalizedData(size_t index) const
    {
        return (float)GetData(index) / max_range_;
    }

    std::vector<float> GetAllNormalizedData() const;

    /**
     * @brief Get voltage (V)
     *
     * @param index [0, NbrOfConversion - 1]
     * @return float Voltage
     */
    float GetVoltage(size_t index) const
    {
        return (float)GetData(index) / max_range_ * vref_;
    }

    std::vector<float> GetAllVoltage() const;

    /**
     * @brief 获取参考电压 (V)
     *
     */
    float GetVref() const
    {
        return vref_;
    }

    /**
     * @brief 计算内部温度传感器的温度（ADC3 有一个 Temperature Sensor Channel，使用这个通道可以读取芯片温度）
     * @param temperature_sensor_index Temperature Sensor Channel 对应的 index
     *
     * @return 温度（摄氏度）
     */
    float GetTemperature(size_t temperature_sensor_index);
};
