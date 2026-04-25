#ifndef __PWM_H__
#define __PWM_H__

/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "main.h"

/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/

#define  MOTOR_PWM_MAX              (PWM_DUTY_MAX*0.2)        //OCR=95%,禁止满占空比输出，造成MOS损坏
#define  MOTOR_PWM_MIN              0       //OCR=95%
#define  MOTOR_SPEED_MAX            10.0f       //电机最大转速(m/s) (0.017,8.04)
//#define  PI                         3.141593f   //π
#define  MOTOR_CONTROL_CYCLE        0.01f       //电机控制周期T：10ms


#define MOTOR1_DIR_PIN              P02_4
#define MOTOR1_PWM_PIN              ATOM1_CH5_P02_5
#define MOTOR2_DIR_PIN              P02_6
#define MOTOR2_PWM_PIN              ATOM1_CH7_P02_7

#define MOTOR1_FORWARD_DIR_LEVEL    (1)
#define MOTOR2_FORWARD_DIR_LEVEL    (1)

#define USE_NO_DIR_DRIVER (1)  //置1时，使用不带方向的电机驱动

/**
* @brief    电机相关
**/
typedef struct
{
    float ReductionRatio ;                      //电机减速比
    float EncoderLine ;                         // 电机轴A相每圈脉冲数（方向计数口径1x）
    signed int samlpe;
    signed int EncoderValue_L;                    //编码器实时速度
    signed int EncoderValue_R;                    //编码器实时速度
    float DiameterWheel;                        //轮子直径：mm
    bool CloseLoop;                             //开环模式
    uint16_t Counter;                           //线程计数器
    signed int PWM_L_value;                     //左电机带符号控制量
    signed int PWM_R_value;                     //右电机带符号控制量

}MotorStruct;


extern MotorStruct motorStr;


void MOTOR_Init(void);
void MOTOR_SetPwmValue(pwm_channel_enum  MOTOR_PWM_PIN,signed int pwm,int i);
void MOTOR_ControlLoop(float speed);
void MOTOR_Timer(void);


//===========================================  文件结束  ===========================================//
#endif


