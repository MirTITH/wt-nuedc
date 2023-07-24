#include "main.h"
#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"

#include "User_VC.h"

uint8_t VC_TASK_ENABLE = 0;
uint8_t PLL_ENABLE     = 0;

// 初始化与采样数据处理线程
void Task_VC_init()
{
    VC_TASK_ENABLE = 1;
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim8); // 开启定时器
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
    User_VC_init();
}

// SPWM占空比调整线程
void Task_Vc_Loop_Spwm() // in freertos 1ms
{
    if (VC_TASK_ENABLE) {
        if (1) {
            // 伺服
            CurrentServo(&hVC, Id_Servo, Iq_Servo, theta_ref);

            Ud_Servo = 10;
            Uq_Servo = 0;
            theta_ref = W_VC * global_timer;
            if(PLL_ENABLE)
            {

            }
            VoltageServo(&Ua_ref, &Ub_ref, &Uc_ref, Ud_Servo, Uq_Servo, theta_ref);

            // SPWM输出
            Spwm_Calculate(&Duty_abc, Gain_u, Ua_ref, Ub_ref, Uc_ref);
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, Duty_abc.ccRA);
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, Duty_abc.ccRB);
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, Duty_abc.ccRC);
            os_printf("%f,%f,%f\n",Duty_abc.ccRA,Duty_abc.ccRB,Duty_abc.ccRC);
        }
    }
}

// SVPWM占空比调整线程
void Task_Vc_Loop_SVpwm(TIM_HandleTypeDef *htim) // 0.2ms in timer
{
    if (VC_TASK_ENABLE) {
        if ((htim->Instance == TIM8) && (htim->Instance->CNT < 10)) {
            // 伺服
            CurrentServo(&hVC, Id_Servo, Iq_Servo, theta_ref);

            Ud_Servo = 10;
            Uq_Servo = 0;
            theta_ref = W_VC * global_timer;
            if(PLL_ENABLE)
            {
                
            }
            VoltageServo(&Ua_ref, &Ub_ref, &Uc_ref, Ud_Servo, Uq_Servo, theta_ref);

            // SPWM输出
            Svpwm_Calculate(&Duty_abc, Udc, Ua_ref, Ub_ref, Uc_ref);
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, Duty_abc.ccRA);
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, Duty_abc.ccRB);
            __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, Duty_abc.ccRC);
        }
    }
}

// void task_pll_loop(TIM_HandleTypeDef *htim)//in timer 0.25ms
// {
//   //采样数据处理
//   if (htim->Instance == TIM1)
//       {
//         f_ref = 50;
//         theta_ref = W_VC * global_timer;
//         Sample_u.abc_value.a = 7.5*cos(theta_ref);//give the sampling data manually
//         // Sample_Refesh(&Sample_i,&Sample_u,ia,ib,ic);
//         //锁相
//         if(PLL_ENABLE){
//         PLL_SinglePhase(&amplitude_pll,&theta_pll,Sample_u.abc_value.a,theta_pll);
//         // f_ref = f_ref_PLL;
//         // theta_ref = theta_pll;
//         }
//       }
// }
