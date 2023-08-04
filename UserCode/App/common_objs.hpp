#pragma once

#include <atomic>
#include "control_system/pll.hpp"
#include "Adc/adc_class_device.hpp"

extern std::atomic<float> kMod;
extern control_system::Pll<float> kAcOutPll;

enum class BoardSelector {
    A,
    B
};

extern BoardSelector kWhoAmI;

float GetGridVoltage();
