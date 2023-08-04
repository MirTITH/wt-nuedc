#pragma once

#include <atomic>
#include "control_system/pll.hpp"
#include "Adc/adc_class_device.hpp"
#include "control_system/pid_controller.hpp"
#include "ads1256/ads1256_device.hpp"
#include "Relay/relay_device.hpp"
#include "pwm/spwm_device.hpp"
#include "Vofa/just_float.hpp"
#include "Keyboard/keyboard_device.hpp"
#include "Encoder/encoder_device.hpp"

extern control_system::Pll<float> kAcOutPll;

extern control_system::Pll<float> kGridPll;
extern std::atomic<float> kGridVoltage;
// extern std::atomic<float> kGridFilterVoltage;

enum class BoardSelector {
    A,
    B
};

extern BoardSelector kWhoAmI;

float GetGridVoltage();
