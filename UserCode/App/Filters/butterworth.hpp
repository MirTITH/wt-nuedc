#pragma once

#include "control_system/z_tf.hpp"

template <typename T>
class Butter_LP_5_20_40dB_5000Hz : public control_system::DiscreteControllerBase<T>
{
private:
    control_system::ZTf<T> section1{{1, 2, 1},
                                    {1,
                                     -1.99387228140026384615168808522867038846,
                                     0.993935262537587349562784311274299398065}};
    T gain1 = 0.000015745284330885315826143236450640472;

    control_system::ZTf<T> section2{{1, 2, 1},
                                    {1,
                                     -1.985358237412071291672077677503693848848,
                                     0.985420949613325825033882665593409910798}};
    T gain2 = 0.000015678050313616359134720468215817846;

public:
    T Step(T input) override
    {
        return gain1 * section1.Step(gain2 * section2.Step(input));
    }

    void ResetState() override
    {
        section1.ResetState();
        section2.ResetState();
    }
};
