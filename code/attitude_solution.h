#ifndef __ATTITUDE_H
#define __ATTITUDE_H

#include "main.h"

typedef struct {
    float gyro_x;
    float gyro_y;
    float gyro_z;
    float acc_x;
    float acc_y;
    float acc_z;
} icm_param_t;

typedef struct {
    float q0;
    float q1;
    float q2;
    float q3;
} quater_param_t;

typedef struct {
    float pitch;
    float roll;
    float yaw;
} euler_param_t;

typedef struct {
    float Xdata;
    float Ydata;
    float Zdata;
} gyro_param_t;

typedef struct {
    float dt_s;
    float gyro_z_raw_dps;
    float gyro_z_bias_dps;
    float gyro_z_corr_dps;
    float yaw_continuous_deg;
    float yaw_wrapped_deg;
    bool calibrated;
    bool stationary;
} attitude_debug_t;

typedef struct {
    uint8 lap_count;
    uint8 target_laps;
    bool race_finished;
    bool start_zone_gate_enabled;
    bool start_zone_active;
    float lap_base_yaw_deg;
} attitude_lap_state_t;

extern euler_param_t eulerAngle;
extern attitude_debug_t attitudeDebug;
extern attitude_lap_state_t attitudeLapState;

void gyroOffset_init(void);
float fast_sqrt(float x);
void ICM_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az);
void ICM_getValues(void);
void ICM_getEulerianAngles(void);

// 重置圈数、完成标志和本次比赛的起始 yaw 基准。
// 建议在比赛开始前、重新发车前或需要重新计圈时调用。
void attitude_lap_reset(void);

// 传入当前是否位于起点/终点检测区域。
// 这个接口是专门给后续赛道状态机预留的：如果起点在线段中间，
// 可以把摄像头、路径状态、地磁点、红外或地标检测结果接到这里防止误判计圈。
// active=true 表示当前已经进入允许计圈的区域，false 表示当前不在该区域。
void attitude_lap_set_start_zone(bool active);

// 控制是否启用“起点区域门控”。
// enable=false 时，只按 yaw 自动计圈；
// enable=true 时，必须同时满足 yaw 条件和 start_zone_active=true 才允许计圈。
// 这个接口保留给后续赛道规则变化时切换使用，不需要改计圈主逻辑。
void attitude_lap_enable_start_zone_gate(bool enable);

#endif
