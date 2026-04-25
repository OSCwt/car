#ifndef __SERVO_H__
#define __SERVO_H__

#include "main.h"

#define servo_pin ATOM1_CH1_P33_9
#define SERVO_MOTOR_L_angle 50
#define SERVO_MOTOR_R_angle 140
#define SERVO_MOTOR_FREQ  50
#define SERVO_MOTOR_DUTY(x)         ((float)PWM_DUTY_MAX/(1000.0/(float)SERVO_MOTOR_FREQ)*(0.5+(float)(x)/90.0))


#define  SERVO_PWM_MAX                  2500                        //舵机方向最大PWM：180°
#define  SERVO_PWM_MIN                  500                         //舵机方向最小PWM：0°
#define  SERVO_PWM_MAX_L                1150                        //舵机左向转角最大值PWM
#define  SERVO_PWM_MAX_R                1850                        //舵机右向转角最大值PWM
#define  SERVO_PWM_MIDDLE               1500                        //舵机中值PWM

#define  SERVO_ANGLE_MAX                38.0f                       //舵机

#define SERVO_PIN ATOM1_CH1_P33_9

extern uint32 PWMVALUE;



/**
* @brief    舵机相关
**/
typedef struct
{
    uint16_t thresholdMiddle;                   //舵机中值PWM
    uint16_t thresholdLeft;                     //舵机左向转角最大值PWM
    uint16_t thresholdRight;                    //舵机右向转角最大值PWM
    uint16_t Counter;                           //线程计数器
    uint16_t Counter1;                           //线程计数器
    float set_angle;                              //设定值
    float imu_angle;                              //陀螺仪数值
    float angle_PreError;                        //角度误差,vi_Ref - vi_FeedBack
    float angel_PreDerror;                        //角度误差之差，d_error-PreDerror;
    float Kp;
    float Ki;
    float Kd;
    float angle;                                  //舵机数值,我希望这个数值是左偏或者右偏多少度
}ServoStruct;

extern ServoStruct servoStr;

void SERVO_Init(void);
void servo_set(float angle);
void angle_calculate(ServoStruct* pp);
void servo_time(void);


void SERVO_SetPwmValue(signed int pwm);
void SERVO_SetPwmValueCorrect(signed int pwm);
void SERVO_AngleControl(float angle);
#endif

//===========================================  文件结束  ===========================================//


