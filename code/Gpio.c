/*
 * text.c
 *
 *  创建时间: 2023年3月19日
 *      作者: 画家
 */
#include "Gpio.h"
// LEGACY_OLD_CAR: 旧车按键、LED、蜂鸣器外设线程，新车当前主控制路径不优先依赖。
// KEEP_FOR_BUILD: 本文件仍参与编译，且旧车自检/提示音逻辑有历史引用，暂不删除。
GpioStruct gpioStr;
BuzzerStruct buzzerStr;

void GPIO_Initialize(void)
{
    // LEGACY_OLD_CAR: 旧车按键、LED、蜂鸣器初始化入口。
    //蜂鸣器IO初始化
    gpio_init(P33_10,GPO,GPIO_LOW,GPO_PUSH_PULL);

    //LED灯IO初始化
    gpio_init(P20_9,GPO,GPIO_HIGH,GPO_PUSH_PULL);
    gpio_init(P21_4,GPO,GPIO_HIGH,GPO_PUSH_PULL);

    //按键IO初始化
    gpio_init(KEY1_PIN,GPI,GPIO_HIGH,GPI_PULL_UP);//k1
    gpio_init(KEY2_PIN,GPI,GPIO_HIGH,GPI_PULL_UP);//k2





    //BuzzerInit
    buzzerStr.Counter = 0;
    buzzerStr.Cut = 0;
    buzzerStr.Enable = false;
    buzzerStr.Times = 0;

    gpioStr.Key1Flag=KEY_UP;
    gpioStr.Key2Flag=KEY_UP;
    gpioStr.Key2Press=false;
    gpioStr.KeyPress=false;

}

void GPIO_Timer(void)
{
    // LEGACY_OLD_CAR: 旧车外设 1ms 线程，当前 SasuTimer.c 中对应调用已在旧模式分支里注释。
    //蜂鸣器控制
    if(buzzerStr.Enable)
    {
        buzzerStr.Counter++;

        if(buzzerStr.Cut<buzzerStr.Counter)
            buzzerStr.Counter = buzzerStr.Cut;
    }

    //LED闪烁
    gpioStr.CounterLed++;


    //按键扫描
    if(gpio_get_level(KEY1_PIN)==0 && gpioStr.Key1Flag==KEY_UP )     //按键按下
    {
        gpioStr.KeyPress = true;
        GPIO_BuzzerEnable(BuzzerDing);
        gpioStr.Key1Flag=KEY_DOWN;
        gpio_toggle_level(P21_4);
    }
    else if(gpio_get_level(KEY1_PIN)==1 && gpioStr.Key1Flag==KEY_DOWN) //按键弹起
    {
        gpioStr.KeyPress = false;
        gpioStr.Key1Flag=KEY_UP;
    }

    if(gpio_get_level(KEY2_PIN)==0 && gpioStr.Key2Flag==KEY_UP )     //按键按下
    {
        gpioStr.Key2Press = true;
        GPIO_BuzzerEnable(BuzzerDing);
        gpioStr.Key2Flag=KEY_DOWN;
        gpio_toggle_level(P21_4);
    }
    else if(gpio_get_level(KEY2_PIN)==1 && gpioStr.Key2Flag==KEY_DOWN) //按键弹起
    {
        gpioStr.Key2Press = false;
        gpioStr.Key2Flag=KEY_UP;
    }

}
void GPIO_Handle(void)
{
    // LEGACY_OLD_CAR: 旧车外设后台处理入口，当前新车主循环未直接调用。
    //蜂鸣器控制
    if(buzzerStr.Enable && !buzzerStr.Silent)
    {
        if(buzzerStr.Times<=0)
        {
            BUZZER_OFF;
            buzzerStr.Enable = false;
            return;
        }
        else if(buzzerStr.Cut<=buzzerStr.Counter)
        {
            BUZZER_REV;
            buzzerStr.Times--;
            buzzerStr.Counter = 0;
        }
    }
    else
        BUZZER_OFF;

    //LED控制
    if(gpioStr.CounterLed > 100)        //100ms
    {
        LED_REV;
        gpioStr.CounterLed = 0;
    }
}
void GPIO_BuzzerEnable(BuzzerEnum buzzer)
{
    // KEEP_FOR_BUILD: 旧车自检代码中仍保留对该接口的历史引用，隔离前不要删除。
    switch(buzzer)
    {
        case BuzzerOk:
            buzzerStr.Cut = 70;         //70ms
            buzzerStr.Enable = true;
            buzzerStr.Times = 4;
            break;

        case BuzzerWarnning:
            buzzerStr.Cut = 100;        //100ms
            buzzerStr.Enable = true;
            buzzerStr.Times = 10;
            break;

        case BuzzerSysStart:
            buzzerStr.Cut = 60;         //60ms
            buzzerStr.Enable = true;
            buzzerStr.Times = 6;
            break;

        case BuzzerDing:
            buzzerStr.Cut = 30;         //30ms
            buzzerStr.Enable = true;
            buzzerStr.Times = 2;
            break;

        case BuzzerFinish:
            buzzerStr.Cut = 200;        //200ms
            buzzerStr.Enable = true;
            buzzerStr.Times = 6;
            break;
    }

    buzzerStr.Counter = 0;
}
