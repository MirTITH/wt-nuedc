#include "spwm_device.hpp"
#include "tim.h"

UnipolarSpwm kSpwm({&htim8, TIM_CHANNEL_1, &htim8, TIM_CHANNEL_2});
