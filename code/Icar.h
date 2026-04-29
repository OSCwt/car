#ifndef __SMARTCAR_H__
#define __SMARTCAR_H__

// KEEP_FOR_BUILD: Icar.h 声明的 IcarStruct / icarStr 仍被新车电机、编码器、遥控和显示路径依赖。
// LEGACY_OLD_CAR: 旧车自检、冲刺、按键等接口已大多注释，暂作为迁移参考保留。

/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/
#include "main.h"


/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/

/**
* @brief    智能车自检步骤
**/
// LEGACY_OLD_CAR: 旧车自检步骤枚举，当前新车主路径未启用。
//typedef enum
//{
//    Selfcheck_None = 0,             //开始测试
//    Selfcheck_MotorA,               //电机正转启动
//    Selfcheck_MotorB,               //电机正转采样
//    Selfcheck_MotorC,               //电机反转启动
//    Selfcheck_MotorD,               //电机反转采样
//    Selfcheck_MotorE,               //电机闭环正传启动
//    Selfcheck_MotorF,               //电机闭环正传采样
//    Selfcheck_MotorG,               //电机闭环反转启动
//    Selfcheck_MotorH,               //电机闭环反转采样
//    Selfcheck_ServoA,               //舵机测试A
//    Selfcheck_Com,                  //通信测试
//    Selfcheck_Buzzer,               //蜂鸣器测试
//    Selfcheck_RgbLed,               //灯效测试
//    Selfcheck_Key,                  //按键测试
//    Selfcheck_Finish                //测试完成
//}SelfcheckEnum;


/**
* @brief    智能车相关
**/
// ACTIVE_NEW_CAR: SpeedSet / SpeedFeedback_L / SpeedFeedback_R 当前仍作为新车速度控制和显示状态使用。
// KEEP_FOR_BUILD: 其余旧字段暂不拆分，避免破坏旧模块编译依赖。
typedef struct                              //[智能车驱动主板]
{
    float Voltage;                          //电池电压
    uint8_t Electricity;                    //电池电量百分比：0~100
    float SpeedSet;                         //电机目标速度：m/s
    float SpeedFeedback_L;                    //电机模型实测速度：m/s
    float SpeedFeedback_R;
    float SpeedMaxRecords;                  //测试记录最高速
    uint16_t ServoPwmSet;                   //舵机PWM设置

    uint16_t counterKeyA;                   //按键模式A计数器
    bool keyPressed;                        //按键按下
    bool sprintEnable;                      //闭环冲刺使能
    uint16_t counterSprint;                 //闭环冲刺时间
    uint16_t errorCode;                     //错误代码

    bool selfcheckEnable;                   //智能车自检使能
    uint16_t counterSelfcheck;              //自检计数器
    uint8_t timesSendStep;                  //发送超时数据次数
    uint16_t counterModuleCheck;            //自检计数器
//    SelfcheckEnum selfcheckStep;            //自检步骤
    uint8_t speedSampleStep;                //速度采样步骤

}IcarStruct;


extern IcarStruct icarStr;


// LEGACY_OLD_CAR: 以下旧车 ICAR 接口当前未启用，后续可考虑条件编译隔离。
//void ICAR_Init(void);
//void ICAR_Timer(void);
//void ICAR_Handle(void);
//void ICAR_Reboot(void);

//自检相关

//void ICAR_Selfcheck(void);
//void ICAR_SelfcheckControl(uint8_t step);
//float ICAR_SpeedSample(void);
//bool ICAR_ServoCheck(void);
//bool ICAR_BuzzerCheck(void);
//void ICAR_RgbCheck(void);
//void ICAR_SortFloat(float *buff, uint8_t start, uint8_t len);
//float CMATH_AbsFloat(float data);


#endif

//===========================================  文件结束  ===========================================//


