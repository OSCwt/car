#include "Kalman_Filter.h"

Kalman_1DimTyp Speed_capture_Kalman;


void Kalman_1DIM_init(Kalman_1DimTyp* KalmanTyp,float Q,float R,float X, float P)
{
	KalmanTyp->K_Kalman=0;
	KalmanTyp->P_kalman_last=0;
	KalmanTyp->X_kalman_last=0;
	KalmanTyp->P_pre=0;
	KalmanTyp->X_pre=0;
	KalmanTyp->Sample=0;
	
	KalmanTyp->Q_Kalman=Q;
	KalmanTyp->R_Kalman=R;
	KalmanTyp->X_kalman=X;
	KalmanTyp->P_kalman=P;
}

float Kalman_1DIM(Kalman_1DimTyp* KalmanTyp)
{
	KalmanTyp->P_kalman_last = KalmanTyp->P_kalman;
	KalmanTyp->X_kalman_last = KalmanTyp->X_kalman;
	
	KalmanTyp->X_pre = KalmanTyp->X_kalman_last;
	KalmanTyp->P_pre = KalmanTyp->P_kalman_last+KalmanTyp->Q_Kalman;
	
	KalmanTyp->K_Kalman = KalmanTyp->P_pre / (KalmanTyp->P_pre+KalmanTyp->R_Kalman);
	KalmanTyp->P_kalman = KalmanTyp->P_pre - KalmanTyp->K_Kalman * KalmanTyp->P_pre;
	KalmanTyp->X_kalman = KalmanTyp->X_pre + KalmanTyp->K_Kalman * (KalmanTyp->Sample - KalmanTyp->X_pre);
	
	return KalmanTyp->X_kalman;
}
