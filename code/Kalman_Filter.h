#ifndef _Kalman_Filter_H
#define _Kalman_Filter_H
#include "main.h"

#define CAP_KAL_Q 0.001F
#define CAP_KAL_R 0.09F
#define CAP_KAL_X 100
#define CAP_KAL_P 2

typedef struct {
	float X_pre;         //上一次状态变量
	float P_pre;         //上一次误差协方差
	float X_kalman_last; //卡尔曼滤波上一次状态
	float P_kalman_last; //卡尔曼滤波上一次协方差
	float X_kalman;      //当前状态估计值
	float P_kalman;      //当前的误差协方差
	float Sample;        //观测值
	float K_Kalman;      //卡尔曼增益
	float Q_Kalman;      //过程噪声协方差矩阵
	float R_Kalman;      //观测噪声协方差矩阵

}Kalman_1DimTyp;

extern Kalman_1DimTyp Speed_capture_Kalman;

float Kalman_1DIM(Kalman_1DimTyp* KalmanTyp);//一维卡尔曼滤波
void Kalman_1DIM_init(Kalman_1DimTyp* KalmanTyp,float Q,float R,float X, float P);

#endif
