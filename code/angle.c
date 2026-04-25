/////*
//// * angle.c
//// *
//// *  创建时间: 2026年3月27日
//// *      作者: 22563
//// */
////
////这个弃用
//#include "zf_common_headfile.h"
//
////gyroOffset_info_struct gyroOffset;
//
//static float T = 0.005f;
//
//float roll;
//float pitch;
//float yaw;
//
//float yaw_new ;
//float gzz1_out;
//
//void gyroOffsetInit(void) {
//    gyroOffset.Xdata = 0;
//    gyroOffset.Ydata = 0;
//    gyroOffset.Zdata = 0;
//
//    for (uint16_t i = 0; i < 500; i++) {
//        imu963ra_get_gyro();  // 读取原始数据
//        gyroOffset.Xdata += imu963ra_gyro_x;
//        gyroOffset.Ydata += imu963ra_gyro_y;
//        gyroOffset.Zdata += imu963ra_gyro_z;
//        system_delay_ms(5);
//    }
//
//    gyroOffset.Xdata /= 500;  // 取平均值
//    gyroOffset.Ydata /= 500;
//    gyroOffset.Zdata /= 500;
//}
//
////低通滤波和积分
//void IMU_Independent(float gyro_z_raw, float gyro_z_offset, float *yaw_out, float *gzz1_out, float sample_time)
//{
//    static float gyro_z_cal = 0;
//
//    gyro_z_cal = gyro_z_raw - gyro_z_offset;
//
//    if (gzz1_out != NULL) *gzz1_out = gyro_z_cal;   // 输出临时值
//
//    if ((gyro_z_cal > 5.0f) || (gyro_z_cal < -5.0f))
//    {
//        *yaw_out -= gyro_z_cal / 14.3f * sample_time;
//
//    }
//    else
//    {
//        gyro_z_cal = 0;
//    }
//}
//
//
//
//
//void loop()
//{
//    imu963ra_get_acc();                                                         // 获取 IMU963RA 的加速度测量数值
//    imu963ra_get_gyro();                                                        // 获取 IMU963RA 的角速度测量数值
//
//
//    //低通滤波加积分运算，基本上够用了
//    IMU_Independent(imu963ra_gyro_z,gyroOffset.Zdata,&yaw_new,&gzz1_out,T);
//
//
//
//}
//
//
//
