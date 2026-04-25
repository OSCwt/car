#include "attitude_solution.h"
#include "IfxStm.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926f
#endif

#define ATTITUDE_DEG_TO_RAD                     (M_PI / 180.0f)
#define ATTITUDE_RAD_TO_DEG                     (180.0f / M_PI)
#define ATTITUDE_DEFAULT_DT_S                   (0.0005f)
#define ATTITUDE_MIN_DT_S                       (0.0003f)
#define ATTITUDE_MAX_DT_S                       (0.0030f)
#define ATTITUDE_ACC_ALPHA                      (0.3f)
#define ATTITUDE_CALIBRATION_WINDOW_MS          (2000U)
#define ATTITUDE_CALIBRATION_SAMPLE_INTERVAL_MS (5U)
#define ATTITUDE_CALIBRATION_REQUIRED_SAMPLES   (200U)
#define ATTITUDE_STATIONARY_GYRO_DPS            (3.0f)
#define ATTITUDE_STATIONARY_ACC_MIN_G           (0.95f)
#define ATTITUDE_STATIONARY_ACC_MAX_G           (1.05f)
#define ATTITUDE_YAW_INTEGRATION_DEADBAND_DPS   (0.20f)
#define ATTITUDE_RUNTIME_BIAS_TC_S              (5.0f)
#define ATTITUDE_RUNTIME_BIAS_ENABLE_DPS        (1.0f)
#define ATTITUDE_DEFAULT_TARGET_LAPS            (3U)
#define ATTITUDE_LAP_YAW_WINDOW_DEG             (20.0f)

static float I_ex = 0.0f;
static float I_ey = 0.0f;
static float I_ez = 0.0f;

quater_param_t Q_info = {1.0f, 0.0f, 0.0f, 0.0f};
euler_param_t eulerAngle;
attitude_debug_t attitudeDebug;
attitude_lap_state_t attitudeLapState;

icm_param_t icm_data;
gyro_param_t GyroOffset;

bool GyroOffset_init = 0;

float param_Kp = 0.17f;
float param_Ki = 0.004f;

static Ifx_STM *attitude_stm = NULL;
static float attitude_stm_freq = 0.0f;
static uint32 attitude_last_tick = 0U;

static float attitude_clamp(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static float attitude_wrap_360(float angle_deg)
{
    while (angle_deg >= 360.0f) {
        angle_deg -= 360.0f;
    }
    while (angle_deg < 0.0f) {
        angle_deg += 360.0f;
    }
    return angle_deg;
}

static void attitude_init_timer(void)
{
    IfxStm_Index stm_index;

    if (NULL != attitude_stm) {
        return;
    }

    stm_index = (IfxStm_Index)IfxCpu_getCoreId();
    attitude_stm = IfxStm_getAddress(stm_index);
    if (NULL == attitude_stm) {
        attitude_stm = &MODULE_STM0;
    }

    attitude_stm_freq = IfxStm_getFrequency(attitude_stm);
    if (attitude_stm_freq <= 0.0f) {
        attitude_stm_freq = 100000000.0f;
    }
}

static float attitude_update_dt(void)
{
    uint32 current_tick;
    float dt_s = ATTITUDE_DEFAULT_DT_S;

    attitude_init_timer();
    current_tick = IfxStm_getLower(attitude_stm);

    if ((0U != attitude_last_tick) && (attitude_stm_freq > 0.0f)) {
        dt_s = (float)(current_tick - attitude_last_tick) / attitude_stm_freq;
    }

    attitude_last_tick = current_tick;
    attitudeDebug.dt_s = attitude_clamp(dt_s, ATTITUDE_MIN_DT_S, ATTITUDE_MAX_DT_S);
    return attitudeDebug.dt_s;
}

static bool attitude_is_stationary(float gyro_x_dps, float gyro_y_dps, float gyro_z_dps, float acc_norm_g)
{
    return (fabsf(gyro_x_dps) < ATTITUDE_STATIONARY_GYRO_DPS)
        && (fabsf(gyro_y_dps) < ATTITUDE_STATIONARY_GYRO_DPS)
        && (fabsf(gyro_z_dps) < ATTITUDE_STATIONARY_GYRO_DPS)
        && (acc_norm_g >= ATTITUDE_STATIONARY_ACC_MIN_G)
        && (acc_norm_g <= ATTITUDE_STATIONARY_ACC_MAX_G);
}

static void attitude_update_runtime_bias_z(void)
{
    float correction_gain;

    if (!attitudeDebug.calibrated || !attitudeDebug.stationary) {
        return;
    }

    if (fabsf(attitudeDebug.gyro_z_corr_dps) >= ATTITUDE_RUNTIME_BIAS_ENABLE_DPS) {
        return;
    }

    correction_gain = attitude_clamp(attitudeDebug.dt_s / ATTITUDE_RUNTIME_BIAS_TC_S, 0.0f, 1.0f);
    GyroOffset.Zdata += (attitudeDebug.gyro_z_raw_dps - GyroOffset.Zdata) * correction_gain;

    attitudeDebug.gyro_z_bias_dps = GyroOffset.Zdata;
    attitudeDebug.gyro_z_corr_dps = attitudeDebug.gyro_z_raw_dps - GyroOffset.Zdata;
    icm_data.gyro_z = attitudeDebug.gyro_z_corr_dps * ATTITUDE_DEG_TO_RAD;
}

static void attitude_update_lap_state(void)
{
    float lap_progress_abs_deg;
    float target_yaw_deg;
    uint8 target_laps;

    target_laps = attitudeLapState.target_laps;
    if (0U == target_laps) {
        target_laps = ATTITUDE_DEFAULT_TARGET_LAPS;
        attitudeLapState.target_laps = target_laps;
    }

    if (attitudeLapState.race_finished || (attitudeLapState.lap_count >= target_laps)) {
        attitudeLapState.race_finished = true;
        return;
    }

    lap_progress_abs_deg = fabsf(attitudeDebug.yaw_continuous_deg - attitudeLapState.lap_base_yaw_deg);
    target_yaw_deg = 360.0f * (float)(attitudeLapState.lap_count + 1U);

    if (lap_progress_abs_deg < (target_yaw_deg - ATTITUDE_LAP_YAW_WINDOW_DEG)) {
        return;
    }

    if (attitudeLapState.start_zone_gate_enabled && !attitudeLapState.start_zone_active) {
        return;
    }

    attitudeLapState.lap_count++;
    if (attitudeLapState.start_zone_gate_enabled) {
        attitudeLapState.start_zone_active = false;
    }

    if (attitudeLapState.lap_count >= target_laps) {
        attitudeLapState.race_finished = true;
    }
}

static void attitude_reset_runtime_state(void)
{
    I_ex = 0.0f;
    I_ey = 0.0f;
    I_ez = 0.0f;

    Q_info.q0 = 1.0f;
    Q_info.q1 = 0.0f;
    Q_info.q2 = 0.0f;
    Q_info.q3 = 0.0f;

    icm_data.acc_x = 0.0f;
    icm_data.acc_y = 0.0f;
    icm_data.acc_z = 0.0f;
    icm_data.gyro_x = 0.0f;
    icm_data.gyro_y = 0.0f;
    icm_data.gyro_z = 0.0f;

    attitudeDebug.dt_s = ATTITUDE_DEFAULT_DT_S;
    attitudeDebug.gyro_z_raw_dps = 0.0f;
    attitudeDebug.gyro_z_bias_dps = 0.0f;
    attitudeDebug.gyro_z_corr_dps = 0.0f;
    attitudeDebug.yaw_continuous_deg = 0.0f;
    attitudeDebug.yaw_wrapped_deg = 0.0f;
    attitudeDebug.calibrated = false;
    attitudeDebug.stationary = false;

    attitudeLapState.lap_count = 0U;
    attitudeLapState.target_laps = ATTITUDE_DEFAULT_TARGET_LAPS;
    attitudeLapState.race_finished = false;
    attitudeLapState.start_zone_gate_enabled = false;
    attitudeLapState.start_zone_active = false;
    attitudeLapState.lap_base_yaw_deg = 0.0f;

    attitude_last_tick = 0U;
}

float fast_sqrt(float x)
{
    float halfx;
    float y;
    long i;

    if (x <= 0.0f) {
        return 0.0f;
    }

    halfx = 0.5f * x;
    y = x;
    i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}

void attitude_lap_reset(void)
{
    if (0U == attitudeLapState.target_laps) {
        attitudeLapState.target_laps = ATTITUDE_DEFAULT_TARGET_LAPS;
    }

    attitudeLapState.lap_count = 0U;
    attitudeLapState.race_finished = false;
    attitudeLapState.start_zone_active = false;
    attitudeLapState.lap_base_yaw_deg = attitudeDebug.yaw_continuous_deg;
}

void attitude_lap_set_start_zone(bool active)
{
    attitudeLapState.start_zone_active = active;
}

void attitude_lap_enable_start_zone_gate(bool enable)
{
    attitudeLapState.start_zone_gate_enabled = enable;
}

void gyroOffset_init(void)
{
    float sum_x = 0.0f;
    float sum_y = 0.0f;
    float sum_z = 0.0f;
    uint32 stationary_samples = 0U;
    uint32 total_samples = ATTITUDE_CALIBRATION_WINDOW_MS / ATTITUDE_CALIBRATION_SAMPLE_INTERVAL_MS;
    uint32 i;

    attitude_init_timer();
    attitude_reset_runtime_state();

    GyroOffset.Xdata = 0.0f;
    GyroOffset.Ydata = 0.0f;
    GyroOffset.Zdata = 0.0f;

    for (i = 0U; i < total_samples; ++i) {
        float gyro_x_dps;
        float gyro_y_dps;
        float gyro_z_dps;
        float acc_x_g;
        float acc_y_g;
        float acc_z_g;
        float acc_norm_g;
        bool stationary;

        imu963ra_get_acc();
        imu963ra_get_gyro();

        gyro_x_dps = imu963ra_gyro_transition(imu963ra_gyro_x);
        gyro_y_dps = imu963ra_gyro_transition(imu963ra_gyro_y);
        gyro_z_dps = imu963ra_gyro_transition(imu963ra_gyro_z);
        acc_x_g = imu963ra_acc_transition(imu963ra_acc_x);
        acc_y_g = imu963ra_acc_transition(imu963ra_acc_y);
        acc_z_g = imu963ra_acc_transition(imu963ra_acc_z);
        acc_norm_g = sqrtf(acc_x_g * acc_x_g + acc_y_g * acc_y_g + acc_z_g * acc_z_g);
        stationary = attitude_is_stationary(gyro_x_dps, gyro_y_dps, gyro_z_dps, acc_norm_g);

        attitudeDebug.gyro_z_raw_dps = gyro_z_dps;
        attitudeDebug.stationary = stationary;

        if (stationary) {
            sum_x += gyro_x_dps;
            sum_y += gyro_y_dps;
            sum_z += gyro_z_dps;
            stationary_samples++;
        }

        system_delay_ms(ATTITUDE_CALIBRATION_SAMPLE_INTERVAL_MS);
    }

    if (stationary_samples >= ATTITUDE_CALIBRATION_REQUIRED_SAMPLES) {
        GyroOffset.Xdata = sum_x / (float)stationary_samples;
        GyroOffset.Ydata = sum_y / (float)stationary_samples;
        GyroOffset.Zdata = sum_z / (float)stationary_samples;
        attitudeDebug.calibrated = true;
    } else {
        attitudeDebug.calibrated = false;
    }

    attitudeDebug.gyro_z_bias_dps = GyroOffset.Zdata;
    attitudeDebug.gyro_z_corr_dps = attitudeDebug.gyro_z_raw_dps - GyroOffset.Zdata;
    GyroOffset_init = attitudeDebug.calibrated;
    attitude_last_tick = IfxStm_getLower(attitude_stm);
    attitude_lap_reset();
}

void ICM_getValues(void)
{
    float acc_x_g = imu963ra_acc_transition(imu963ra_acc_x);
    float acc_y_g = imu963ra_acc_transition(imu963ra_acc_y);
    float acc_z_g = imu963ra_acc_transition(imu963ra_acc_z);
    float gyro_x_raw_dps = imu963ra_gyro_transition(imu963ra_gyro_x);
    float gyro_y_raw_dps = imu963ra_gyro_transition(imu963ra_gyro_y);
    float gyro_z_raw_dps = imu963ra_gyro_transition(imu963ra_gyro_z);
    float acc_norm_g = sqrtf(acc_x_g * acc_x_g + acc_y_g * acc_y_g + acc_z_g * acc_z_g);

    icm_data.acc_x = acc_x_g * ATTITUDE_ACC_ALPHA + icm_data.acc_x * (1.0f - ATTITUDE_ACC_ALPHA);
    icm_data.acc_y = acc_y_g * ATTITUDE_ACC_ALPHA + icm_data.acc_y * (1.0f - ATTITUDE_ACC_ALPHA);
    icm_data.acc_z = acc_z_g * ATTITUDE_ACC_ALPHA + icm_data.acc_z * (1.0f - ATTITUDE_ACC_ALPHA);

    icm_data.gyro_x = (gyro_x_raw_dps - GyroOffset.Xdata) * ATTITUDE_DEG_TO_RAD;
    icm_data.gyro_y = (gyro_y_raw_dps - GyroOffset.Ydata) * ATTITUDE_DEG_TO_RAD;
    icm_data.gyro_z = (gyro_z_raw_dps - GyroOffset.Zdata) * ATTITUDE_DEG_TO_RAD;

    attitudeDebug.gyro_z_raw_dps = gyro_z_raw_dps;
    attitudeDebug.gyro_z_bias_dps = GyroOffset.Zdata;
    attitudeDebug.gyro_z_corr_dps = gyro_z_raw_dps - GyroOffset.Zdata;
    attitudeDebug.stationary = attitude_is_stationary(gyro_x_raw_dps, gyro_y_raw_dps, gyro_z_raw_dps, acc_norm_g);
}

void ICM_AHRSupdate(float gx, float gy, float gz, float ax, float ay, float az)
{
    float halfT = 0.5f * attitudeDebug.dt_s;
    float vx;
    float vy;
    float vz;
    float ex = 0.0f;
    float ey = 0.0f;
    float ez = 0.0f;
    float q0 = Q_info.q0;
    float q1 = Q_info.q1;
    float q2 = Q_info.q2;
    float q3 = Q_info.q3;
    float q0q0 = q0 * q0;
    float q0q1 = q0 * q1;
    float q0q2 = q0 * q2;
    float q1q1 = q1 * q1;
    float q1q3 = q1 * q3;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q3q3 = q3 * q3;
    float norm_inv;

    norm_inv = fast_sqrt(ax * ax + ay * ay + az * az);
    if (norm_inv > 0.0f) {
        ax = ax * norm_inv;
        ay = ay * norm_inv;
        az = az * norm_inv;

        vx = 2.0f * (q1q3 - q0q2);
        vy = 2.0f * (q0q1 + q2q3);
        vz = q0q0 - q1q1 - q2q2 + q3q3;

        ex = ay * vz - az * vy;
        ey = az * vx - ax * vz;
        ez = ax * vy - ay * vx;

        I_ex += halfT * ex;
        I_ey += halfT * ey;
        I_ez += halfT * ez;

        gx = gx + param_Kp * ex + param_Ki * I_ex;
        gy = gy + param_Kp * ey + param_Ki * I_ey;
        gz = gz + param_Kp * ez + param_Ki * I_ez;
    }

    q0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * halfT;
    q1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * halfT;
    q2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * halfT;
    q3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * halfT;

    norm_inv = fast_sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    if (norm_inv > 0.0f) {
        Q_info.q0 = q0 * norm_inv;
        Q_info.q1 = q1 * norm_inv;
        Q_info.q2 = q2 * norm_inv;
        Q_info.q3 = q3 * norm_inv;
    }
}

void ICM_getEulerianAngles(void)
{
    float q0;
    float q1;
    float q2;
    float q3;
    float pitch_arg;
    float yaw_rate_for_integration_dps;
    imu963ra_get_acc();
    imu963ra_get_gyro();

    attitude_update_dt();
    ICM_getValues();
    attitude_update_runtime_bias_z();
    ICM_AHRSupdate(icm_data.gyro_x, icm_data.gyro_y, icm_data.gyro_z, icm_data.acc_x, icm_data.acc_y, icm_data.acc_z);

    q0 = Q_info.q0;
    q1 = Q_info.q1;
    q2 = Q_info.q2;
    q3 = Q_info.q3;

    pitch_arg = attitude_clamp(-2.0f * q1 * q3 + 2.0f * q0 * q2, -1.0f, 1.0f);
    eulerAngle.pitch = asinf(pitch_arg) * ATTITUDE_RAD_TO_DEG;
    eulerAngle.roll = atan2f(2.0f * q2 * q3 + 2.0f * q0 * q1, -2.0f * q1 * q1 - 2.0f * q2 * q2 + 1.0f) * ATTITUDE_RAD_TO_DEG;

    if ((eulerAngle.roll > 90.0f) || (eulerAngle.roll < -90.0f)) {
        if (eulerAngle.pitch > 0.0f) {
            eulerAngle.pitch = 180.0f - eulerAngle.pitch;
        }
        if (eulerAngle.pitch < 0.0f) {
            eulerAngle.pitch = -(180.0f + eulerAngle.pitch);
        }
    }

    // Smart-car control uses gyro-relative yaw to avoid wrap jumps and magnetic interference.
    yaw_rate_for_integration_dps = attitudeDebug.gyro_z_corr_dps;
    if (fabsf(yaw_rate_for_integration_dps) < ATTITUDE_YAW_INTEGRATION_DEADBAND_DPS) {
        yaw_rate_for_integration_dps = 0.0f;
    }

    attitudeDebug.yaw_continuous_deg += yaw_rate_for_integration_dps * attitudeDebug.dt_s;
    attitudeDebug.yaw_wrapped_deg = attitude_wrap_360(attitudeDebug.yaw_continuous_deg);
    eulerAngle.yaw = attitudeDebug.yaw_wrapped_deg;
    attitude_update_lap_state();
}
