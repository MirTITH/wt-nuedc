#include "Coordinate_Trans.h"
#include "math.h"
#include "main.h"

void CT_Refresh_abc(CT_data_t *CTdata,float a,float b,float c,float theta)
{
    CTdata->AlphaBeta_value.alpha 
    = sqrt(2.0f/3.0f) * ((1)*a - (0.5f)*b - (0.5f)*c);
     CTdata->AlphaBeta_value.beta 
    = sqrt(2.0f/3.0f) * ((0)*a + (sqrt(0.75f))*b - (sqrt(0.75f))*c);

    CTdata->dq_value.d 
    = (2.0f/3.0f) * ((cosf(theta))*a + (cosf(theta-2*PI/3))*b + (cosf(theta+2*PI/3))*c);
     CTdata->dq_value.q 
    = -(2.0f/3.0f) * ((sinf(theta))*a + (sinf(theta-2*PI/3))*b - (sinf(theta+2*PI/3))*c);
}

void CT_Refresh_AlphaBeta(CT_data_t *CTdata,float alpha,float beta,float theta)
{
    CTdata->abc_value.a 
    = sqrt(2.0f/3.0f) * ((1)*alpha - (0)*beta);
    CTdata->abc_value.b
    = sqrt(2.0f/3.0f) * (-(0.5f)*alpha + (sqrt(0.75f))*beta);
    CTdata->abc_value.c
    = sqrt(2.0f/3.0f) * (-(0.5f)*alpha - (sqrt(0.75f))*beta);

    CTdata->dq_value.d 
    = (cosf(theta))*alpha + (sinf(theta))*beta;
     CTdata->dq_value.q 
    = -(sinf(theta))*alpha + (cosf(theta))*beta;
}

void CT_Refresh_dq(CT_data_t *CTdata,float d,float q,float theta)
{
    CTdata->abc_value.a 
    = cosf(theta)*d - sinf(theta)*q;
    CTdata->abc_value.b
    = cosf(theta-2*PI/3)*d - sinf(theta-2*PI/3)*q;
    CTdata->abc_value.c
   = cosf(theta+2*PI/3)*d - sinf(theta+2*PI/3)*q;

    CTdata->AlphaBeta_value.alpha 
    = (cosf(theta))*d - (sinf(theta))*q;
     CTdata->AlphaBeta_value.beta 
    = (sinf(theta))*d + (cosf(theta))*q;
}

void CT_get_AlphaBeta(CT_data_t *CTdata,float a,float b,float c)
{
    CTdata->AlphaBeta_value.alpha 
    = sqrt(2.0f/3.0f) * ((1)*a - (0.5f)*b - (0.5f)*c);
     CTdata->AlphaBeta_value.beta 
    = sqrt(2.0f/3.0f) * ((0)*a + (sqrt(0.75f))*b - (sqrt(0.75f))*c);
}

void CT_get_abc(CT_data_t *CTdata,float alpha,float beta)
{
    CTdata->abc_value.a 
    = sqrt(2.0f/3.0f) * ((1)*alpha - (0)*beta);
    CTdata->abc_value.b
    = sqrt(2.0f/3.0f) * (-(0.5f)*alpha + (sqrt(0.75f))*beta);
    CTdata->abc_value.c
    = sqrt(2.0f/3.0f) * (-(0.5f)*alpha - (sqrt(0.75f))*beta);
}