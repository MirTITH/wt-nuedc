/**
 * @file ads1256.cpp
 * @author X. Y.
 * @brief ADS1256 的驱动程序
 * @version 0.1
 * @date 2023-07-08
 *
 * @copyright Copyright (c) 2023
 *
 * 本驱动在 ADS1256 时钟频率为 7.68 MHz 下测试
 *
 * SPI 频率范围： 3K ~ 1.92M
 *
 */

#include "ads1256.hpp"
#include "ads1256_cmd_define.h"
#include "HighPrecisionTime/high_precision_time.h"

static constexpr float kClkin = 7.68; // MHz, 晶振频率

// 延时数据 (page 6)
// > 50 tclkin, Delay from last SCLK edge for DIN to first SCLK rising edge for DOUT: RDATA, RDATAC, RREG Commands
static constexpr uint32_t kT6 = 100 / kClkin + 2;

// > 8 tclkin, nCS low after final SCLK falling edge
static constexpr uint32_t kT10 = 16 / kClkin + 2;

// Final SCLK falling edge of command to first SCLK rising edge of next command.
static constexpr uint32_t kT11_1 = 8 / kClkin + 2;  // > 4 tclkin, for RREG, WREG, RDATA
static constexpr uint32_t kT11_2 = 48 / kClkin + 2; // > 24 tclkin, for RDATAC, SYNC

void Ads1256::Init()
{
    // 等待电源稳定
    HPT_DelayMs(100);

    Reset();
    HPT_DelayMs(100);

    // /**
    //  * @brief  A/D Control Register
    //  * Clock Out OFF
    //  * Sensor Detect OFF (default)（可用来检测外部传感器是否完好）
    //  * Programmable Gain Amplifier Setting = 1
    //  */
    // WriteReg(ADS1256_ADCON, 0x00);

    // /**
    //  * @brief GPIO Control Register
    //  * 全部设为 Output
    //  * 引脚电平全部设为 low
    //  */
    // WriteReg(ADS1256_IO, 0x00);

    /**
     * @brief STATUS REGISTER
     * ORDER: Most Significant Bit First (default)
     * 自动校准: 开启
     * BUFEN: Buffer Enabled
     */
    WriteReg(ADS1256_STATUS, 0x04);

    /**
     * @brief A/D Data Rate
     * 默认值：30,000SPS
     */
    HPT_DelayMs(10);
    WaitForDataReady();
    SetDataRate(DataRate::SPS_100);

    // /**
    //  * @brief Input Multiplexer Control Register
    //  * Positive Input Channel: AIN0 (default)
    //  * Negative Input Channel: AINCOM
    //  */
    // WriteReg(ADS1256_MUX, 0x01);

    // WaitForDataReady();
    // WriteCmd(ADS1256_CMD_SELFCAL); // 自校准

    // WaitForDataReady();

    HPT_DelayMs(10);
}

void Ads1256::Reset()
{
    if (n_reset_port_ != nullptr) {
        HAL_GPIO_WritePin(n_reset_port_, n_reset_pin_, GPIO_PIN_RESET);
        HPT_DelayMs(10);
        HAL_GPIO_WritePin(n_reset_port_, n_reset_pin_, GPIO_PIN_SET);
        HPT_DelayMs(10);
    } else {
        WriteCmd(ADS1256_CMD_RESET);
        HPT_DelayMs(10);
    }
}

void Ads1256::WaitForDataReady()
{
    while (IsDataReady() == false) {};
}

int32_t Ads1256::ReadDataNoWait()
{
    WriteCmd(ADS1256_CMD_RDATA);
    HPT_DelayUs(kT6);

    uint8_t data[3];
    SpiRead(data, 3);

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

void Ads1256::SyncWakeup()
{
    WriteCmd(ADS1256_CMD_SYNC);
    HPT_DelayUs(kT11_2);
    WriteCmd(ADS1256_CMD_WAKEUP);
}

void Ads1256::DRDY_Callback()
{
    drdy_count_++;
    if (use_conv_queue_) {
        switch (conv_queue_.size()) {
            case 0:
                break;
            case 1:
                conv_queue_.at(0).value = ReadDataNoWait();
                break;

            default:
                auto now_index = conv_queue_index_;

                // 下一个通道
                conv_queue_index_++;
                if (conv_queue_index_ >= conv_queue_.size()) {
                    conv_queue_index_ = 0;
                }

                // 开始转换下一个通道
                SetMux(conv_queue_.at(conv_queue_index_).mux);

                // 上一个通道转换完成，读取它的值
                conv_queue_.at(now_index).value = ReadDataNoWait();
                break;
        }
    }
}

int32_t Ads1256::ReadData()
{
    WaitForDataReady();
    return ReadDataNoWait();
}

bool Ads1256::SpiRead(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if (HAL_SPI_Receive(hspi_, pData, Size, Timeout) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}

void Ads1256::SetConvQueue(const std::vector<uint8_t> &muxs)
{
    auto prev_use_conv_queue = use_conv_queue_;
    use_conv_queue_          = false;

    conv_queue_index_ = 0;

    auto size = muxs.size();
    conv_queue_.resize(size);
    for (size_t i = 0; i < size; i++) {
        conv_queue_.at(i).mux   = muxs.at(i);
        conv_queue_.at(i).value = 0;
    }

    if (prev_use_conv_queue) {
        StartConvQueue();
    }
}

void Ads1256::StartConvQueue()
{
    if (conv_queue_.size() != 0) {
        SetMux(conv_queue_.at(0).mux);
    }

    use_conv_queue_ = true;
}

void Ads1256::SetMux(uint8_t mux)
{
    WriteReg(ADS1256_MUX, mux);
    SyncWakeup();
}

bool Ads1256::SpiWrite(const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if (HAL_SPI_Transmit(hspi_, (uint8_t *)pData, Size, Timeout) == HAL_OK) {
        return true;
    } else {
        return false;
    }
}

void Ads1256::WriteReg(uint8_t regaddr, const uint8_t *databyte, uint8_t size)
{
    if (size == 0) {
        return;
    }

    // Datasheet page 36
    // Write command
    uint8_t temp[2];
    temp[0] = ADS1256_CMD_WREG | (regaddr & 0x0F);
    temp[1] = size - 1;

    // WaitForDataReady();
    SpiWrite(temp, sizeof(temp));

    // Write reg
    SpiWrite(databyte, size);
    HPT_DelayUs(kT11_1);
}

void Ads1256::WriteReg(uint8_t regaddr, uint8_t databyte)
{
    WriteReg(regaddr, &databyte, 1);
}

void Ads1256::WriteCmd(uint8_t cmd)
{
    SpiWrite(&cmd, 1);
}

void Ads1256::ReadReg(uint8_t regaddr, uint8_t *databyte, uint8_t size)
{
    if (size == 0) {
        return;
    }

    // Datasheet page 36
    // Write command
    uint8_t temp[2];
    temp[0] = ADS1256_CMD_RREG | (regaddr & 0x0F);
    temp[1] = size - 1;
    SpiWrite(temp, sizeof(temp));

    HPT_DelayUs(kT6);

    // Read reg
    SpiRead(databyte, size);

    HPT_DelayUs(kT11_1);
}

void Ads1256::SetGain(PGA gain)
{
    // 实际上 ADCON 的 bit[2-0] 控制增益，其他 bit 控制时钟输出和传感器检测
    // 这里将时钟输出和传感器检测都关闭
    WriteReg(ADS1256_ADCON, (uint8_t)gain);
}

void Ads1256::SetDataRate(DataRate rate)
{
    WriteReg(ADS1256_DRATE, (uint8_t)rate);
}

uint8_t Ads1256::ReadReg(uint8_t regaddr)
{
    uint8_t result = 0;
    ReadReg(regaddr, &result, 1);
    return result;
}
