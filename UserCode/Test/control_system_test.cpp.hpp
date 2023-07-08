#pragma once

#include "main.h"
#include "control_system/z_tf.hpp"
#include "freertos_io/os_printf.h"
#include "HighPrecisionTime/high_precision_time.h"
#include "control_system/pid_controller.hpp"
#include "control_system/saturation.hpp"

using namespace std;
using namespace control_system;

namespace user_test
{
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
void PidStepTest(DiscreteControllerBase<T> &controller, const uint32_t loop_time = 200000)
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

    os_printf("\n\nStep(1) from tick 1 to 10:\n");
    controller.ResetState();
    for (size_t i = 0; i < 10; i++) {
        os_printf("%g\t", controller.Step(1));
    }

    os_printf("\nThen Step(-1) from tick 11 to 20:\n");
    for (size_t i = 0; i < 10; i++) {
        os_printf("%g\t", controller.Step(-1));
    }
    os_printf("\n");
}

void PidTest()
{
    os_printf("\n==============Start PidTest=============\n");

    // os_printf("\np_controller:\n");
    // pid::P<float> p_controller{2};
    // PidStepTest(p_controller);

    // os_printf("\ni_controller:\n");
    // control_system::DiscreteIntegratorSaturation<float> i_controller{{2, 0.01}, {-10, 10}};
    // PidStepTest(i_controller);

    // os_printf("\nDiscreteIntegrator:\n");
    // control_system::DiscreteIntegrator<float> integrator{2, 0.01};
    // PidStepTest(integrator);

    // os_printf("\nd_controller:\n");
    // pid::D<double> d_controller{1, 100, 0.01};
    // PidStepTest(d_controller);

    os_printf("\npid_controller:\n");
    pid::PID<float> pid_controller{1.23, 0.54, 0.5, 100, 0.01};
    PidStepTest(pid_controller);

    os_printf("\npid_controller, no saturation:\n");
    pid::PID<float, DiscreteIntegrator<float>> pid_controller1{1.23, 0.54, 0.5, 100, 0.01};
    PidStepTest(pid_controller1);

    // os_printf( "\npi_controller:\n");
    // pid::PI<float> pi_controller{1.23, 0.54, 0.01};
    // PidStepTest(pi_controller, 1000000);

    // os_printf("\npi_controller, no saturation:\n");
    // pid::PI<float, DiscreteIntegrator<float>> pi_controller1{1.23, 0.54, 0.01};
    // PidStepTest(pi_controller1, 1000000);

    // os_printf("\npd_controller:\n");
    // pid::PD<float> pd_controller{1.23, 0.76, 100, 0.01};
    // PidStepTest(pd_controller);

    // os_printf("PID_AntiWindup:\n");
    // pid::PID_AntiWindup<float> pid_controller{2, 100, 0, 100, 0.01, 50, -5, 5};
    // // pid_controller.i_controller.SetOutputMinMax(-5, 5);
    // PidStepTest(pid_controller);
    // os_printf("\n");

    // os_printf("PI_AntiWindup:\n");
    // pid::PI_AntiWindup<float> pi_controller{2, 100, 0.01, 50, -5, 5};

    // pid_controller.i_controller.SetOutputMinMax(-5, 5);
    // PidStepTest(pi_controller);
    os_printf("\n");
}

} // namespace user_test
