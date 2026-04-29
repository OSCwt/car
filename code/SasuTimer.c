#include "SasuTimer.h"
// ACTIVE_NEW_CAR: 本文件提供当前新车 1ms 控制定时中断 cc60_pit_ch1_isr。
// KEEP_FOR_BUILD: cc60_pit_ch1_isr 是当前控制节拍的中断符号，不能直接删除或移动。
// LEGACY_OLD_CAR: 文件底部保留的模式选择/PS2/ICAR/USB 分支是旧车遗留注释。
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


/**
* @brief        定时器TIM2初始化
* @param
* @ref
* @author       Leo
* @note
**/
void Sasu_TIM_Init(void)
{
    // ACTIVE_NEW_CAR: 新车当前使用 CCU60_CH1 作为 1ms 控制节拍。
    pit_ms_init(CCU60_CH1, 1);
}


/**
* @brief        TIM2定时中断服务
* @param
* @ref
* @author       Leo
* @note
**/
IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // 开启中断嵌套
    pit_clear_flag(CCU60_CH1);

    // ACTIVE_NEW_CAR: 当前新车 1ms 主路径，顺序保持不变。
    PID_debug_Timer();
    PID_debug_Handle();
    RemoteControl_Timer1ms();
    MOTOR_Timer();
    // ACTIVE_NEW_CAR: 舵机周期刷新。
    servo_time();

//    LEGACY_OLD_CAR: 以下旧车按模式调度 PS2/ICAR/GPIO/USB 的逻辑已停用，暂保留作迁移参考。
//    if(modetyp.mode==PID_DEBUG)
//    {
//        if(piddebug.MotorEnable==true)
//          MOTOR_Timer();
//        PID_debug_Timer();
//
//        PS2_Timer();
//
//    }
//    else
//    {
//        if(modetyp.mode==PS2_ICAR || modetyp.mode==ICAR)
//        {
//            GPIO_Timer();           //GPIO外设线程
//            MOTOR_Timer();          //电机控制线程
//            //    SOC_Timer();            //电量计监测线程
//            ICAR_Timer();           //智能车综合处理线程计数器
//            USB_Edgeboard_Timr();   //USB通信线程
//        }
//        if(modetyp.mode==PS2 || modetyp.mode==PS2_ICAR )
//          PS2_Timer();
//    }


}













