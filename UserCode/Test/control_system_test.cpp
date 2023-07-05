#include "main.h"
#include "control_system/z_tf.hpp"
#include "freertos_io/os_printf.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "control_system/pid_controller.hpp"

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

template <typename T>
void PidStepTest(DiscreteTf<T> &controller, const uint32_t loop_time = 200000)
{
    uint32_t start_time = HPT_GetUs();
    for (size_t i = 0; i < loop_time; i++) {
        controller.Step(1);
    }
    auto duration = HPT_GetUs() - start_time;
    auto speed    = (float)loop_time / duration * 1000.0f;
    os_printf("Step(1) for %lu times: duration: %lu us, speed: %g kps\n", loop_time, duration, speed);

    os_printf("Step(1) from %lu to %lu times:\n", loop_time + 1, loop_time + 11);
    for (size_t i = 0; i < 10; i++) {
        os_printf("%g\t", controller.Step(1));
    }

    os_printf("\n\nStep(1) from 1 to 10 times:\n");
    controller.ResetState();
    for (size_t i = 0; i < 10; i++) {
        os_printf("%g\t", controller.Step(1));
    }
    os_printf("\n");
}

void PidTest()
{
    os_printf("\n==============Start PidTest=============\n");

    // os_printf("\npidn_controller:\n");
    // Pidn<float> pidn_controller{1.23, 0.54, 0.76, 1000, 0.01};
    // PidStepTest(pidn_controller);

    // os_printf("\np_controller:\n");
    // pid::P<float> p_controller{2};
    // PidStepTest(p_controller);

    // os_printf("\ni_controller:\n");
    // pid::I<float> i_controller{2, 0.01};
    // PidStepTest(i_controller);

    // os_printf("\nd_controller:\n");
    // pid::D<double> d_controller{1, 100, 0.01};
    // PidStepTest(d_controller);

    os_printf("\npid_controller_I:\n");
    pid::PID<float> pid_controller{1.23, 0.54, 0, 100, 0.01};
    PidStepTest(pid_controller);

    os_printf("\npid_controller_D:\n");
    pid_controller.SetParam(1.23, 0, 0.76, 100);
    pid_controller.ResetState();
    PidStepTest(pid_controller);

    os_printf("\npi_controller:\n");
    pid::PI<float> pi_controller{1.23, 0.54, 0.01};
    PidStepTest(pi_controller);

    os_printf("\npd_controller:\n");
    pid::PD<float> pd_controller{1.23, 0.76, 100, 0.01};
    PidStepTest(pd_controller);
}
