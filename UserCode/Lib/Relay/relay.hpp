#pragma once

#include "main.h"
#include <cassert>

enum class Relay_State {
    Close = 0,
    On
};

class ElectricRelay
{
public:
private:
    GPIO_TypeDef *port_;
    uint16_t pin_;

public:
    ElectricRelay(GPIO_TypeDef *port, uint16_t pin)
        : port_(port), pin_(pin){};

    void Set(Relay_State state)
    {
        switch (state) {
            case Relay_State::On:
                HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET);
                break;

            case Relay_State::Close:
                HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET);
                break;

            default:
                assert(false); // Unknown state
                break;
        }
    }
};
