#include "ads1256.hpp"
#include "ads1256_cmd_define.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "FreeRTOS.h"
#include "task.h"
#include "freertos_io/os_printf.h"
#include <string.h>
#include "in_handle_mode.h"

static constexpr float kClkin = 7.68; // MHz, 晶振频率

// 延时数据 (page 6)
// > 50 tclkin, Delay from last SCLK edge for DIN to first SCLK rising edge for DOUT: RDATA, RDATAC, RREG Commands
static constexpr uint32_t kT6 = 50 / kClkin + 2;

// > 8 tclkin, nCS low after final SCLK falling edge
static constexpr uint32_t kT10 = 8 / kClkin + 2;

// Final SCLK falling edge of command to first SCLK rising edge of next command.
static constexpr uint32_t kT11_1 = 4 / kClkin + 2;  // > 4 tclkin, for RREG, WREG, RDATA
static constexpr uint32_t kT11_2 = 24 / kClkin +2; // > 24 tclkin, for RDATAC, SYNC

// RESET, SYNC/PDWN, pulse width
static constexpr uint32_t kT16 = 4 / kClkin + 2; // > 4 tclkin

static const uint8_t zeros_[3]{};

void Ads1256::DRDY_Callback()
{
    drdy_count_++;
    if (use_conv_queue_) {
        if (IsDmaCplt() == false) {
            dma_busy_count_++;
            return;
        }

        switch (conv_queue_.size()) {
            case 0:
                break;
            case 1:
                if (is_in_rdatac_mode_) {
                    ReadDataContinousToQueueDma(0);
                } else {
                    ReadDataToQueueDma(0);
                }
                break;

            default:
                size_t now_index = conv_queue_index_;

                // 下一个通道
                size_t next_index = now_index + 1;
                if (next_index >= conv_queue_.size()) {
                    next_index = 0;
                }
                conv_queue_index_ = next_index;

                // 开始转换下一个通道
                SetMux(conv_queue_.at(next_index).mux);

                // 上一个通道转换完成，读取它的值
                ReadDataToQueueDma(now_index);
                break;
        }
    }
}

void Ads1256::Init(DataRate data_rate, PGA gain, bool input_buffer, bool auto_calibration)
{
    // 等待电源稳定
    vTaskDelay(100);

    Reset();

    // 尝试 4 次
    for (size_t i = 0; i < 4; i++) {
        if (CheckForReset() == true) {
            break;
        }
        Reset();
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
    SetGain(gain);

    /**
     * @brief STATUS REGISTER
     * default value: 0xX1 (X 为 ID，未知)
     * ORDER: Most Significant Bit First (default: MSB)
     * 自动校准: 开启 (default: OFF)
     * BUFEN: default: Disable
     */
    // WriteReg(ADS1256_STATUS, 0x04); // Buffer disable
    // WriteReg(ADS1256_STATUS, 0x06); // Buffer enable
    SetInputBufferAndAutoCalibration(input_buffer, auto_calibration);

    /**
     * @brief A/D Data Rate
     * 默认值：30,000SPS
     */
    SetDataRate(data_rate);

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
    StopConvQueue();
    WaitForDmaCplt();

    if (n_reset_port_ != nullptr) {
        HAL_GPIO_WritePin(n_reset_port_, n_reset_pin_, GPIO_PIN_RESET);
        vTaskDelay(10);
        HAL_GPIO_WritePin(n_reset_port_, n_reset_pin_, GPIO_PIN_SET);
    } else {
        // 多发送几个 RESET 命令，防止 RESET 命令不能正确送达
        // 尽管如此，ADS 还是有可能不能成功 RESET，建议使用 RESET 引脚
        // 原因之一：
        // ADS 读写指令需要多个 spi 字节周期，如果上一时刻 ADS 正在进行读写指令，
        // 则此时发送 RESET 可能被 ADS 认为是整个读写过程的一部分，从而不能 RESET
        for (size_t i = 0; i < 5; i++) {
            WaitForDataReady();
            WriteCmd(ADS1256_CMD_RESET);
            vTaskDelay(100);
        }
    }

    vTaskDelay(100);
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

void Ads1256::ReadDataToQueueDma(uint8_t queue_index)
{
    WriteCmd(ADS1256_CMD_RDATA);
    HPT_DelayUs(kT6);

    dma_transfer_index_ = queue_index;
    if (SpiReadDma(dma_rx_buffer_, 3) == false) {
        dma_transfer_index_ = 0xff;
    }
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

void Ads1256::EnterReadDataContinousMode()
{
    WaitForDataReady();
    WriteCmd(ADS1256_CMD_RDATAC);
    is_in_rdatac_mode_ = true;
}

void Ads1256::ExitReadDataContinousMode()
{
    WaitForDataReady();
    WriteCmd(ADS1256_CMD_SDATAC);
    HPT_DelayUs(kT6); // 手册里没写，按最大时间延时
    is_in_rdatac_mode_ = false;
}

void Ads1256::ReadDataContinousToQueueDma(uint8_t queue_index)
{
    dma_transfer_index_ = queue_index;
    if (SpiReadDma(dma_rx_buffer_, 3) == false) {
        dma_transfer_index_ = 0xff;
    }
}

void Ads1256::InitAdsGpio()
{
    WriteReg(ADS1256_IO, kIO_REG_INIT_VALUE);
}

int32_t Ads1256::ReadData()
{
    assert(GetConvQueueState() == false); // 如果启动了转换队列，请不要使用这个函数，否则会导致 SPI 读写冲突
    WaitForDataReady();

    WriteCmd(ADS1256_CMD_RDATA);
    HPT_DelayUs(kT6);

    uint8_t data[3];
    SpiRead(data, 3);

    return RawDataToInt32(data);
}

void Ads1256::SpiRead(uint8_t *rx_data, uint16_t Size, uint32_t Timeout)
{
    HAL_SPI_TransmitReceive(hspi_, (uint8_t *)zeros_, rx_data, Size, Timeout);
}

bool Ads1256::SpiReadDma(uint8_t *rx_data, uint16_t Size)
{
    if (HAL_SPI_TransmitReceive_DMA(hspi_, (uint8_t *)zeros_, rx_data, Size) == HAL_OK) {
        return true;
    }

    return false;
}

void Ads1256::SpiWrite(const uint8_t *tx_data, uint16_t Size, uint32_t Timeout)
{
    HAL_SPI_TransmitReceive(hspi_, (uint8_t *)tx_data, (uint8_t *)zeros_, Size, Timeout);
}

void Ads1256::SetConvQueue(const std::vector<uint8_t> &muxs)
{
    assert(GetConvQueueState() == false); // 请先关闭转换队列

    auto in_isr = InHandlerMode();
    UBaseType_t uxSavedInterruptStatus;
    if (in_isr) {
        uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
    } else {
        taskENTER_CRITICAL();
    }

    conv_queue_index_ = 0;

    auto size = muxs.size();
    conv_queue_.resize(size);
    for (size_t i = 0; i < size; i++) {
        conv_queue_.at(i).mux  = muxs.at(i);
        conv_queue_.at(i).data = 0;
    }

    if (in_isr) {
        taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
    } else {
        taskEXIT_CRITICAL();
    }
}

void Ads1256::StartConvQueue()
{
    assert(use_conv_queue_ == false); // 开启后不要再次开启

    if (conv_queue_.size() != 0) {
        SetMux(conv_queue_.at(0).mux);
    }

    if (conv_queue_.size() == 1) {
        EnterReadDataContinousMode();
    }

    use_conv_queue_ = true;
}

void Ads1256::StopConvQueue()
{
    use_conv_queue_ = false;
    WaitForDmaCplt();
    if (is_in_rdatac_mode_) {
        ExitReadDataContinousMode();
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
    assert(GetConvQueueState() == false); // 转换队列启动时不要读写 ADS

    // 实际上 ADCON 的 bit[2-0] 控制增益，其他 bit 控制时钟输出和传感器检测
    // 这里将时钟输出和传感器检测都关闭
    WriteReg(ADS1256_ADCON, (uint8_t)gain);
    pga_ = gain;
}

Ads1256::PGA Ads1256::GetGainFromChip()
{
    assert(GetConvQueueState() == false); // 转换队列启动时不要读写 ADS

    auto reg       = ReadReg(ADS1256_ADCON);
    uint8_t result = reg & 0x7;
    if (result == 0x7) {
        result = 0x6; // 0x6 和 0x7 都表示增益为 64 倍
    }
    pga_ = static_cast<PGA>(result);
    return static_cast<PGA>(result);
}

void Ads1256::SetInputBufferAndAutoCalibration(bool input_buffer, bool auto_calibration)
{
    uint8_t value = (input_buffer << 1) | (auto_calibration << 2);
    WriteReg(ADS1256_STATUS, value);
    enable_input_buffer_     = input_buffer;
    enable_auto_calibration_ = auto_calibration;
}

void Ads1256::SetDataRate(DataRate rate)
{
    assert(GetConvQueueState() == false); // 转换队列启动时不要读写 ADS
    WriteReg(ADS1256_DRATE, (uint8_t)rate);
    data_rate_ = rate;
}

Ads1256::DataRate Ads1256::GetDataRateFromChip()
{
    auto reg = ReadReg(ADS1256_DRATE);
    return static_cast<DataRate>(reg);
}

void Ads1256::SelfCalibrateOffsetGain()
{
    assert(GetConvQueueState() == false); // 转换队列启动时不要读写 ADS
    WriteCmd(ADS1256_CMD_SELFCAL);
    HPT_DelayUs(100);
    WaitForDataReady();
}

bool Ads1256::CheckForReset()
{
    assert(GetConvQueueState() == false); // 转换队列启动时不要读写 ADS

    auto io_reg = ReadReg(ADS1256_IO);

    // IO 寄存器的 bit7 ~ bit4 为输入输出方向，Reset Value = 0xE0
    if ((io_reg & 0xF0) == 0xE0) {
        return true;
    }

    return false;
}

bool Ads1256::CheckForPresent()
{
    assert(GetConvQueueState() == false); // 转换队列启动时不要读写 ADS

    WaitForDataReady(1000 * 1000);
    auto status_reg = ReadReg(ADS1256_STATUS);

    if ((status_reg & 0xF0) != (kADS1256_ID << 4)) {
        return false;
    }

    return true;
}

bool Ads1256::CheckForConfig()
{
    auto regs = ReadAllRegs();

    if ((regs.IO) != kIO_REG_INIT_VALUE) {
        return false;
    }

    if (regs.DRATE != (uint8_t)data_rate_) {
        return false;
    }

    if ((regs.ADCON & 0x7) != (uint8_t)pga_) {
        return false;
    }

    uint8_t exp_value = (kADS1256_ID << 4) |
                        (enable_input_buffer_ << 1) |
                        (enable_auto_calibration_ << 2);
    if ((regs.STATUS & 0xFE) != exp_value) {
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
