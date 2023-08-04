#include "common_objs.hpp"
#include "control_system/pll.hpp"
#include "line_calis.hpp"
#include <atomic>
#include "Filters/butterworth.hpp"

BoardSelector kWhoAmI;

control_system::Pll<float> kAcOutPll(1.0 / 5000.0);                         // 交流输出端的 PLL
control_system::Pll<float> kGridPll(1.0 / 5000.0, 2 * 50 * M_PI, 1, 1, 1); // 电网 PLL

// 用于标定滤波
// Butter_LP_5_50_20dB_5000Hz<double> kGridFilter; 
// std::atomic<float> kGridFilterVoltage;

std::atomic<float> kGridVoltage;

float GetGridVoltage()
{
    // auto volt          = Adc3.GetDiffVoltage(0, 1);
    // kGridFilterVoltage = kGridFilter.Step(volt);
    return kLineCali_GridAdc.Calc(Adc3.GetDiffVoltage(0, 1));
    // return volt;
}
