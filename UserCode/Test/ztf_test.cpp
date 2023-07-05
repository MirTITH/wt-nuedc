#include "main.h"
#include "control_system/z_tf.hpp"
#include "freertos_io/os_printf.h"
#include "HighPrecisionTime/high_precision_time.h"

using namespace std;

void ZtfTest()
{
    // ztf
    ZTf<float> ztf({66.2117333333333, -124.136000000000, 58.1856000000000},
                   {1, -0.333333333333333, -0.666666666666667});

    ztf.ResetState();
    uint32_t start_time = HPT_GetUs();
    for (size_t i = 0; i < 1000000; i++) {
        ztf.Step(1);
    }
    auto duration         = HPT_GetUs() - start_time;
    auto speed            = 1000000.0f / duration * 1000.0f;
    auto next_step_result = ztf.Step(1);

    ztf.ResetState();
    os_printf("ztf.Step(1): %g\n duration: %lu us\n speed: %g kps\n", next_step_result, duration, speed);

    for (size_t i = 0; i < 10; i++) {
        os_printf("%g\n", ztf.Step(1));
    }
}
