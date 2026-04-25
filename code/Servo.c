#include "main.h"

ServoStruct servoStr;
uint32 PWMVALUE = 0;

static float servo_limit_float(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }

    if (value > max_value) {
        return max_value;
    }

    return value;
}

static uint32 servo_angle_to_duty(float angle_deg)
{
    float limited_angle = servo_limit_float(angle_deg, SERVO_MOTOR_L_angle, SERVO_MOTOR_R_angle);
    return (uint32)SERVO_MOTOR_DUTY(limited_angle);
}

static float servo_pwm_us_to_angle(signed int pwm_us)
{
    float limited_pwm = servo_limit_float((float)pwm_us, (float)SERVO_PWM_MIN, (float)SERVO_PWM_MAX);
    return (limited_pwm - 500.0f) * 90.0f / 1000.0f;
}

static void servo_apply_angle(float angle_deg)
{
    servoStr.output_angle = servo_limit_float(angle_deg, SERVO_MOTOR_L_angle, SERVO_MOTOR_R_angle);
    servoStr.offset_angle = servoStr.output_angle - SERVO_CENTER_ANGLE;
    servoStr.output_duty = servo_angle_to_duty(servoStr.output_angle);
    servoStr.thresholdMiddle = servo_angle_to_duty(SERVO_CENTER_ANGLE);
    servoStr.thresholdLeft = servo_angle_to_duty(SERVO_MOTOR_L_angle);
    servoStr.thresholdRight = servo_angle_to_duty(SERVO_MOTOR_R_angle);
    servoStr.angle = servoStr.offset_angle;
    PWMVALUE = servoStr.output_duty;
    pwm_set_duty(SERVO_PIN, servoStr.output_duty);
}

void SERVO_Init(void)
{
    servoStr.thresholdLeft = servo_angle_to_duty(SERVO_MOTOR_L_angle);
    servoStr.thresholdRight = servo_angle_to_duty(SERVO_MOTOR_R_angle);
    servoStr.thresholdMiddle = servo_angle_to_duty(SERVO_CENTER_ANGLE);
    servoStr.Counter = 0;
    servoStr.Counter1 = 0;
    servoStr.set_angle = SERVO_CENTER_ANGLE;
    servoStr.imu_angle = 0.0f;
    servoStr.angle_PreError = 0.0f;
    servoStr.angel_PreDerror = 0.0f;
    servoStr.Kp = 0.0f;
    servoStr.Ki = 0.0f;
    servoStr.Kd = 0.0f;
    servoStr.angle = 0.0f;
    servoStr.target_angle = SERVO_CENTER_ANGLE;
    servoStr.output_angle = SERVO_CENTER_ANGLE;
    servoStr.offset_angle = 0.0f;
    servoStr.output_duty = servoStr.thresholdMiddle;
    PWMVALUE = servoStr.output_duty;

    pwm_init(SERVO_PIN, SERVO_MOTOR_FREQ, servoStr.output_duty);
}

void servo_set(float angle)
{
    servoStr.target_angle = servo_limit_float(angle, SERVO_MOTOR_L_angle, SERVO_MOTOR_R_angle);
    servo_apply_angle(servoStr.target_angle);
}

void SERVO_SetAngle(float angle_deg)
{
    servoStr.target_angle = servo_limit_float(angle_deg, SERVO_MOTOR_L_angle, SERVO_MOTOR_R_angle);
    servoStr.set_angle = servoStr.target_angle;
}

void SERVO_SetOffset(float offset_deg)
{
    float max_left_offset = SERVO_MOTOR_L_angle - SERVO_CENTER_ANGLE;
    float max_right_offset = SERVO_MOTOR_R_angle - SERVO_CENTER_ANGLE;
    float limited_offset = servo_limit_float(offset_deg, max_left_offset, max_right_offset);

    servoStr.offset_angle = limited_offset;
    SERVO_SetAngle(SERVO_CENTER_ANGLE + limited_offset);
}

void SERVO_Center(void)
{
    SERVO_SetAngle(SERVO_CENTER_ANGLE);
}

void angle_calculate(ServoStruct* pp)
{
    if (pp == 0) {
        return;
    }

    pp->angle = pp->target_angle - SERVO_CENTER_ANGLE;
}

void servo_time(void)
{
    servoStr.Counter++;

    if (servoStr.Counter >= SERVO_UPDATE_PERIOD_MS) {
        servoStr.Counter = 0;
        angle_calculate(&servoStr);
        servo_apply_angle(servoStr.target_angle);
    }
}

void SERVO_SetPwmValue(signed int pwm)
{
    SERVO_SetAngle(servo_pwm_us_to_angle(pwm));
}

void SERVO_SetPwmValueCorrect(signed int pwm)
{
    SERVO_SetPwmValue(pwm);
}

void SERVO_AngleControl(float angle)
{
    SERVO_SetOffset(angle);
}
