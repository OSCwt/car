#include "Encoder.h"

void ENCODER_Init(void)
{
    encoder_quad_init(ENCODER_RIGHT, ENCODER_RIGHT_A, ENCODER_RIGHT_B);
    encoder_clear_count(ENCODER_RIGHT);
    encoder_quad_init(ENCODER_LEFT, ENCODER_LEFT_A, ENCODER_LEFT_B);
    encoder_clear_count(ENCODER_LEFT);
    ENCODER_ClearTotal();
}

void ENCODER_ClearTotal(void)
{
    motorStr.EncoderTotal_L = 0;
    motorStr.EncoderTotal_R = 0;
}

void ENCODER_RevSample(void)
{
    motorStr.EncoderValue_R = ENCODER_RIGHT_DIR_SIGN * encoder_get_count(ENCODER_RIGHT);
    encoder_clear_count(ENCODER_RIGHT);

    motorStr.EncoderValue_L = ENCODER_LEFT_DIR_SIGN * encoder_get_count(ENCODER_LEFT);
    encoder_clear_count(ENCODER_LEFT);

    motorStr.EncoderTotal_L += motorStr.EncoderValue_L;
    motorStr.EncoderTotal_R += motorStr.EncoderValue_R;

    // PID 反馈
    L_motor.vi_FeedBack = (float)motorStr.EncoderValue_L;
    R_motor.vi_FeedBack = (float)motorStr.EncoderValue_R;

    // 实际速度反馈 (m/s)
    icarStr.SpeedFeedback_L = (float)(motorStr.EncoderValue_L * PI * motorStr.DiameterWheel)
        / MOTOR_CONTROL_CYCLE / motorStr.EncoderLine / motorStr.ReductionRatio;
    icarStr.SpeedFeedback_R= (float)(motorStr.EncoderValue_R * PI * motorStr.DiameterWheel)
                / MOTOR_CONTROL_CYCLE / motorStr.EncoderLine / motorStr.ReductionRatio;
//    if(icarStr.SpeedFeedback_L > 0 && icarStr.SpeedFeedback_L > icarStr.SpeedMaxRecords)
//        icarStr.SpeedMaxRecords = icarStr.SpeedFeedback_L;
//    else if(icarStr.SpeedFeedback < 0 && -icarStr.SpeedFeedback_L > icarStr.SpeedMaxRecords)
//        icarStr.SpeedMaxRecords = -icarStr.SpeedFeedback_L;

}
