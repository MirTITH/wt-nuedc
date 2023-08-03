#include "common_objs.hpp"
#include "control_system/pll.hpp"

control_system::Pll<float> kAcOutPll(1.0 / 5000.0); // 交流输出端的 PLL
std::atomic<float> kMod;

BoardSelector kWhoAmI;
