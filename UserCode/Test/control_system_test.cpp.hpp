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

template <typename T>
void DiscreteControllerTest(DiscreteControllerBase<T> &controller, const uint32_t loop_time = 500000)
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

void ZtfTest()
{
    // ztf
    ZTf<float> ztf1({66.2117333333333, -124.136000000000, 58.1856000000000},
                    {1, -0.333333333333333, -0.666666666666667});
    os_printf("==== ZtfTest: ztf1 ====\n");
    DiscreteControllerTest(ztf1);

    ZTf<float> ztf2({1, 2}, {1, 2, 3, 4, 5});
    os_printf("==== ZtfTest: ztf2 ====\n");
    DiscreteControllerTest(ztf2);

    ZTf<float> ztf3({5}, {1});
    os_printf("==== ZtfTest: ztf3 ====\n");
    DiscreteControllerTest(ztf3);

    ZTf<float> ztf4({2.5}, {1, 3, 4});
    os_printf("==== ZtfTest: ztf4 ====\n");
    DiscreteControllerTest(ztf4);

    ZTf<float> ztf5({}, {1, 4, 4});
    os_printf("==== ZtfTest: ztf5 ====\n");
    DiscreteControllerTest(ztf5);

    ZTf<float> ztf6({1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    os_printf("==== ZtfTest: ztf6 ====\n");
    DiscreteControllerTest(ztf6);
}

void PidTest()
{
    os_printf("\n==============Start PidTest=============\n");

    // os_printf("\np_controller:\n");
    // pid::PController<float> p_controller{2};
    // DiscreteControllerTest(p_controller);

    // os_printf("\ni_controller:\n");
    // control_system::DiscreteIntegratorSaturation<float> i_controller{{2, 0.01}, {-10, 10}};
    // DiscreteControllerTest(i_controller);

    // os_printf("\nDiscreteIntegrator:\n");
    // control_system::DiscreteIntegrator<float> integrator{2, 0.01};
    // DiscreteControllerTest(integrator);

    // os_printf("\nd_controller:\n");
    // pid::DController<double> d_controller{1, 100, 0.01};
    // DiscreteControllerTest(d_controller);

    os_printf("\npid_controller:\n");
    pid::PIDController<float> pid_controller{1.23, 0.54, 0.5, 100, 0.01};
    DiscreteControllerTest(pid_controller);

    os_printf("\npid_controller, no saturation:\n");
    pid::PIDController<float, DiscreteIntegrator<float>> pid_controller1{1.23, 0.54, 0.5, 100, 0.01};
    DiscreteControllerTest(pid_controller1);

    // os_printf( "\npi_controller:\n");
    // pid::PIController<float> pi_controller{1.23, 0.54, 0.01};
    // DiscreteControllerTest(pi_controller, 1000000);

    // os_printf("\npi_controller, no saturation:\n");
    // pid::PIController<float, DiscreteIntegrator<float>> pi_controller1{1.23, 0.54, 0.01};
    // DiscreteControllerTest(pi_controller1, 1000000);

    // os_printf("\npd_controller:\n");
    // pid::PDController<float> pd_controller{1.23, 0.76, 100, 0.01};
    // DiscreteControllerTest(pd_controller);

    // os_printf("PID_AntiWindup:\n");
    // pid::PID_AntiWindup<float> pid_controller{2, 100, 0, 100, 0.01, 50, -5, 5};
    // // pid_controller.i_controller.SetOutputMinMax(-5, 5);
    // DiscreteControllerTest(pid_controller);
    // os_printf("\n");

    // os_printf("PI_AntiWindup:\n");
    // pid::PI_AntiWindup<float> pi_controller{2, 100, 0.01, 50, -5, 5};

    // pid_controller.i_controller.SetOutputMinMax(-5, 5);
    // DiscreteControllerTest(pi_controller);
    os_printf("\n");
}

} // namespace user_test
