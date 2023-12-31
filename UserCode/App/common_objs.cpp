#include "common_objs.hpp"
#include "control_system/pll.hpp"
#include "line_calis.hpp"
#include <atomic>
#include "Filters/butterworth.hpp"

BoardSelector kWhoAmI;

control_system::Pll<float> kAcOutPll(1.0 / 5000.0);                        // 交流输出端的 PLL
control_system::Pll<float> kGridPll(1.0 / 5000.0, 2 * 50 * M_PI, 1, 1, 1); // 电网 PLL

// 用于标定滤波
// Butter_LP_5_50_20dB_5000Hz<double> kGridFilter;
// std::atomic<float> kGridFilterVoltage;

std::atomic<float> kGridVoltage;

// 观察器
std::atomic<float> kAcVrefWatcher;
std::atomic<float> kAcIrefWatcher;
std::atomic<float> kActiveInv_ControllerOutputWatcher = 0;
std::atomic<float> kOnGridInv_ErrPhaseWatcher         = 0;

float GetGridVoltage()
{
    // auto volt          = kLineCali_GridAdc.Calc((Adc1.GetData(0) - Adc2.GetData(0)) * 3.3f / (1 << 12));
    // kGridFilterVoltage = kGridFilter.Step(volt);
    // return volt;
    return kLineCali_GridAdc.Calc((Adc1.GetData(0) - Adc2.GetData(0)) * 3.3f / (1 << 12));
}
