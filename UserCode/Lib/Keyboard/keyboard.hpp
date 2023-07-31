#pragma once

#include "main.h"
#include <array>
#include "HighPrecisionTime/high_precision_time.h"
#include <cassert>

enum class Keys {
    k0 = 0,
    k1,
    k2,
    k3,
    k4,
    k5,
    k6,
    k7,
    k8,
    k9,
    kDot,
    kNegative,
    kLeft,
    kRight,
    kBackspace,
    kOk,
    kKnobBtn,
    kSwitch
};

class KeyboardClass
{
private:
    using Gpio_t = struct
    {
        GPIO_TypeDef *port;
        uint16_t pin;
    };

public:
    KeyboardClass(const std::array<Gpio_t, 4> &addr_gpios, const Gpio_t &data_gpio)
        : addr_gpios_(addr_gpios), data_gpio_(data_gpio){};

    bool ReadKey(Keys key)
    {
        switch (key) {
            case Keys::kKnobBtn:
                return !HAL_GPIO_ReadPin(Key_EncoderBtn_GPIO_Port, Key_EncoderBtn_Pin);
                break;
            case Keys::kSwitch:
                return HAL_GPIO_ReadPin(Key_Switch_GPIO_Port, Key_Switch_Pin);
                break;

            default:
                assert((int)key < 16);
                SetAddr((int)key);
                HPT_DelayUs(10);
                return ReadData();
                break;
        }
    }

private:
    std::array<Gpio_t, 4> addr_gpios_;
    Gpio_t data_gpio_;

    void WritePin(const Gpio_t &gpio, bool level)
    {
        HAL_GPIO_WritePin(gpio.port, gpio.pin, (GPIO_PinState)level);
    }

    bool ReadData()
    {
        return HAL_GPIO_ReadPin(data_gpio_.port, data_gpio_.pin);
    }

    void SetAddr(size_t addr)
    {
        for (size_t i = 0; i < 4; i++) {
            WritePin(addr_gpios_[i], (addr >> i) & 1);
        }
    }

    friend void UpdateButtonMatrix();
};
