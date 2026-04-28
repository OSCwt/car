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


//typedef enum { false = 0,true = 1} bool;


/************************ System *****************************/
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "zf_common_headfile.h"

/*************************USER_MODULES***********************************************/
#include "isr_config.h"
#include "Delay.h"
#include "Encoder.h"
#include "Gpio.h"
#include "Pid.h"
#include "Motor.h"
//#include "Flash.h"
//#include "Soc.h"
#include "SasuTimer.h"
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


