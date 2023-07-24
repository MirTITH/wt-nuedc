#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "Coordinate_Trans.h"
#include "FeedBack.h"
#include "SVPWM.h"
#include "User_VC_Thread.h"
#include "VC_Controller.h"
#include "PLL.h"

#define W_VC (2*PI*f_ref)

void Task_VC();
void PLL_init(float kp,float ki,float fref);
void Task_Vc_Loop(TIM_HandleTypeDef *htim);//0.2ms

#ifdef __cplusplus
}
#endif
