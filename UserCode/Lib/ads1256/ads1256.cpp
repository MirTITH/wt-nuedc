/**
 * @file ads1256.cpp
 * @author X. Y.
 * @brief ADS1256 的驱动程序
 * @version 0.3
 * @date 2023-07-23
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
#include "FreeRTOS.h"
#include "task.h"

static constexpr float kClkin = 7.68; // MHz, 晶振频率

// 延时数据 (page 6)
// > 50 tclkin, Delay from last SCLK edge for DIN to first SCLK rising edge for DOUT: RDATA, RDATAC, RREG Commands
static constexpr uint32_t kT6 = 50 / kClkin + 2;

// > 8 tclkin, nCS low after final SCLK falling edge
static constexpr uint32_t kT10 = 8 / kClkin + 2;

// Final SCLK falling edge of command to first SCLK rising edge of next command.
static constexpr uint32_t kT11_1 = 4 / kClkin + 2;  // > 4 tclkin, for RREG, WREG, RDATA
static constexpr uint32_t kT11_2 = 24 / kClkin + 2; // > 24 tclkin, for RDATAC, SYNC

void Ads1256::Init()
{
    // 等待电源稳定
    HPT_DelayMs(100);

    Reset();

    /**
     * @brief  A/D Control Register
     * default value: 0x20
     * Clock Out: OFF (default: ON)
     * Sensor Detect OFF (default: OFF)（用来检测外部传感器）
     * Programmable Gain Amplifier Setting = 1 (default: 1)
     */
    // WriteReg(ADS1256_ADCON, 0x00);
    SetGain(PGA::Gain1);

    /**
     * @brief GPIO Control Register
     * default value: 0xE0
     * 全部设为输出 (default: 全部设为输入)
     * 引脚电平全部设为低 (default: 全部设为低)
     */
    // WriteReg(ADS1256_IO, 0x00);

    /**
     * @brief STATUS REGISTER
     * default value: 0xX1 (X 为 ID，未知)
     * ORDER: Most Significant Bit First (default: MSB)
     * 自动校准: 开启 (default: OFF)
     * BUFEN: default: Disable
     */
    WriteReg(ADS1256_STATUS, 0x04); // Buffer disable
    // WriteReg(ADS1256_STATUS, 0x06); // Buffer enable

    /**
     * @brief A/D Data Rate
     * 默认值：30,000SPS
     */
    // SetDataRate(DataRate::SPS_100);

    /**
     * @brief Input Multiplexer Control Register
     * Positive Input Channel: AIN0 (default)
     * Negative Input Channel: AINCOM
     */
    // WriteReg(ADS1256_MUX, 0x01);

    // 自校准
    SelfCalibrateOffsetGain();
    HPT_DelayMs(100);
}

void Ads1256::Reset()
{
    if (n_reset_port_ != nullptr) {
        HAL_GPIO_WritePin(n_reset_port_, n_reset_pin_, GPIO_PIN_RESET);
        HPT_DelayMs(1);
        HAL_GPIO_WritePin(n_reset_port_, n_reset_pin_, GPIO_PIN_SET);
        HPT_DelayMs(1);
        WaitForDataReady();
    } else {
        // 先发送几个 WAKEUP 命令，防止 RESET 命令不能正确送达
        // 原因之一：
        // ADS 读写指令需要多个 spi 字节周期，如果上一时刻 ADS 正在进行读写指令，
        // 则此时发送 RESET 可能被 ADS 认为是整个读写过程的一部分，从而不能 RESET
        for (size_t i = 0; i < 5; i++) {
            WakeUp();
        }

        WriteCmd(ADS1256_CMD_RESET);
        HPT_DelayMs(1);
        WaitForDataReady();
    }
}

bool Ads1256::WaitForDataReady(uint32_t timeout_us) const
{
    if (timeout_us == std::numeric_limits<uint32_t>::max()) {
        while (IsDataReady() == false) {};
    } else {
        uint32_t startUs = HPT_GetUs();
        while (IsDataReady() == false) {
            if (HPT_GetUs() - startUs >= timeout_us) return false;
        }
    }
    return true;
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
                auto now_index = conv_queue_index_.load();

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
    auto prev_use_conv_queue = GetConvQueueState();
    StopConvQueue();

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

    SpiWrite(temp, sizeof(temp));
    SpiWrite(databyte, size);

    HPT_DelayUs(kT11_1);
}

void Ads1256::WriteReg(uint8_t regaddr, uint8_t databyte)
{
    uint8_t temp[3];
    temp[0] = ADS1256_CMD_WREG | (regaddr & 0x0F);
    temp[1] = 0;
    temp[2] = databyte;

    SpiWrite(temp, sizeof(temp));

    HPT_DelayUs(kT11_1);
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

uint8_t Ads1256::ReadReg(uint8_t regaddr)
{
    uint8_t result = 0;
    uint8_t temp[2];
    temp[0] = ADS1256_CMD_RREG | (regaddr & 0x0F);
    temp[1] = 0;
    SpiWrite(temp, sizeof(temp));

    HPT_DelayUs(kT6);

    // Read reg
    SpiRead(&result, 1);

    HPT_DelayUs(kT11_1);
    return result;
}

void Ads1256::SetGain(PGA gain)
{
    // 实际上 ADCON 的 bit[2-0] 控制增益，其他 bit 控制时钟输出和传感器检测
    // 这里将时钟输出和传感器检测都关闭
    WriteReg(ADS1256_ADCON, (uint8_t)gain);
    pga_ = gain;
}

Ads1256::PGA Ads1256::GetGainFromChip()
{
    auto reg       = ReadReg(ADS1256_ADCON);
    uint8_t result = reg & 0x7;
    if (result == 0x7) {
        result = 0x6; // 0x6 和 0x7 都表示增益为 64 倍
    }
    pga_ = static_cast<PGA>(result);
    return static_cast<PGA>(result);
}

void Ads1256::SetDataRate(DataRate rate)
{
    WriteReg(ADS1256_DRATE, (uint8_t)rate);
}

Ads1256::DataRate Ads1256::GetDataRateFromChip()
{
    auto reg = ReadReg(ADS1256_DRATE);
    return static_cast<DataRate>(reg);
}

void Ads1256::SelfCalibrateOffsetGain()
{
    WriteCmd(ADS1256_CMD_SELFCAL);
    HPT_DelayUs(100);
    WaitForDataReady();
}

void Ads1256::WakeUp()
{
    WriteCmd(ADS1256_CMD_WAKEUP);
    HPT_DelayUs(kT11_2); // 手册里没说等多久，按最长时间等
}
