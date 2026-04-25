#ifndef __SERVO_H__
#define __SERVO_H__

#include "main.h"

#define SERVO_PIN                       ATOM1_CH1_P33_9
#define servo_pin                       SERVO_PIN

#define SERVO_MOTOR_L_angle             50.0f
#define SERVO_MOTOR_MID_angle           90.0f
#define SERVO_MOTOR_R_angle             140.0f
#define SERVO_CENTER_TRIM_DEG           1.75f
#define SERVO_CENTER_ANGLE              (SERVO_MOTOR_MID_angle + SERVO_CENTER_TRIM_DEG)
#define SERVO_MOTOR_FREQ                50
#define SERVO_UPDATE_PERIOD_MS          20u

#define SERVO_MOTOR_DUTY(x)             ((float)PWM_DUTY_MAX / (1000.0f / (float)SERVO_MOTOR_FREQ) * (0.5f + (float)(x) / 90.0f))

#define SERVO_PWM_MAX                   2500
#define SERVO_PWM_MIN                   500
#define SERVO_PWM_MAX_L                 1150
#define SERVO_PWM_MAX_R                 1850
#define SERVO_PWM_MIDDLE                1500

#define SERVO_ANGLE_MAX                 38.0f

extern uint32 PWMVALUE;

typedef struct
{
    uint16_t thresholdMiddle;
    uint16_t thresholdLeft;
    uint16_t thresholdRight;
    uint16_t Counter;
    uint16_t Counter1;

    float set_angle;
    float imu_angle;
    float angle_PreError;
    float angel_PreDerror;
    float Kp;
    float Ki;
    float Kd;
    float angle;

    float target_angle;
    float output_angle;
    float offset_angle;
    uint32 output_duty;
} ServoStruct;

extern ServoStruct servoStr;

void SERVO_Init(void);
void servo_set(float angle);
void angle_calculate(ServoStruct* pp);
void servo_time(void);

void SERVO_SetAngle(float angle_deg);
void SERVO_SetOffset(float offset_deg);
void SERVO_Center(void);

void SERVO_SetPwmValue(signed int pwm);
void SERVO_SetPwmValueCorrect(signed int pwm);
void SERVO_AngleControl(float angle);

#endif
