#include "main.h"

volatile RemoteControlStatus remoteControlStatus;

static uint8 remote_rx_buf[REMOTE_FRAME_LEN];
static uint8 remote_rx_index = 0;
static RemoteControlPacket remote_latest_packet;
static uint8 remote_has_valid_packet = 0;
static uint8 remote_last_key0 = 0;
static uint8 remote_lost_protected = 1;
static float remote_steer_filtered = 0.0f;
static float remote_last_servo_offset = 0.0f;

// 将摇杆值限制在归一化范围内，避免异常数据导致目标值过大。
static float remote_limit_float(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }

    if (value > max_value) {
        return max_value;
    }

    return value;
}

// 计算浮点数绝对值，避免额外依赖数学库。
static float remote_abs_float(float value)
{
    return (value >= 0.0f) ? value : -value;
}

// 清空遥控转向滤波状态，避免失联或急停恢复后舵机跳到旧目标。
static void remote_reset_steer_filter(void)
{
    remote_steer_filtered = 0.0f;
    remote_last_servo_offset = 0.0f;
    remoteControlStatus.steer_offset = 0.0f;
}

// 把四个 0/1 字节压成 bit 掩码，方便屏幕调试显示。
static uint8 remote_pack_mask(const uint8 *data)
{
    uint8 mask = 0;

    for (uint8 i = 0; i < 4; i++) {
        if (data[i]) {
            mask |= (uint8)(1u << i);
        }
    }

    return mask;
}

// 按遥控器样例工程的算法校验：校验字节按 0 参与整帧累加。
static uint8 remote_checksum_ok(const uint8 *frame)
{
    uint8 sum = 0;

    for (uint8 i = 0; i < REMOTE_FRAME_LEN; i++) {
        if (i == 1) {
            continue;
        }
        sum += frame[i];
    }

    return (sum == frame[1]);
}

// 小端合成 int16，匹配 AT32 端结构体直接透传的内存顺序。
static int16 remote_get_int16_le(const uint8 *data)
{
    uint16 value = (uint16)data[0] | ((uint16)data[1] << 8);
    return (int16)value;
}

// 收到合法帧后缓存字段，定时器里再应用到电机和舵机目标，避免中断里做复杂控制。
static void remote_accept_frame(const uint8 *frame)
{
    remote_latest_packet.head = frame[0];
    remote_latest_packet.sum_check = frame[1];

    for (uint8 i = 0; i < 4; i++) {
        remote_latest_packet.joystick[i] = remote_get_int16_le(&frame[2 + i * 2]);
    }

    for (uint8 i = 0; i < 4; i++) {
        remote_latest_packet.key[i] = frame[10 + i];
        remote_latest_packet.switch_key[i] = frame[14 + i];
    }

    remoteControlStatus.throttle_raw = remote_latest_packet.joystick[1];
    remoteControlStatus.steer_raw = remote_latest_packet.joystick[2];
    remoteControlStatus.key_mask = remote_pack_mask(remote_latest_packet.key);
    remoteControlStatus.mode_mask = remote_pack_mask(remote_latest_packet.switch_key);
    remoteControlStatus.lost_ms = 0;
    remoteControlStatus.online = 1;
    remoteControlStatus.valid_count++;
    remote_has_valid_packet = 1;
    remote_lost_protected = 0;
}

// 初始化遥控接收串口，LoRa 模块需提前配好 115200 透传模式。
void RemoteControl_Init(void)
{
    memset((void *)&remoteControlStatus, 0, sizeof(remoteControlStatus));
    memset((void *)&remote_latest_packet, 0, sizeof(remote_latest_packet));

    remote_rx_index = 0;
    remote_has_valid_packet = 0;
    remote_last_key0 = 0;
    remote_lost_protected = 1;
    remote_reset_steer_filter();

    // 遥控上电前保持安全状态：速度为 0，舵机回中。
    icarStr.SpeedSet = 0.0f;
    SERVO_Center();

    uart_init(REMOTE_UART_INDEX, REMOTE_UART_BAUDRATE, REMOTE_UART_TX_PIN, REMOTE_UART_RX_PIN);
    uart_rx_interrupt(REMOTE_UART_INDEX, 1);
}

// 串口中断每收到 1 字节就调用本函数，状态机负责同步帧头和拼满整帧。
void RemoteControl_RecvByte(uint8 dat)
{
    if (remote_rx_index == 0) {
        if (dat != REMOTE_FRAME_HEAD) {
            return;
        }
    }

    remote_rx_buf[remote_rx_index++] = dat;

    if (remote_rx_index >= REMOTE_FRAME_LEN) {
        remote_rx_index = 0;

        if (remote_rx_buf[0] != REMOTE_FRAME_HEAD) {
            return;
        }

        if (remote_checksum_ok(remote_rx_buf)) {
            remote_accept_frame(remote_rx_buf);
        }
        else {
            remoteControlStatus.checksum_error++;

            // 当前字节流可能已经错位，下一帧重新等 0xA3 帧头。
            if (dat == REMOTE_FRAME_HEAD) {
                remote_rx_buf[0] = dat;
                remote_rx_index = 1;
            }
        }
    }
}

// 1ms 调用一次：处理失联保护、按键使能和摇杆到目标值的映射。
void RemoteControl_Timer1ms(void)
{
    if (remoteControlStatus.lost_ms < 60000u) {
        remoteControlStatus.lost_ms++;
    }

    if (remoteControlStatus.lost_ms > REMOTE_LOST_TIMEOUT_MS) {
        remoteControlStatus.online = 0;
        remoteControlStatus.speed_set = 0.0f;
        remote_reset_steer_filter();
        icarStr.SpeedSet = 0.0f;
        SERVO_Center();

        // 失联后关闭电机使能，避免恢复前继续沿用旧的运行状态。
        if (!remote_lost_protected) {
            piddebug.MotorEnable = false;
            remote_lost_protected = 1;
        }
        return;
    }

    if (!remote_has_valid_packet) {
        return;
    }

    uint8 key0 = remote_latest_packet.key[0] ? 1 : 0;
    uint8 key1 = remote_latest_packet.key[1] ? 1 : 0;

    // 左摇杆按键上升沿切换电机使能，右摇杆按键作为急停。
    if (key0 && !remote_last_key0) {
        piddebug.MotorEnable = !piddebug.MotorEnable;
    }
    remote_last_key0 = key0;

    if (key1) {
        piddebug.MotorEnable = false;
        icarStr.SpeedSet = 0.0f;
        remote_reset_steer_filter();
        SERVO_Center();
    }

    float throttle = remote_limit_float((float)remote_latest_packet.joystick[1] / REMOTE_JOY_MAX, -1.0f, 1.0f);
    int16 steer_raw = remote_latest_packet.joystick[2];

    // 右摇杆方向加入死区，先吃掉零点附近的小抖动。
    if (steer_raw > -REMOTE_STEER_DEADBAND_RAW && steer_raw < REMOTE_STEER_DEADBAND_RAW) {
        steer_raw = 0;
    }

    float steer = remote_limit_float((float)steer_raw / REMOTE_JOY_MAX, -1.0f, 1.0f);

    // 一阶低通滤波，让舵机目标平滑追随摇杆，不跟着每帧噪声抖。
    remote_steer_filtered += (steer - remote_steer_filtered) * REMOTE_STEER_FILTER_ALPHA;

    float speed_set = throttle * REMOTE_MAX_SPEED_MPS * REMOTE_SPEED_DIR;
    float steer_offset = remote_steer_filtered * REMOTE_MAX_STEER_DEG * REMOTE_STEER_DIR;

    remoteControlStatus.speed_set = speed_set;
    remoteControlStatus.steer_offset = steer_offset;

    if (!key1) {
        icarStr.SpeedSet = speed_set;

        // 目标角变化很小时不重复刷新舵机，减少机械来回找位置。
        if (remote_abs_float(steer_offset - remote_last_servo_offset) >= REMOTE_STEER_MIN_CHANGE_DEG) {
            remote_last_servo_offset = steer_offset;
            SERVO_SetOffset(steer_offset);
        }
    }
}

uint8 RemoteControl_IsOnline(void)
{
    return remoteControlStatus.online;
}

int16 RemoteControl_GetThrottleRaw(void)
{
    return remoteControlStatus.throttle_raw;
}

int16 RemoteControl_GetSteerRaw(void)
{
    return remoteControlStatus.steer_raw;
}

uint8 RemoteControl_GetKeyMask(void)
{
    return remoteControlStatus.key_mask;
}

uint8 RemoteControl_GetModeMask(void)
{
    return remoteControlStatus.mode_mask;
}

uint16 RemoteControl_GetLostMs(void)
{
    return remoteControlStatus.lost_ms;
}

uint32 RemoteControl_GetValidCount(void)
{
    return remoteControlStatus.valid_count;
}

uint32 RemoteControl_GetChecksumError(void)
{
    return remoteControlStatus.checksum_error;
}

float RemoteControl_GetSpeedSet(void)
{
    return remoteControlStatus.speed_set;
}

float RemoteControl_GetSteerOffset(void)
{
    return remoteControlStatus.steer_offset;
}
