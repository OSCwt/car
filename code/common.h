/*
 * common.h
 *
 *  创建时间: 2026年3月23日
 *      作者: 22563
 */

#ifndef CODE_COMMON_H_
#define CODE_COMMON_H_

//电机
#define PWM_L  ATOM1_CH0_P21_2
#define PWM_R  ATOM1_CH3_P10_3

#define motor_L_F (P21_5)
#define motor_L_B (P21_4)

#define motor_R_F (P02_5)
#define motor_R_B (P02_4)


//编码器
#define ENCODER_1                   (TIM4_ENCODER)
#define ENCODER_1_A                 (TIM4_ENCODER_CH1_P02_8)
#define ENCODER_1_B                 (TIM4_ENCODER_CH2_P00_9)

#define ENCODER_3                   (TIM5_ENCODER)
#define ENCODER_3_A                 (TIM5_ENCODER_CH1_P10_3)
#define ENCODER_3_B                 (TIM5_ENCODER_CH2_P10_1)


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

