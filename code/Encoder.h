#ifndef __ENCODER_H
#define __ENCODER_H

#include "main.h"

#define ENCODER_RIGHT               (TIM4_ENCODER)
#define ENCODER_RIGHT_A             (TIM4_ENCODER_CH1_P02_8)
#define ENCODER_RIGHT_B             (TIM4_ENCODER_CH2_P00_9)

#define ENCODER_LEFT                (TIM5_ENCODER)
#define ENCODER_LEFT_A              (TIM5_ENCODER_CH1_P10_3)
#define ENCODER_LEFT_B              (TIM5_ENCODER_CH2_P10_1)

#define ENCODER_RIGHT_DIR_SIGN      (1)
#define ENCODER_LEFT_DIR_SIGN       (-1)

void ENCODER_Init(void);
void ENCODER_RevSample(void);

#endif
