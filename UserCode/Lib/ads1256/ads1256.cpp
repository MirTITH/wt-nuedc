#include "ads1256.hpp"
#include "ads1256_cmd_define.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_io/os_printf.h"

static constexpr float kClkin = 7.68; // MHz, 晶振频率

// 延时数据 (page 6)
// > 50 tclkin, Delay from last SCLK edge for DIN to first SCLK rising edge for DOUT: RDATA, RDATAC, RREG Commands
static constexpr uint32_t kT6 = 50 / kClkin + 2;

// > 8 tclkin, nCS low after final SCLK falling edge
static constexpr uint32_t kT10 = 8 / kClkin + 2;

// Final SCLK falling edge of command to first SCLK rising edge of next command.
static constexpr uint32_t kT11_1 = 4 / kClkin + 2;  // > 4 tclkin, for RREG, WREG, RDATA
static constexpr uint32_t kT11_2 = 24 / kClkin + 2; // > 24 tclkin, for RDATAC, SYNC

// RESET, SYNC/PDWN, pulse width
static constexpr uint32_t kT16 = 8 / kClkin + 2; // > 4 tclkin

void Ads1256::Init()
{
    // 等待电源稳定
    vTaskDelay(100);

    Reset();

    if (CheckForReset() == false) {
        os_printf("CheckForReset failed\n");
    }

    InitAdsGpio();

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
    SetDataRate(DataRate::SPS_7500);

    /**
     * @brief Input Multiplexer Control Register
     * Positive Input Channel: AIN0 (default)
     * Negative Input Channel: AINCOM
     */
    // WriteReg(ADS1256_MUX, 0x01);

    // 自校准
    SelfCalibrateOffsetGain();
    vTaskDelay(100);
}

void Ads1256::Reset()
{
    ExitPowerDownMode();
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
        // for (size_t i = 0; i < 5; i++) {
        //     WriteCmd(ADS1256_CMD_WAKEUP);
        //     HPT_DelayUs(kT6); // 手册里没说等多久，按最长时间等
        // }

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
    if (n_sync_port_ == nullptr) {
        WriteCmd(ADS1256_CMD_SYNC);
        HPT_DelayUs(kT11_2);
        WriteCmd(ADS1256_CMD_WAKEUP);
    } else {
        HAL_GPIO_WritePin(n_sync_port_, n_sync_pin_, GPIO_PIN_RESET);
        HPT_DelayUs(kT16);
        HAL_GPIO_WritePin(n_sync_port_, n_sync_pin_, GPIO_PIN_SET);
    }
}

void Ads1256::EnterRDataCMode()
{
    WaitForDataReady();
    WriteCmd(ADS1256_CMD_RDATAC);
    HPT_DelayUs(kT6);
    ReadDataCNoWait();
    is_in_rdatac_mode_ = true;
}

void Ads1256::ExitRDataCMode()
{
    WaitForDataReady();
    WriteCmd(ADS1256_CMD_SDATAC);
    HPT_DelayUs(kT6); // 手册里没写，按最大时间延时
    is_in_rdatac_mode_ = false;
}

int32_t Ads1256::ReadDataCNoWait()
{
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

void Ads1256::InitAdsGpio()
{
    WriteReg(ADS1256_IO, kIO_REG_INIT_VALUE);
}

void Ads1256::DRDY_Callback()
{
    drdy_count_++;
    if (use_conv_queue_) {
        switch (conv_queue_.size()) {
            case 0:
                break;
            case 1:
                if (is_in_rdatac_mode_) {
                    conv_queue_.at(0).value = ReadDataCNoWait();
                } else {
                    conv_queue_.at(0).value = ReadDataNoWait();
                }

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

void Ads1256::SpiRead(uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    HAL_SPI_Receive(hspi_, pData, Size, Timeout);
}

void Ads1256::SpiWrite(const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    HAL_SPI_Transmit(hspi_, (uint8_t *)pData, Size, Timeout);
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

    // ReadDataC 有问题，不启用
    // if (conv_queue_.size() == 1) {
    //     EnterRDataCMode();
    // }

    use_conv_queue_ = true;
}

void Ads1256::StopConvQueue()
{
    use_conv_queue_ = false;
    if (is_in_rdatac_mode_) {
        ExitRDataCMode();
    }
}

void Ads1256::SetMux(uint8_t mux)
{
    WriteReg(ADS1256_MUX, mux);
    SyncWakeup();
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

bool Ads1256::CheckForReset()
{
    auto io_reg = ReadReg(ADS1256_IO);

    // IO 寄存器的 bit7 ~ bit4 为输入输出方向，Reset Value = 0xE0
    if ((io_reg & 0xF0) == 0xE0) {
        return true;
    }

    return false;
}

bool Ads1256::CheckForPresent()
{
    auto status_reg = ReadReg(ADS1256_STATUS);

    // ADS1256 的 STATUS REGISTER 的 bit7~bit4 为 Factory Programmed Identification Bits.
    // datasheet 中未给出，实测该值为 0x3
    if ((status_reg & 0xF0) != 0x30) {
        return false;
    }

    return true;
}

void Ads1256::EnterPowerDownMode()
{
    // Holding the SYNC/PDWN pin low for 20 DRDY cycles activates the Power-Down mode.
    HAL_GPIO_WritePin(n_sync_port_, n_sync_pin_, GPIO_PIN_RESET);
}

void Ads1256::ExitPowerDownMode()
{
    // To exit Power-Down mode, take the SYNC/PDWN pin high.
    // Upon exiting from Power-Down mode, the ADS1255/6 crystal oscillator typically requires 30ms to wake up.
    // If using an external clock source, 8192 CLKIN cycles are needed before conversions begin.
    HAL_GPIO_WritePin(n_sync_port_, n_sync_pin_, GPIO_PIN_SET);
    vTaskDelay(100);
}
