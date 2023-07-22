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
 * 以下内容为写驱动时的笔记，使用者可以不必深究
 * 在 f = 7.68 MHz (T = 130 ns) 下：
 *
 * (手册 page 5)
 * SPI T_min = 4T = 521 ns
 * 输入命令到读取数据的延时 T_min = 50T = 6510 ns
 * 命令之间延时各不相同，最长的需要 24T = 3125 ns
 * 片选使能后可以立即进行操作，但失能需要与最后一个读写操作相隔至少 8T = 1042 ns
 *
 * Settling Time Using the Input Multiplexer读取数据时，
 * 需要在发送 SYNC 命令后在 SETTLING TIME 内完成
 * SETTLING TIME 长度随着 DATA RATE 变化（见手册Table 13, page 20）
 * 摘取几个值：
 * DATA RATE        SETTLING TIME(ms)
 * 30 000           0.21
 * 2000             0.68
 * 1000             1.18
 * 500              2.18
 * 100              10.18
 */

#include "ads1256.hpp"
#include "ads1256_cmd_define.h"
#include "HighPrecisionTime/high_precision_time.h"

constexpr float kClkin = 7.68; // MHz

// 延时数据 (page 6)
constexpr uint32_t kT6    = 80 / kClkin + 1; // > 50 tclk
constexpr uint32_t kT10   = 16 / kClkin + 1; // > 8 tclk
constexpr uint32_t kT11_1 = 8 / kClkin + 1;  // for RREG, WREG, RDATA, > 4 tclk
constexpr uint32_t kT11_2 = 48 / kClkin + 1; // for RDATAC, nSYNC, > 24 tclk

void Ads1256::Init()
{
    // 等待电源稳定
    HPT_DelayMs(100);

    Reset();

    /**
     * @brief STATUS REGISTER
     * ORDER: Most Significant Bit First (default)
     * 自校准: 开启
     * BUFEN: Buffer Enabled
     */
    WriteReg(ADS1256_STATUS, 0x06);

    /**
     * @brief Input Multiplexer Control Register
     * Positive Input Channel: AIN0 (default)
     * Negative Input Channel: AINCOM
     */
    WriteReg(ADS1256_MUX, 0x01);

    /**
     * @brief  A/D Control Register
     * Clock Out OFF
     * Sensor Detect OFF (default)（可用来检测外部传感器是否完好）
     * Programmable Gain Amplifier Setting = 1
     */
    WriteReg(ADS1256_ADCON, 0x00);

    /**
     * @brief A/D Data Rate
     * 默认值：30,000SPS
     */
    WriteReg(ADS1256_DRATE, ADS1256_DRATE_1000);

    /**
     * @brief GPIO Control Register
     * 全部设为 Output
     * 引脚电平全部设为 low
     */
    WriteReg(ADS1256_IO, 0x00);

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
        WaitForDataReady();
        WriteCmd(ADS1256_CMD_RESET);
        HPT_DelayMs(10);
    }
}

int32_t Ads1256::ReadData()
{
    WaitForDataReady();
    WriteCmd(ADS1256_CMD_RDATA);
    HPT_DelayUs(kT6);
    uint8_t data[3];
    SpiRead(data, 3);

    // 将读取的三段数据拼接
    int32_t result = 0;
    result |= ((int32_t)data[0] << 16);
    result |= ((int32_t)data[1] << 8);
    result |= (int32_t)data[2];

    // 处理负数
    if (result & 0x800000) {
        result = ~(unsigned long)result;
        result &= 0x7fffff;
        result += 1;
        result = -result;
    }
    return result;
}

bool Ads1256::SpiRead(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if (HAL_SPI_Receive(hspi_, pData, Size, Timeout) == HAL_OK) {
        return true;
    } else {
        return false;
    }
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

    WaitForDataReady();
    SpiWrite(temp, sizeof(temp));

    // Write reg
    SpiWrite(databyte, size);
    HPT_DelayUs(10); // t11
}

void Ads1256::WriteReg(uint8_t regaddr, uint8_t databyte)
{
    WriteReg(regaddr, &databyte, 1);
}

void Ads1256::WriteCmd(uint8_t cmd)
{
    SpiWrite(&cmd, 1);
}

void Ads1256::WaitForDataReady()
{
    while (IsDataReady() == false) {};
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
    WaitForDataReady();
    SpiWrite(temp, sizeof(temp));

    HPT_DelayUs(10); // t6

    // Read reg
    SpiRead(databyte, size);

    HPT_DelayUs(10); // t11
}

uint8_t Ads1256::ReadReg(uint8_t regaddr)
{
    uint8_t result = 0;
    ReadReg(regaddr, &result, 1);
    return result;
}
