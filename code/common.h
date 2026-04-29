/*
 * common.h
 *
 *  创建时间: 2026年3月23日
 *      作者: 22563
 */

#ifndef CODE_COMMON_H_
#define CODE_COMMON_H_

// LEGACY_OLD_CAR: common.h 保留旧车早期电机、编码器和调试接口定义。
// KEEP_FOR_BUILD: zf_common_headfile.h 会间接 include 本文件，删除或大幅改动可能导致编译链路报错。

//电机
// LEGACY_OLD_CAR: 旧车早期电机引脚宏，当前新车电机主路径使用 Motor.h 中的 MOTOR1/2_* 宏。
#define PWM_L  ATOM1_CH0_P21_2
#define PWM_R  ATOM1_CH3_P10_3

#define motor_L_F (P21_5)
#define motor_L_B (P21_4)

#define motor_R_F (P02_5)
#define motor_R_B (P02_4)


//编码器
// LEGACY_OLD_CAR: 旧/过渡期编码器资源宏，当前新车编码器主路径以 Encoder.h 为准。
#define ENCODER_1                   (TIM4_ENCODER)
#define ENCODER_1_A                 (TIM4_ENCODER_CH1_P02_8)
#define ENCODER_1_B                 (TIM4_ENCODER_CH2_P00_9)

#define ENCODER_3                   (TIM5_ENCODER)
#define ENCODER_3_A                 (TIM5_ENCODER_CH1_P10_3)
#define ENCODER_3_B                 (TIM5_ENCODER_CH2_P10_1)


// LEGACY_OLD_CAR: 以下接口为旧车早期辅助函数，暂保留声明以维持工程兼容。
void motor_init(void);
void motor_start(int L_speed,int R_speed);
void set_motor_enable_l(void);
void set_motor_enable_r(void);
void set_motor_disable(void);
void set_motor_enable(void);


void bluetooth_test(void);

//void encoder_init(void);

void my_imu963ra_init(void);

#endif /* CODE_COMMON_H_ */

