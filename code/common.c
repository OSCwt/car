/*
 * common.c
 *
 *  创建时间: 2026年3月23日
 *      作者: 22563
 */

//这个也弃用

#include "zf_common_headfile.h"
#include "stdio.h"
#include "main.h"

void motor_init(void)
{

    pwm_init(PWM_L,50,0);
    pwm_init(PWM_R,50,0);

    gpio_init(motor_L_F, GPO, 0, GPO_PUSH_PULL);
    gpio_init(motor_L_B, GPO, 0, GPO_PUSH_PULL);
    gpio_init(motor_R_F, GPO, 0, GPO_PUSH_PULL);
    gpio_init(motor_R_B, GPO, 0, GPO_PUSH_PULL);
}

void motor_start(int L_speed,int R_speed)
{
    if(L_speed>0)
    {
        gpio_set_level(motor_L_F,0);
        gpio_set_level(motor_L_B,1);
        pwm_set_duty(PWM_L,L_speed);
    }
    if(R_speed>0)
    {
        gpio_set_level(motor_R_F,0);
        gpio_set_level(motor_R_B,1);
        pwm_set_duty(PWM_R,R_speed);
    }
}
void set_motor_enable_l(void)
{
    gpio_set_level(motor_L_F,1);
    gpio_set_level(motor_L_B,0);
}
void set_motor_enable_r(void)
{
    gpio_set_level(motor_R_F,1);
    gpio_set_level(motor_R_B,0);
}
void set_motor_enable(void)
{
    set_motor_enable_l();
    set_motor_enable_r();
}
void set_motor_disable(void)
{
    gpio_set_level(motor_R_F,0);
    gpio_set_level(motor_R_B,1);
    gpio_set_level(motor_L_F,0);
    gpio_set_level(motor_L_B,1);
}







void bluetooth_test()
{
    if(bluetooth_ch9141_init())                                                 // 判断是否通过初始化
        {
            while(1)                                                                // 初始化失败就在这进入死循环
            {

                system_delay_ms(100);                                               // 短延时快速闪灯表示异常
            }
        }
    bluetooth_ch9141_send_byte('\r');
    bluetooth_ch9141_send_byte('\n');
    bluetooth_ch9141_send_string("SEEKFREE ch9141 demo.\r\n");                  // 初始化正常 输出测试信息

}

//void encoder_init(void)
//{
//    encoder_quad_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B);                     // 初始化编码器模块与引脚 正交解码编码器模式
//    encoder_quad_init(ENCODER_3, ENCODER_3_A, ENCODER_3_B);                     // 初始化编码器模块与引脚 正交解码编码器模式
//    pit_ms_init(CCU60_CH0, 50);
//
//}

void my_imu963ra_init(void)
{
    while(1)
        {
            if(imu963ra_init())
            {
               printf("\r\nIMU963RA init error.");                                 // IMU963RA 初始化失败
            }
            else
            {
               break;
            }
        }

}
