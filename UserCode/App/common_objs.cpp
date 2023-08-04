#include "common_objs.hpp"
#include "control_system/pll.hpp"
#include "line_calis.hpp"
#include <atomic>

BoardSelector kWhoAmI;

control_system::Pll<float> kAcOutPll(1.0 / 5000.0);                         // 交流输出端的 PLL
control_system::Pll<float> kGridPll(1.0 / 5000.0, 2 * 50 * M_PI, 1, 1, 10); // 电网 PLL
std::atomic<float> kMod;

std::atomic<float> kGridVoltage;

float GetGridVoltage()
{
    return kLineCali_GridAdc.Calc(Adc3.GetDiffVoltage(0, 1));
}
