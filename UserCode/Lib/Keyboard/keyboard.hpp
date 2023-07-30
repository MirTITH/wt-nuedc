#pragma once

#include "main.h"
#include <array>
#include "HighPrecisionTime/high_precision_time.h"

enum class Key {
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
    kEncoderBtn
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

    bool ReadKey(Key key)
    {
        return ReadKey((uint8_t)key);
    }

    bool ReadKey(uint8_t index)
    {
        if (index < 16) {
            for (size_t i = 0; i < 4; i++) {
                WritePin(addr_gpios_[i], (index >> i) & 1);
            }
        } else if (index == 16) {
            return !HAL_GPIO_ReadPin(Key_EncoderBtn_GPIO_Port, Key_EncoderBtn_Pin);
        }

        HPT_DelayUs(10);

        auto result = ReadPin(data_gpio_);
        return result;
    }

    bool ReadSwitch()
    {
        return HAL_GPIO_ReadPin(Key_Switch_GPIO_Port, Key_Switch_Pin);
    }

private:
    std::array<Gpio_t, 4> addr_gpios_;
    Gpio_t data_gpio_;

    void WritePin(const Gpio_t &gpio, bool level)
    {
        HAL_GPIO_WritePin(gpio.port, gpio.pin, (GPIO_PinState)level);
    }

    bool ReadPin(const Gpio_t &gpio)
    {
        return HAL_GPIO_ReadPin(gpio.port, gpio.pin);
    }
};
