#include "main.h"
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

#define YAW_CONTROL_SIGN (-1.0f)

ServoStruct servoStr;

/**
* @brief        舵机控制初始化
* @param
* @ref
* @author       Leo
* @note
**/
void SERVO_Init(void)
{

    pwm_init(SERVO_PIN,50,SERVO_MOTOR_DUTY(90));
    //SERVO_SetPwmValue(servoStr.thresholdMiddle);

    servoStr.thresholdLeft=SERVO_MOTOR_DUTY(SERVO_MOTOR_L_angle);
    servoStr.thresholdRight=SERVO_MOTOR_DUTY(SERVO_MOTOR_R_angle);
    servoStr.thresholdMiddle=SERVO_MOTOR_DUTY(90);
    servoStr.set_angle=0;
    servoStr.imu_angle=0;
    servoStr.angle_PreError=0;
    servoStr.angel_PreDerror=0;
    servoStr.angle=0;
    servoStr.Kp=1;
    servoStr.Ki=0.1;
    servoStr.Kd=0;

}
void servo_set(float angle)
{

    if(angle<SERVO_MOTOR_L_angle)
    {
        angle=SERVO_MOTOR_L_angle;
        servoStr.angle=-50;
    }
    if(angle>SERVO_MOTOR_R_angle)
    {
        angle=SERVO_MOTOR_R_angle;
        servoStr.angle=50;
    }

    pwm_set_duty(servo_pin,(uint32_t)SERVO_MOTOR_DUTY(angle));


}

void angle_calculate(ServoStruct* pp)
{
    float  error,d_error,dd_error;

        error = pp->set_angle - pp->imu_angle;
        d_error = error - pp->angle_PreError;
        dd_error = d_error - pp->angel_PreDerror;

        pp->angle_PreError = error;
        pp->angel_PreDerror = d_error;

        if( ( error < VV_DEADLINE ) && ( error > -VV_DEADLINE ) )
        {
            ;
        }
        else
        {
            pp->angle += (pp -> Kp * d_error + pp -> Ki * error + pp->Kd*dd_error);

        }
}

void servo_time(void)
{
    servoStr.Counter++;

    if(servoStr.Counter>10)
    {
        servoStr.Counter=0;
        servoStr.imu_angle=YAW_CONTROL_SIGN * attitudeDebug.yaw_continuous_deg;
        angle_calculate(&servoStr);
        servoStr.Counter1++;
        if(servoStr.Counter1>10)
        {
            servo_set(90+servoStr.angle);
            servoStr.Counter1=0;
        }
    }

}


///**
//* @brief        舵机输出PWM设置
//* @param        pwm：-20000~20000
//* @ref
//* @author       Leo
//* @note
//**/
//uint32 PWMVALUE=0;
//void SERVO_SetPwmValue(signed int pwm)
//{
//   pwm = 3000 - pwm;  //左→右
//
//    if(pwm < SERVO_PWM_MIN)
//        pwm = SERVO_PWM_MIN;
//    else if(pwm > SERVO_PWM_MAX)
//        pwm = SERVO_PWM_MAX;
//
//    pwm_set_duty(SERVO_PIN,pwm);
//    PWMVALUE=pwm;
////    TIM_SetCompare1(TIM4,pwm);
//}
//
//
///**
//* @brief        舵机输出PWM设置（矫正后）
//* @param        pwm：500~2500
//* @ref
//* @author       Leo
//* @note
//**/
//uint16_t pwm_Servo = 0;
//void SERVO_SetPwmValueCorrect(signed int pwm)
//{
//    pwm = 3000 - pwm;  //左→右
//
//    pwm -= servoStr.thresholdMiddle-SERVO_PWM_MIDDLE; //中值补偿
//
//    uint16_t pwmMax = 3000 - servoStr.thresholdLeft;
//    uint16_t pwmMin = 3000 - servoStr.thresholdRight;
//    if(pwm < pwmMin)
//        pwm = pwmMin;
//    else if(pwm > pwmMax)
//        pwm = pwmMax;
//
//    pwm_Servo = (uint16_t)pwm;
//    pwm_set_duty(SERVO_PIN,pwm);
//    PWMVALUE=pwm;
////    TIM_SetCompare1(TIM4,pwm);
//}
//
///**
//* @brief       舵机角度控制
//* @param
//* @ref
//* @author
//* @note
//**/
//uint16_t ServoPwm = 1500;
//void SERVO_AngleControl(float angle)
//{
//    uint16_t pwm = 1500;
//    angle = -angle;
//    if(angle > SERVO_ANGLE_MAX)
//        angle = SERVO_ANGLE_MAX;
//    else if(angle < -SERVO_ANGLE_MAX)
//        angle = -SERVO_ANGLE_MAX;
//
//    if(angle >= 0)  //右转
//        pwm = (float)angle/SERVO_ANGLE_MAX * (SERVO_PWM_MAX_R-servoStr.thresholdMiddle) + servoStr.thresholdMiddle;     //绝对角度计算
//    else if(angle < 0)      //左转
//        pwm = (float)angle/SERVO_ANGLE_MAX * (servoStr.thresholdMiddle - SERVO_PWM_MAX_L) + servoStr.thresholdMiddle;       //绝对角度计算
//
//    ServoPwm = pwm;
//    SERVO_SetPwmValue(pwm);
//
//}



