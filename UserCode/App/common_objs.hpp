#pragma once

#include <atomic>
#include "control_system/pll.hpp"

extern std::atomic<float> kMod;
extern control_system::Pll<float> kAcOutPll;