#include "Motor.h"

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

MotorStruct motorStr;

static gpio_pin_enum MOTOR_GetDirPin(int motor_index)
{
    return (motor_index == 0) ? MOTOR1_DIR_PIN : MOTOR2_DIR_PIN;
}

static uint8 MOTOR_GetForwardDirLevel(int motor_index)
{
    return (motor_index == 0) ? MOTOR1_FORWARD_DIR_LEVEL : MOTOR2_FORWARD_DIR_LEVEL;
}


/**
* @brief        电机控制初始化
* @param
* @ref
* @author       Leo
* @note
**/
void MOTOR_Init(void)
{
    //PWM-IO初始化
    gpio_init(MOTOR1_DIR_PIN, GPO, MOTOR1_FORWARD_DIR_LEVEL, GPO_PUSH_PULL);
    gpio_init(MOTOR2_DIR_PIN, GPO, MOTOR2_FORWARD_DIR_LEVEL, GPO_PUSH_PULL);
    pwm_init(MOTOR1_PWM_PIN,12000,0);
    pwm_init(MOTOR2_PWM_PIN,12000,0);
    MOTOR_SetPwmValue(MOTOR1_PWM_PIN,0,0);
    MOTOR_SetPwmValue(MOTOR2_PWM_PIN,0,1);
    //电机模型初始化
    motorStr.EncoderLine = 1024.0f;                        // 电机轴A相每圈脉冲数（方向计数口径1x）
    motorStr.ReductionRatio = 30;
//    motorStr.ReductionRatio = 2.7f;                         //电机减速比
    motorStr.EncoderValue_L = 0;
    motorStr.EncoderValue_R = 0;
    motorStr.DiameterWheel = 0.068f;//68cm                  //轮子直径:m
    motorStr.CloseLoop = CLOSELOOP_DEFAULT;                              //默认闭环模式
    motorStr.PWM_L_value=0;
    motorStr.PWM_R_value=0;
    motorStr.samlpe=0;

}


/**
* @brief        电机输出PWM设置
* @param        pwm：-2000~2000
* @ref
* @author       Leo
* @note
**/
void MOTOR_SetPwmValue(pwm_channel_enum  MOTOR_PWM_PIN,signed int pwm,int i)
{
    signed int pwm_abs = (pwm >= 0) ? pwm : -pwm;
    uint8 dir_level = MOTOR_GetForwardDirLevel(i);

    if(pwm < 0)
    {
        dir_level = !dir_level;
    }

    if(pwm_abs > MOTOR_PWM_MAX)
    {
        pwm_abs = MOTOR_PWM_MAX;
    }

    gpio_set_level(MOTOR_GetDirPin(i), dir_level);
    pwm_set_duty(MOTOR_PWM_PIN, pwm_abs);
    if(i==0)
    {
        motorStr.PWM_L_value=pwm;

    }
    else
    {
        motorStr.PWM_R_value=pwm;

    }


}


/**
* @brief        电机闭环速控
* @param        speed：速度m/s
* @ref
* @author       Leo
* @note
**/
void MOTOR_ControlLoop(float speed)
{
    if(speed > MOTOR_SPEED_MAX)
        speed = MOTOR_SPEED_MAX;
    else if(speed < -MOTOR_SPEED_MAX)
        speed = -MOTOR_SPEED_MAX;

    L_motor.vi_Ref = (float)(speed*MOTOR_CONTROL_CYCLE * motorStr.EncoderLine * motorStr.ReductionRatio/ motorStr.DiameterWheel / PI );
    R_motor.vi_Ref = (float)(speed*MOTOR_CONTROL_CYCLE * motorStr.EncoderLine * motorStr.ReductionRatio/ motorStr.DiameterWheel / PI );

    MOTOR_SetPwmValue(MOTOR1_PWM_PIN,PID_MoveCalculate(&L_motor),0);
    MOTOR_SetPwmValue(MOTOR2_PWM_PIN,PID_MoveCalculate(&R_motor),1);

}


/**
* @brief        电机控制线程
* @param
* @ref
* @author       Leo
* @note
**/

void MOTOR_Timer(void)
{
    motorStr.Counter++;
    if(motorStr.Counter >= 10)                              //速控:10ms
    {
        ENCODER_RevSample();                                //编码器采样
        //记得将piddebug.MotorEnable改为true，要不然车不跑
        if(piddebug.MotorEnable)
        {
            MOTOR_ControlLoop(icarStr.SpeedSet);
        }

        motorStr.Counter = 0;
//    if(modetyp.mode==PID_DEBUG)
//    {
//        if(piddebug.MotorEnable)
//        {
//            if(motorStr.CloseLoop)
//            MOTOR_ControlLoop(icarStr.SpeedSet);
//            else
//           MOTOR_SetPwmValue(0);//用于查看卡尔曼滤波效果
//        }
    }
    else
    {
//        if(icarStr.sprintEnable || usbStr.connected )        //通信连接或电机测试才开启闭环（保护+省电）
//        {
//            if(motorStr.CloseLoop)
//            {
//                MOTOR_ControlLoop(icarStr.SpeedSet);        //闭环速控
//            }
//            else//开环百分比控制
//            {
//                if(icarStr.SpeedSet > 100)
//                    icarStr.SpeedSet = 100;
//                else if(icarStr.SpeedSet < -100)
//                    icarStr.SpeedSet = -100;
//                signed int speedRate = MOTOR_PWM_MAX/100.f*icarStr.SpeedSet; //开环：百分比%
//
//                MOTOR_SetPwmValue(speedRate);       //开环速控
//            }
//        }
//        else
//        {
//            MOTOR_SetPwmValue(0);
//        }
//     }


    }
}

