/**
 * @file ads1256.cpp
 * @author X. Y.
 * @brief ADS1256 的驱动程序
 * @version 0.7
 * @date 2023-07-26
 *
 * @copyright Copyright (c) 2023
 *
 * 本驱动在 ADS1256 时钟频率为 7.68 MHz 下测试
 *
 * SPI 配置：
 *      Frame Format: Motorola
 *      Data Size: 8 Bits
 *      First Bit: MSB
 *
 *      Baud Rate： 3K ~ 1.92M（实测超一点也没关系，甚至能够缩短 mcu 中断执行时间）
 *      CPOL: Low
 *      CPHA: 2 Edge
 *
 *      CRC: Disabled
 *
 * SPI DMA:
 * 使能 SPI 的 DMA_TX DMA_RX
 *
 * ADS 的 IO 口必须浮空或者上下拉，不能直接与 VCC 或 GND 相连
 * 因为程序里给 IO 口配置为输出，并且通过读取 IO 口状态判断是否 reset 成功
 *
 * 保证 DRDY 的中断优先级低于 SPI DMA 的中断优先级
 *
 */

#pragma once

#include "main.h"
#include "spi.h"
#include <vector>
#include <stdint.h>
#include <atomic>
#include "atom_wrapper.hpp"
#include <limits>
#include <cassert>
#include "FreeRTOS.h"
#include "task.h"

class Ads1256
{
public:
    // Type defines
    // 可编程增益器
    enum class PGA {
        Gain1 = 0,
        Gain2,
        Gain4,
        Gain8,
        Gain16,
        Gain32,
        Gain64
    };

    // 转换速率 (SPS)
    enum class DataRate {
        SPS_2p5   = 0x03, // 2.5 SPS
        SPS_5     = 0x13,
        SPS_10    = 0x23,
        SPS_15    = 0x33,
        SPS_25    = 0x43,
        SPS_30    = 0x53,
        SPS_50    = 0x63,
        SPS_60    = 0x72,
        SPS_100   = 0x82,
        SPS_500   = 0x92,
        SPS_1000  = 0xA1,
        SPS_2000  = 0xB0,
        SPS_3750  = 0xC0,
        SPS_7500  = 0xD0,
        SPS_15000 = 0xE0,
        SPS_30000 = 0xF0
    };

    typedef struct {
        uint8_t STATUS;
        uint8_t MUX;
        uint8_t ADCON;
        uint8_t DRATE;
        uint8_t IO;
    } __attribute__((packed)) Registers_t;

    typedef struct
    {
        uint8_t mux;                // 输入通道选择。例如：0x23 表示正输入为 AIN2, 负输入 AIN3，以此类推。AINCOM 用 f 表示
        atom_wrapper<int32_t> data; // 转换为 int32_t 后的数据
    } ConvInfo_t;

    using ConvQueue_t = std::vector<ConvInfo_t>;

public: // 统计变量
    uint32_t dma_busy_count_{0};
    uint32_t drdy_count_{0};

public: // Public functions
    Ads1256(SPI_HandleTypeDef *hspi,
            GPIO_TypeDef *n_drdy_port, uint16_t n_drdy_pin,
            GPIO_TypeDef *n_reset_port = nullptr, uint16_t n_reset_pin = 0,
            GPIO_TypeDef *n_sync_port = nullptr, uint16_t n_sync_pin = 0,
            float vref = 2.5)
        : v_max_(2 * vref), hspi_(hspi),
          n_drdy_port_(n_drdy_port), n_drdy_pin_(n_drdy_pin),
          n_reset_port_(n_reset_port), n_reset_pin_(n_reset_pin),
          n_sync_port_(n_sync_port), n_sync_pin_(n_sync_pin){};

    /**
     * @brief 请在 DRDY 中断中调用此函数
     *
     */
    void DRDY_Callback();

    void SPI_TxRxCpltCallback()
    {
        conv_queue_.at(dma_transfer_index_).data = RawDataToInt32(dma_rx_buffer_);

        dma_transfer_index_ = 0xff;
        extern TaskHandle_t pll_task_handle;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        vTaskNotifyGiveFromISR(pll_task_handle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    void Init(DataRate data_rate = DataRate::SPS_3750, PGA gain = PGA::Gain1, bool input_buffer = false, bool auto_calibration = true);

    void Reset();

    bool IsDataReady() const
    {
        return !HAL_GPIO_ReadPin(n_drdy_port_, n_drdy_pin_);
    }

    bool WaitForDataReady(uint32_t timeout_us = std::numeric_limits<uint32_t>::max()) const;

    /**
     * @brief 直接从 ADS 读取数据
     * @note 如果启动了转换队列，请不要使用这个函数，否则会导致 SPI 读写冲突
     *
     * @return 转换成 int32_t 后的数据
     */
    int32_t ReadData();

    /**
     * @brief 将 int32_t 数据转换为电压 (V)
     *
     */
    float Data2Voltage(int32_t data) const
    {
        if (data > 0) {
            return (v_max_ / (Pow2(static_cast<int>(pga_)) * 0x7FFFFF)) * data;
        } else {
            return (v_max_ / (Pow2(static_cast<int>(pga_)) * 0x800000)) * data;
        }
    }

    /**
     * @brief 设置转换队列
     * @param muxs 输入通道选择。例如：0x23 表示正输入为 AIN2, 负输入 AIN3，以此类推。AINCOM 用 f 表示
     * @note 设置前转换队列必须是停止状态，之后记得启动转换队列
     */
    void SetConvQueue(const std::vector<uint8_t> &muxs);

    /**
     * @brief 启动转换队列
     *
     */
    void StartConvQueue();

    /**
     * @brief 停止转换队列
     *
     */
    void StopConvQueue();

    /**
     * @brief 获取转换队列状态
     *
     * @return true 已启动
     * @return false 未启动
     */
    bool GetConvQueueState() const
    {
        return use_conv_queue_;
    }

    std::vector<float> GetVoltage() const
    {
        auto size = conv_queue_.size();
        std::vector<float> result(size);

        for (size_t i = 0; i < size; i++) {
            result.at(i) = Data2Voltage(conv_queue_.at(i).data);
        }
        return result;
    }

    float GetVoltage(uint8_t index) const
    {
        return Data2Voltage(conv_queue_.at(index).data);
    }

    Registers_t ReadAllRegs()
    {
        assert(GetConvQueueState() == false); // 转换队列启动时不要读寄存器
        Registers_t result{};
        ReadReg(0x00, (uint8_t *)(&result), sizeof(result));
        return result;
    }

    void SetGain(PGA gain);
    PGA GetGain() const
    {
        return pga_;
    }
    PGA GetGainFromChip();

    void SetInputBufferAndAutoCalibration(bool input_buffer, bool auto_calibration);
    bool GetInputBufferStatus() const
    {
        return enable_input_buffer_;
    }
    bool GetAutoCalibrationStatus() const
    {
        return enable_auto_calibration_;
    }

    void SetDataRate(DataRate rate);
    DataRate GetDataRateFromChip();

    void SelfCalibrateOffsetGain();

    /**
     * @brief 检查 Reset 是否成功
     *
     */
    bool CheckForReset();

    /**
     * @brief 检查 ADS 是否成功连接
     *
     */
    bool CheckForPresent();

    /**
     * @brief 检查是否配置成功
     *
     */
    bool CheckForConfig();

    void EnterPowerDownMode();
    void ExitPowerDownMode();

    SPI_HandleTypeDef *GetSpiHandle() const
    {
        return hspi_;
    }

    constexpr int32_t RawDataToInt32(uint8_t data[3]) const
    {
        // 将读取的三段数据拼接
        int32_t result = (static_cast<uint32_t>(data[0]) << 16) |
                         (static_cast<uint32_t>(data[1]) << 8) |
                         static_cast<uint32_t>(data[2]);

        // 处理负数
        if (result & 0x800000) {
            result |= 0xFF000000;
        }
        return result;
    }

private: // ADS 配置
    PGA pga_;
    DataRate data_rate_;
    bool enable_auto_calibration_;
    bool enable_input_buffer_;

    const float v_max_; // 2 * vref

    // 初始化时将 ADS 的 GPIO Control Register 设为此值。
    // 当 ADS reset 成功后，这个寄存器会被恢复为默认值，读取此寄存器可以判断是否重置成功
    // 这也意味着 D0~D3 必须浮空或者上下拉，不能强接地
    constexpr static uint8_t kIO_REG_INIT_VALUE = 0x0A;

    // STATUS REGISTER 的 ID3, ID2, ID1, ID0 (Factory Programmed Identification Bits)
    // datasheet 中未给出，实测为 0x3
    constexpr static uint8_t kADS1256_ID = 0x3;

private: // 引脚配置
    SPI_HandleTypeDef *const hspi_;

    GPIO_TypeDef *const n_drdy_port_;
    const uint16_t n_drdy_pin_;

    GPIO_TypeDef *const n_reset_port_;
    const uint16_t n_reset_pin_;

    GPIO_TypeDef *const n_sync_port_;
    const uint16_t n_sync_pin_;

private: // 转换队列实现
    ConvQueue_t conv_queue_;
    std::atomic<bool> use_conv_queue_{false};
    std::atomic<size_t> conv_queue_index_{0};
    std::atomic<uint8_t> dma_transfer_index_{0xff}; // DMA 正在接收的 ConvQueue_t 序号，0xff 表示没有正在进行的 DMA 接收
    uint8_t dma_rx_buffer_[3];

    std::atomic<bool> is_in_rdatac_mode_{false}; // 是否处于连续读取模式

private: // 私有函数
    /**
     * @brief 从 SPI 读取
     *
     * @param rx_data
     * @param Size
     * @param Timeout
     */
    void SpiRead(uint8_t *rx_data, uint16_t Size, uint32_t Timeout = HAL_MAX_DELAY);
    bool SpiReadDma(uint8_t *rx_data, uint16_t Size);

    /**
     * @brief 从 SPI 写入
     *
     * @param tx_data
     * @param Size
     * @param Timeout
     */
    void SpiWrite(const uint8_t *tx_data, uint16_t Size, uint32_t Timeout = HAL_MAX_DELAY);

    /**
     * @brief 写入寄存器
     *
     * @param regaddr The address to the first register to be written.
     * @param databyte Data to be written to the registers.
     * @param size The number of bytes to be written. [1, 16]
     */
    void WriteReg(uint8_t regaddr, const uint8_t *databyte, uint8_t size);
    void WriteReg(uint8_t regaddr, uint8_t databyte);

    /**
     * @brief 写命令
     *
     * @note Do not call this function for WREG or RREG.
     */
    void WriteCmd(uint8_t cmd);

    /**
     * @brief 读取寄存器
     *
     * @param regaddr The address of the first register to read
     * @param buffer Read buffer
     * @param size The number of bytes to read [1, 16]
     */
    void ReadReg(uint8_t regaddr, uint8_t *databyte, uint8_t size);
    uint8_t ReadReg(uint8_t regaddr);

    /**
     * @brief 设置多路选择器
     *
     * @param mux 输入通道选择。例如：0x23 表示正输入为 AIN2, 负输入 AIN3，以此类推。AINCOM 用 f 表示
     */
    void SetMux(uint8_t mux);

    void ReadDataToQueueDma(uint8_t queue_index);
    void ReadDataContinousToQueueDma(uint8_t queue_index);

    void SyncWakeup();

    /**
     * @brief 进入连续读取模式
     *
     */
    void EnterReadDataContinousMode();

    /**
     * @brief 退出连续读取模式
     *
     */
    void ExitReadDataContinousMode();

    bool IsDmaCplt() const
    {
        if (dma_transfer_index_ == 0xff) {
            return true;
        }
        return false;
    }

    void WaitForDmaCplt()
    {
        while (dma_transfer_index_ != 0xff) {}
    }

    /**
     * @brief 将 ADS 自带的 D0 ~ D3 GPIO 设为 kIO_REG_INIT_VALUE
     * @note 重要：在 Reset 后调用此函数，因为本库用这个寄存器检测是否 Reset 成功
     */
    void InitAdsGpio();

    /**
     * @brief 2^y
     *
     */
    static constexpr int Pow2(int y)
    {
        return 1 << y;
    }
};
