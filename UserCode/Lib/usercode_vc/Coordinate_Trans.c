#include "Coordinate_Trans.h"
#include "math.h"
#include "main.h"
#include "arm_math.h"

void CT_Refresh_abc(CT_data_t *CTdata,float a,float b,float c,float theta)
{
    CTdata->AlphaBeta_value.alpha 
    = sqrtf(2.0f/3.0f) * ((1)*a - (0.5f)*b - (0.5f)*c);
     CTdata->AlphaBeta_value.beta 
    = sqrtf(2.0f/3.0f) * ((0)*a + (sqrtf(0.75f))*b - (sqrtf(0.75f))*c);

    CTdata->dq_value.d 
    = (2.0f/3.0f) * ((arm_cos_f32(theta))*a + (arm_cos_f32(theta-2*PI/3))*b + (arm_cos_f32(theta+2*PI/3))*c);
     CTdata->dq_value.q 
    = -(2.0f/3.0f) * ((arm_sin_f32(theta))*a + (arm_sin_f32(theta-2*PI/3))*b - (arm_sin_f32(theta+2*PI/3))*c);
}

void CT_Refresh_AlphaBeta(CT_data_t *CTdata,float alpha,float beta,float theta)
{
    CTdata->abc_value.a 
    = sqrtf(2.0f/3.0f) * ((1)*alpha - (0)*beta);
    CTdata->abc_value.b
    = sqrtf(2.0f/3.0f) * (-(0.5f)*alpha + (sqrtf(0.75f))*beta);
    CTdata->abc_value.c
    = sqrtf(2.0f/3.0f) * (-(0.5f)*alpha - (sqrtf(0.75f))*beta);

    CTdata->dq_value.d 
    = (arm_cos_f32(theta))*alpha + (arm_sin_f32(theta))*beta;
     CTdata->dq_value.q 
    = -(arm_sin_f32(theta))*alpha + (arm_cos_f32(theta))*beta;
}

void CT_Refresh_dq(CT_data_t *CTdata,float d,float q,float theta)
{
    CTdata->abc_value.a 
    = arm_cos_f32(theta)*d - arm_sin_f32(theta)*q;
    CTdata->abc_value.b
    = arm_cos_f32(theta-2*PI/3)*d - arm_sin_f32(theta-2*PI/3)*q;
    CTdata->abc_value.c
    = arm_cos_f32(theta+2*PI/3)*d - arm_sin_f32(theta+2*PI/3)*q;

    CTdata->AlphaBeta_value.alpha 
    = (arm_cos_f32(theta))*d - (arm_sin_f32(theta))*q;
     CTdata->AlphaBeta_value.beta 
    = (arm_sin_f32(theta))*d + (arm_cos_f32(theta))*q;
}

void CT_get_AlphaBeta(CT_data_t *CTdata,float a,float b,float c)
{
    CTdata->AlphaBeta_value.alpha 
    = sqrtf(2.0f/3.0f) * ((1)*a - (0.5f)*b - (0.5f)*c);
     CTdata->AlphaBeta_value.beta 
    = sqrtf(2.0f/3.0f) * ((0)*a + (sqrtf(0.75f))*b - (sqrtf(0.75f))*c);
}

void CT_get_abc(CT_data_t *CTdata,float alpha,float beta)
{
    CTdata->abc_value.a 
    = sqrtf(2.0f/3.0f) * ((1)*alpha - (0)*beta);
    CTdata->abc_value.b
    = sqrtf(2.0f/3.0f) * (-(0.5f)*alpha + (sqrtf(0.75f))*beta);
    CTdata->abc_value.c
    = sqrtf(2.0f/3.0f) * (-(0.5f)*alpha - (sqrtf(0.75f))*beta);
}