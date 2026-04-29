#ifndef __REMOTE_CONTROL_H__
#define __REMOTE_CONTROL_H__

#include "zf_common_headfile.h"

// 遥控器 LoRa 透传帧固定参数，与 AT32F415 遥控器样例工程保持一致。
#define REMOTE_FRAME_HEAD           (0xA3)
#define REMOTE_FRAME_LEN            (18)

// 默认使用 UART2 接 LoRa 目标模块。如接线变化，只需要改这里的串口号和引脚。
#define REMOTE_UART_INDEX           (UART_2)
#define REMOTE_UART_BAUDRATE        (115200)
#define REMOTE_UART_TX_PIN          (UART2_TX_P10_5)
#define REMOTE_UART_RX_PIN          (UART2_RX_P10_6)

// 遥控控制参数：第一版保守限速，方便实车低速验证。
#define REMOTE_LOST_TIMEOUT_MS      (300u)
#define REMOTE_JOY_MAX              (2048.0f)
#define REMOTE_MAX_SPEED_MPS        (0.8f)
#define REMOTE_MAX_STEER_DEG        (10.0f)
#define REMOTE_SPEED_DIR            (-1.0f)
#define REMOTE_STEER_DIR            (1.0f)

// 遥控器样例工程发送的 18 字节数据包。
typedef struct
{
    uint8 head;             // 帧头，固定 0xA3。
    uint8 sum_check;        // 和校验，发送端先置 0 后累加整帧。
    int16 joystick[4];      // 摇杆值：0 左右，1 左上下，2 右左右，3 右上下。
    uint8 key[4];           // 按键值：0 左摇杆按键，1 右摇杆按键，2 左侧键，3 右侧键。
    uint8 switch_key[4];    // 四个拨码/开关值，当前先作为模式位调试显示。
} RemoteControlPacket;

// 解析后的调试状态，供屏幕显示和调参观察使用。
typedef struct
{
    int16 throttle_raw;     // 油门原始值，来自 joystick[1]。
    int16 steer_raw;        // 方向原始值，来自 joystick[2]。
    uint8 key_mask;         // 四个按键压成 bit0-bit3。
    uint8 mode_mask;        // 四个开关压成 bit0-bit3。
    uint16 lost_ms;         // 距离最近一次有效帧的时间，单位 ms。
    uint32 valid_count;     // 有效帧计数。
    uint32 checksum_error;  // 校验错误计数。
    uint8 online;           // 1 表示最近 300ms 内收到过有效帧。
    float speed_set;        // 遥控映射后的目标速度，单位 m/s。
    float steer_offset;     // 遥控映射后的舵机偏移角，单位 deg。
} RemoteControlStatus;

extern volatile RemoteControlStatus remoteControlStatus;

void RemoteControl_Init(void);
void RemoteControl_RecvByte(uint8 dat);
void RemoteControl_Timer1ms(void);

uint8 RemoteControl_IsOnline(void);
int16 RemoteControl_GetThrottleRaw(void);
int16 RemoteControl_GetSteerRaw(void);
uint8 RemoteControl_GetKeyMask(void);
uint8 RemoteControl_GetModeMask(void);
uint16 RemoteControl_GetLostMs(void);
uint32 RemoteControl_GetValidCount(void);
uint32 RemoteControl_GetChecksumError(void);
float RemoteControl_GetSpeedSet(void);
float RemoteControl_GetSteerOffset(void);

#endif
