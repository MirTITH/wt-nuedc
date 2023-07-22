#pragma once
#include "main.h"
#include "spi.h"
#include <vector>
#include <stdint.h>

class Ads1256
{
public:
    enum class PGA {
        Gain1 = 0,
        Gain2,
        Gain4,
        Gain8,
        Gain16,
        Gain32,
        Gain64
    };

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
        uint8_t mux; // 输入通道选择。例如：0x23 表示正输入为 AIN2, 负输入 AIN3，以此类推。AINCOM 用 8 表示
        int32_t value;
    } ConvInfo_t;

    using ConvQueue_t = std::vector<ConvInfo_t>;

    uint32_t drdy_count_ = 0;
    ConvQueue_t conv_queue_;

public:
    Ads1256(SPI_HandleTypeDef *hspi,
            GPIO_TypeDef *n_drdy_port, uint16_t n_drdy_pin,
            GPIO_TypeDef *n_reset_port = nullptr, uint16_t n_reset_pin = 0,
            float vref = 2.5)
        : hspi_(hspi),
          n_drdy_port_(n_drdy_port), n_drdy_pin_(n_drdy_pin),
          n_reset_port_(n_reset_port), n_reset_pin_(n_reset_pin),
          v_max_(2 * vref){};

    /**
     * @brief 请在 DRDY 中断中调用此函数
     *
     */
    void DRDY_Callback();

    void Init();

    void Reset();

    bool IsDataReady()
    {
        return !HAL_GPIO_ReadPin(n_drdy_port_, n_drdy_pin_);
    }

    void WaitForDataReady();

    int32_t ReadData();

    /**
     * @brief 将 原始数据转换为电压 (V)
     *
     */
    float Data2Voltage(int32_t data)
    {
        if (data > 0) {
            return v_max_ * ReadData() / 0x7FFFFF;
        } else {
            return v_max_ * ReadData() / 0x800000;
        }
    }

    /**
     * @brief 设置转换队列
     * @note 之后记得启动转换队列
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
    void StopConvQueue()
    {
        use_conv_queue_ = false;
    }

    /**
     * @brief 获取转换队列状态
     *
     * @return true 已启动
     * @return false 未启动
     */
    bool GetConvQueueState()
    {
        return use_conv_queue_;
    }

    /**
     * @brief 设置多路选择器
     *
     * @param mux 输入通道选择。例如：0x23 表示正输入为 AIN2, 负输入 AIN3，以此类推。AINCOM 用 8 表示
     */
    void SetMux(uint8_t mux);

    Registers_t ReadAllRegs()
    {
        Registers_t result{};
        ReadReg(0x00, (uint8_t *)(&result), sizeof(result));
        return result;
    }

    void SetGain(PGA gain);
    void SetDataRate(DataRate rate);

private:
    SPI_HandleTypeDef *hspi_;

    GPIO_TypeDef *n_drdy_port_;
    uint16_t n_drdy_pin_;

    GPIO_TypeDef *n_reset_port_;
    uint16_t n_reset_pin_;

    float v_max_; // 2 * vref

    volatile bool use_conv_queue_ = false;
    size_t conv_queue_index_      = 0;

    /**
     * @brief 从 SPI 读取
     *
     * @param pData
     * @param Size
     * @param Timeout
     * @return true 读取成功
     * @return false 读取失败
     */
    bool SpiRead(uint8_t *pData, uint16_t Size, uint32_t Timeout = HAL_MAX_DELAY);

    /**
     * @brief 从 SPI 写入
     *
     * @param pData
     * @param Size
     * @param Timeout
     * @return true 写入成功
     * @return false 写入失败
     */
    bool SpiWrite(const uint8_t *pData, uint16_t Size, uint32_t Timeout = HAL_MAX_DELAY);

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

    int32_t ReadDataNoWait();

    void SyncWakeup();
};
