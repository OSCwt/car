#ifndef _PID_DEBUG_H__
#define _PID_DEBUG_H__

#include "main.h"


typedef struct{
    uint16_t Counter;                   //计数器
    bool pidset;
    bool pidsend;
    float Kp;
    float Ki;
    float Kd;
    float SpeedSet;
    bool MotorEnable;
    signed int pwm;

}PIDDebugStruct;

extern PIDDebugStruct piddebug;

void PID_debug_get_data(float p,float i,float d);
void PID_get_target(float speed);

void PID_debug_Init(void);
void PID_debug_Handle(void);
void PID_debug_Timer(void);
#endif
