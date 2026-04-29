/*
********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2019; SaiShu.Lcc.; Leo
*
*               本代码仅供内部学习使用，请勿商用.
*               本代码适配对应硬件电路板, 
*               具体细节请咨询专业人员.
*********************************************************************************************************
*/

#ifndef __MAIN_H__
#define __MAIN_H__

// KEEP_FOR_BUILD: main.h 是当前业务模块聚合头，许多 .c/.h 通过它间接获得类型、宏和函数声明。
// ACTIVE_NEW_CAR: 新车主路径仍依赖 Motor、Encoder、Servo、RemoteControl、SasuTimer、PID_debug、top 等 include。
// LEGACY_OLD_CAR: 部分 include 来自旧车迁移保留，整理前不要为了“看起来干净”直接删除。

//typedef enum { false = 0,true = 1} bool;


/************************ System *****************************/
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "zf_common_headfile.h"

/*************************USER_MODULES***********************************************/
// KEEP_FOR_BUILD: 以下 include 同时承担编译依赖聚合功能，删除前必须确认工程配置、全局变量和 ISR 依赖。
#include "isr_config.h"
#include "Delay.h"
// ACTIVE_NEW_CAR: 编码器、电机、PID、定时器、舵机、遥控和显示为当前新车主路径。
#include "Encoder.h"
// LEGACY_OLD_CAR / KEEP_FOR_BUILD: Gpio 当前偏旧车外设线程，仍被聚合 include 保留。
#include "Gpio.h"
#include "Pid.h"
#include "Motor.h"
// LEGACY_OLD_CAR: Flash/Soc/Icar_Sys/Rgb/Usb/ps2/ModeCtr 等旧车模块当前未启用。
//#include "Flash.h"
//#include "Soc.h"
#include "SasuTimer.h"
// KEEP_FOR_BUILD: Icar.h 提供 icarStr 状态结构，新车速度控制仍依赖该全局状态。
#include "Icar.h"
//#include "Icar_Sys.h"
#include "Servo.h"
//#include "Rgb.h"
//#include "Usb.h"
#include "protocol.h"
#include "PID_debug.h"
//#include "ps2.h"
//#include "ModeCtr.h"
#include "Kalman_Filter.h"

#include "top.h"
#include "attitude_solution.h"
#include "RemoteControl.h"

#define MODE_ENABLE (1)//模式选择使能，值0时不可选择模式，只执行比赛模式
#define KALMAN_FILTER (1) //1：使能卡尔曼滤波
#define CLOSELOOP_DEFAULT (true) //true:默认初始闭环  false:默认开环

#define UART_INDEX              (DEBUG_UART_INDEX   )                           // 默认 UART_0
#define UART_BAUDRATE           (DEBUG_UART_BAUDRATE)                           // 默认 115200
#define UART_TX_PIN             (DEBUG_UART_TX_PIN  )                           // 默认 UART0_TX_P14_0
#define UART_RX_PIN             (DEBUG_UART_RX_PIN  )                           // 默认 UART0_RX_P14_1

typedef union 
{
	uint8_t U8_Buff[2];
	uint16_t U16;
	int16_t S16;
}Bint16_Union;
	
typedef union 
{
	uint8_t U8_Buff[4];
	float Float;
    unsigned long U32;
}Bint32_Union;

#define u8 uint8
#define u16 uint16
#define u32 uint32

//typedef unsigned short u16;
//typedef unsigned int u32;

#endif


