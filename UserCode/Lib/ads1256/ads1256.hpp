#pragma once
#include "main.h"
#include "spi.h"

class Ads1256
{
public:
    typedef struct {
        uint8_t STATUS;
        uint8_t MUX;
        uint8_t ADCON;
        uint8_t DRATE;
        uint8_t IO;
    } __attribute__((packed)) Registers_t;

    uint32_t drdy_count = 0;

public:
    Ads1256(SPI_HandleTypeDef *hspi,
            GPIO_TypeDef *n_drdy_port, uint16_t n_drdy_pin,
            GPIO_TypeDef *n_reset_port, uint16_t n_reset_pin)
        : hspi_(hspi),
          n_drdy_port_(n_drdy_port), n_drdy_pin_(n_drdy_pin),
          n_reset_port_(n_reset_port), n_reset_pin_(n_reset_pin){};

    Ads1256(SPI_HandleTypeDef *hspi, GPIO_TypeDef *n_drdy_port, uint16_t n_drdy_pin)
        : hspi_(hspi), n_drdy_port_(n_drdy_port), n_drdy_pin_(n_drdy_pin)
    {
        n_reset_port_ = nullptr;
        n_reset_pin_  = 0;
    };

    void Init();

    bool IsDataReady()
    {
        return !HAL_GPIO_ReadPin(n_drdy_port_, n_drdy_pin_);
    }

    void Reset();

    Registers_t ReadAllRegs()
    {
        Registers_t result{};
        ReadReg(0x00, (uint8_t *)(&result), sizeof(result));
        return result;
    }

    void DRDY_Callback()
    {
        drdy_count++;
    }

    int32_t ReadData();

    float ReadVoltage()
    {
        return vref_2 * ReadData() / 0x7FFFFF;
    }

private:
    SPI_HandleTypeDef *hspi_;

    GPIO_TypeDef *n_drdy_port_;
    uint16_t n_drdy_pin_;

    GPIO_TypeDef *n_reset_port_;
    uint16_t n_reset_pin_;

    float vref_2 = 5.0f; // 2 * vref

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

    void WriteCmd(uint8_t cmd);

    void WaitForDataReady();

    /**
     * @brief 读取寄存器
     *
     * @param regaddr The address of the first register to read
     * @param buffer Read buffer
     * @param size The number of bytes to read [1, 16]
     */
    void ReadReg(uint8_t regaddr, uint8_t *databyte, uint8_t size);

    uint8_t ReadReg(uint8_t regaddr);
};
