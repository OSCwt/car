#ifndef __GPIO_H__
#define __GPIO_H__

// LEGACY_OLD_CAR: 旧车按键、LED、蜂鸣器模块声明。
// KEEP_FOR_BUILD: 该头文件由 main.h 聚合 include，暂不能直接删除。

/*-----------------------------------------  I N C L U D E S  -----------------------------------------*/

#include "main.h"

/*---------------------------------------  D E F I N I T I O N  ---------------------------------------*/


// LEGACY_OLD_CAR: 旧车 LED/蜂鸣器/按键引脚宏，当前新车主路径暂不优先使用。
#define LED_ON          gpio_set_level(P20_9,0)
#define LED_OFF         gpio_set_level(P20_9,1)
#define LED_REV         gpio_set_level(P20_9,1)

#define BUZZER_ON        gpio_set_level(P33_10,1)
#define BUZZER_OFF       gpio_set_level(P33_10,0)
#define BUZZER_REV       gpio_set_level(P33_10,1)

#define KEY1_PIN P20_7
#define KEY2_PIN P20_6

#define KEY_DOWN 1
#define KEY_UP 0

typedef enum
{
    BuzzerOk = 0,                       //确认提示音
    BuzzerWarnning,                     //报警提示音
    BuzzerSysStart,                     //开机提示音
    BuzzerDing,                         //叮=====(￣▽￣*)
    BuzzerFinish,                       //结束提示音
}BuzzerEnum;


/**
* @brief    按键和LED相关
**/
// LEGACY_OLD_CAR: 旧车按键/LED 状态结构，保留用于兼容旧外设线程。
typedef struct
{
    bool KeyPress;                      //按键输入-B
    bool Key2Press;
    uint8_t Key1Flag;
    uint8_t Key2Flag;
    uint16_t CounterLed;                //LED闪烁计数器
}GpioStruct;


/**
* @brief    蜂鸣器相关
**/
// LEGACY_OLD_CAR: 旧车蜂鸣器状态结构，后续可随旧自检逻辑一起条件编译隔离。
typedef struct
{
    bool Enable;                        //使能标志
    uint16_t Times;                     //鸣叫次数
    uint16_t Counter;                   //计数器
    uint16_t Cut;                       //间隔时间
    bool Silent;                        //是否禁用蜂鸣器
}BuzzerStruct;

extern GpioStruct gpioStr;
extern BuzzerStruct buzzerStr;

void GPIO_Initialize(void);
void GPIO_Timer(void);
void GPIO_Handle(void);
void GPIO_BuzzerEnable(BuzzerEnum buzzer);


#endif

//===========================================  文件结束  ===========================================//

