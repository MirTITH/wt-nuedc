#include "main.h"
#include "tim.h"

#include "User_VC.h"

//初始化与采样数据处理线程
void Task_VC()
{
    #define VC_TASK_ENABLE
    HAL_TIM_Base_Start_IT(&htim1); //开启定时器
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
    User_VC_init();
}

//SVPWM占空比调整线程
void Task_Vc_Loop(TIM_HandleTypeDef *htim)
{
  #ifdef VC_TASK_ENABLE
  if ((htim->Instance == TIM1) && (1)) {
    
    //采样数据处理
    Sample_u.abc_value.a = 7.5*cos(theta_ref);//give the sampling data manually
    // Sample_Refesh(&Sample_i,&Sample_u,ia,ib,ic);
    
    //锁相
    PLL_SinglePhase(&amplitude_pll,&theta_pll,Sample_u.abc_value.a,theta_pll);
    #ifdef PLL_ENABLE
      // f_ref = f_ref_PLL;
      // theta_ref = theta_pll;
    #endif

    //伺服
    CurrentServo(&hVC,Id_Servo,Iq_Servo,theta_ref);
    
    Ud_Servo = 10;
    Uq_Servo = 0;
    f_ref = 50;
    theta_ref = W_VC * global_timer;
    VoltageServo(&Ua_ref,&Ub_ref,&Uc_ref,Ud_Servo,Uq_Servo,theta_ref);

    //SVPWM输出
    Spwm_Calculate(&Duty_abc,Gain_u,Ua_ref,Ub_ref,Uc_ref);
    __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,Duty_abc.ccRA);
    __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,Duty_abc.ccRB);
    __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,Duty_abc.ccRC);
  }
  #endif
}



