 #include "Pid.h"
/*
********************************************************************************************************
*                                               示例代码
*                                             示例代码
*
*                             (c) Copyright 2021; SaiShu.Lcc.; Leo
*                                 版权所属[北京赛曙科技有限公司]
*
*               本代码仅供内部学习使用，请勿商用(开源学习,请勿商用).
*               本代码适配对应硬件电路板(代码使用CarDo智控板),
*               具体细节请咨询专业人员(欢迎联系我们).
*********************************************************************************************************
*/

PIDStruct L_motor;
PIDStruct R_motor;


/**
* @brief        PID参数初始化
* @param
* @ref
* @author       Leo
* @note
**/
void PID_Init(void)
{
    L_motor.vi_Ref = 0 ;
    L_motor.vi_FeedBack = 0 ;
    L_motor.vi_PreError = 0 ;
    L_motor.vi_PreDerror = 0 ;
    L_motor.v_Kp = 1.2f;
    L_motor.v_Ki = 1.2f;
    L_motor.v_Kd = 0;
    L_motor.vl_PreU = 0;

    R_motor.vi_Ref = 0 ;
    R_motor.vi_FeedBack = 0 ;
    R_motor.vi_PreError = 0 ;
    R_motor.vi_PreDerror = 0 ;
    R_motor.v_Kp = 1.2f;
    R_motor.v_Ki = 1.2f;
    R_motor.v_Kd =0;
    R_motor.vl_PreU = 0;

}

/**
* @brief
* @param
* @ref
* @author
**/
signed int PID_MoveCalculate(PIDStruct *pp)
{

    if (pp->vi_Ref == 0)
        {
            pp->vl_PreU = 0;         // 强制清空之前累加的 PWM 输出
            pp->vi_PreError = 0;     // 清空历史误差，防止下次起步瞬间暴走
            pp->vi_PreDerror = 0;    // 清空历史误差变化率
            return 0;                // 直接返回 0，让电机立刻停止
        }
    float error, d_error, dd_error;
    float res_inc = 0;

    // 1. 计算误差
    error = pp->vi_Ref - pp->vi_FeedBack;
    d_error = error - pp->vi_PreError;
    dd_error = d_error - pp->vi_PreDerror;

    // 2. 更新历史误差
    pp->vi_PreError = error;
    pp->vi_PreDerror = d_error;


    if( ( error <= VV_DEADLINE ) && ( error >= -VV_DEADLINE ) )
    {
        res_inc = 0;
    }
    else
    {

        res_inc = (pp->v_Kp * d_error + pp->v_Ki * error + pp->v_Kd * dd_error);
    }

    if (pp->vl_PreU >= MOTOR_PWM_MAX && res_inc > 0)
    {
        res_inc = 0;
    }
    else if (pp->vl_PreU <= MOTOR_PWM_MIN && res_inc < 0)
    {
        res_inc = 0;
    }


    pp->vl_PreU += res_inc;

    if( pp->vl_PreU >= MOTOR_PWM_MAX ) pp->vl_PreU = MOTOR_PWM_MAX;
    else if( pp->vl_PreU <= MOTOR_PWM_MIN ) pp->vl_PreU = MOTOR_PWM_MIN;

    return (signed int)(pp->vl_PreU);
}
