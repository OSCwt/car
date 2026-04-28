/*
 * top.c
 *
 *  创建时间: 2026-04-05
 *  作者: 22563
 */
#include "main.h"
#include "zf_common_headfile.h"
uint16 label_ids[LABEL_ROWS];

static float encoder_delta_to_speed_mps(signed int encoder_delta)
{
    return (float)(encoder_delta * PI * motorStr.DiameterWheel)
        / MOTOR_CONTROL_CYCLE
        / motorStr.EncoderLine
        / motorStr.ReductionRatio;
}

void isp_init(void)
{
    uint16 page_id = ips200pro_init("Demo", IPS200PRO_TITLE_TOP, 20);
    for (int i = 0; i < LABEL_ROWS; i++) {
        label_ids[i] = ips200pro_label_create(10, START_Y + i * LINE_HEIGHT, 200, LINE_HEIGHT);
    }
}

void task_init(void)
{
    isp_init();
    PID_debug_Init();
    SERVO_Init();
    RemoteControl_Init();
    Sasu_TIM_Init();

    ips200pro_label_printf(label_ids[0], "yaw_rel=%.2f", 0.0f);
    ips200pro_label_printf(label_ids[1], "yaw_wrap=%.2f", 0.0f);
    ips200pro_label_printf(label_ids[2], "gyro_z=%.2f dps", 0.0f);
    ips200pro_label_printf(label_ids[3], "bias_z=%.2f dps", 0.0f);
    ips200pro_label_printf(label_ids[4], "corr_z=%.2f dps", 0.0f);
    ips200pro_label_printf(label_ids[5], "dt=%u us", 0U);
    ips200pro_label_printf(label_ids[6], "calib=WAIT");
    ips200pro_label_printf(label_ids[7], "still=NO");
    ips200pro_label_printf(label_ids[8], "pitch=%.2f", 0.0f);
    ips200pro_label_printf(label_ids[9], "roll=%.2f", 0.0f);
    ips200pro_label_printf(label_ids[10], "lap=0/3");
    ips200pro_label_printf(label_ids[11], "finish=NO");
    ips200pro_label_printf(label_ids[12], "totalL=0");
    ips200pro_label_printf(label_ids[13], "totalR=0");
    ips200pro_label_printf(label_ids[14], "turn 10 laps");
}

void task(void)
{
    PID_debug_Handle();
    ips200pro_label_printf(label_ids[0], "P=%.2f I=%.2f D=%.2f", L_motor.v_Kp, L_motor.v_Ki, L_motor.v_Kd);
    ips200pro_label_printf(label_ids[1], "speed=%f", icarStr.SpeedFeedback_L);
    ips200pro_label_printf(label_ids[2], "speedset=%f", icarStr.SpeedSet);
    if(piddebug.MotorEnable==true)
        ips200pro_label_printf(label_ids[3], "motor_on");
    else
        ips200pro_label_printf(label_ids[3], "motor_off");

    ips200pro_label_printf(label_ids[4], "set_encoder_L=%.3f", L_motor.vi_Ref);
    ips200pro_label_printf(label_ids[5], "set_encoder_R=%.3f", R_motor.vi_Ref);
    ips200pro_label_printf(label_ids[6], "Encoder_L=%f", (float)motorStr.EncoderValue_L);
    ips200pro_label_printf(label_ids[7], "Encoder_R=%f", (float)motorStr.EncoderValue_R);
    ips200pro_label_printf(label_ids[8], "PWM_L=%d", motorStr.PWM_L_value);
    ips200pro_label_printf(label_ids[9], "PWM_R=%d", motorStr.PWM_R_value);
    ips200pro_label_printf(label_ids[10], "totalL=%d", motorStr.EncoderTotal_L);
    ips200pro_label_printf(label_ids[11], "totalR=%d", motorStr.EncoderTotal_R);
    ips200pro_label_printf(label_ids[12], "avg10L=%d", motorStr.EncoderTotal_L / 10);
    ips200pro_label_printf(label_ids[13], "avg10R=%d", motorStr.EncoderTotal_R / 10);
    ips200pro_label_printf(label_ids[14], "turn 10 laps");
}

void control(void)
{
    float speed_l = encoder_delta_to_speed_mps(motorStr.EncoderValue_L);
    float speed_r = encoder_delta_to_speed_mps(motorStr.EncoderValue_R);
    uint8 enc_r_a = gpio_get_level(P02_8);
    uint8 enc_r_b = gpio_get_level(P00_9);
    uint8 enc_l_a = gpio_get_level(P10_3);
    uint8 enc_l_b = gpio_get_level(P10_1);


    ips200pro_label_printf(label_ids[0], "remote=%s lost=%ums", RemoteControl_IsOnline() ? "ON" : "OFF", RemoteControl_GetLostMs());
    ips200pro_label_printf(label_ids[1], "thr=%d dir=%d", RemoteControl_GetThrottleRaw(), RemoteControl_GetSteerRaw());
    ips200pro_label_printf(label_ids[2], "key=0x%02X mode=0x%02X", RemoteControl_GetKeyMask(), RemoteControl_GetModeMask());
    ips200pro_label_printf(label_ids[3], "r_spd=%.3f steer=%.1f", RemoteControl_GetSpeedSet(), RemoteControl_GetSteerOffset());
    ips200pro_label_printf(label_ids[4], "valid=%u err=%u", (unsigned int)RemoteControl_GetValidCount(), (unsigned int)RemoteControl_GetChecksumError());
    ips200pro_label_printf(label_ids[5], "fb=%.3f set=%.3f", icarStr.SpeedFeedback_L, icarStr.SpeedSet);
    ips200pro_label_printf(label_ids[6], "spdL=%.3f spdR=%.3f", speed_l, speed_r);
    ips200pro_label_printf(label_ids[7], "pwmL=%d pwmR=%d", motorStr.PWM_L_value, motorStr.PWM_R_value);
    ips200pro_label_printf(label_ids[8], "motor=%s cnt=%u", piddebug.MotorEnable ? "ON" : "OFF", motorStr.Counter);
    ips200pro_label_printf(label_ids[9], "encL=%d encR=%d", motorStr.EncoderValue_L, motorStr.EncoderValue_R);
    ips200pro_label_printf(label_ids[10], "pinR A=%u B=%u", enc_r_a, enc_r_b);
    ips200pro_label_printf(label_ids[11], "pinL A=%u B=%u", enc_l_a, enc_l_b);
    ips200pro_label_printf(label_ids[12], "yaw=%.2f roll=%.2f", attitudeDebug.yaw_continuous_deg, eulerAngle.roll);
    ips200pro_label_printf(label_ids[13], "refL=%.1f refR=%.1f", L_motor.vi_Ref, R_motor.vi_Ref);
    ips200pro_label_printf(label_ids[14], "avg10 L=%d R=%d", motorStr.EncoderTotal_L / 10, motorStr.EncoderTotal_R / 10);
}
