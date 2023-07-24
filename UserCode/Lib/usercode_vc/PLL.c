#include "PLL.h"
#include "Coordinate_Trans.h"
#include "VC_Controller.h"

float Kp_pll,Ki_PLL,f_ref_PLL;

void PLL_init(float kp,float ki,float fref)
{
    Kp_pll = kp;
    Ki_PLL = ki;
    f_ref_PLL = fref;
    #define PLL_ENABLE
}

void PLL_SinglePhase(float *amplitude_pll,float *theta_pll,float Sample_in,float theta_pll_in)//0.2ms
{
    CT_data_t pll_data;
    PID_t id_con;
    float wc = 2*PI*f_ref;
    float id_out_sum = wc;//前馈偏置
    uint8_t num_reg = (uint8_t)(1250/f_ref_PLL);
    float Sample_reg[num_reg];
    uint8_t i = 0;
    for(i=0; i<num_reg; i++)
    {
        if(i < num_reg-1) Sample_reg[i] = Sample_reg[i+1];
        else Sample_reg[i] = Sample_in;//Sample[num_reg-1]
    }

    CT_Refresh_AlphaBeta(&pll_data,Sample_in,Sample_reg[0],theta_pll_in+(PI/2));

    id_con.KP = Kp_pll;
    id_con.KI = Ki_PLL;
    id_con.KD = 0;
    id_con.outputMax = 80;
    id_con.outputMin = 0;

    id_con.ref = 0;
    id_con.fdb = pll_data.dq_value.d;
    PID_Calc(&id_con);

    id_out_sum += id_con.output;

    //output
    *theta_pll = id_out_sum - PI/2;
    *amplitude_pll = -pll_data.dq_value.q;
}

void PLL_ThreePhase(float *amplitude_pll,float *theta_pll,CT_data_t Sample_in,float theta_pll_in)
{

}