# 项目版本日志（2026-04-13 ~ 2026-04-14）

## 文档说明

这份日志用于整合：

- `2026-04-13` 的项目梳理与第一轮 `yaw` 调试改动
- `2026-04-14` 的智能车相对 `yaw` 控制方案演进、版本 A / 版本 B 的对比试验，以及最终选定方案

本文档为**中文完整版本日志**，作为当前项目的连续开发记录。  
昨天生成的英文版日志文件 `VERSION_LOG_2026-04-13.md` 仍然保留，本文件将其内容整合并补充今天的所有关键变化。

---

## 2026-04-13 版本记录

### 1. 项目理解

确认本项目是一个基于 **Infineon AURIX TC264D** 的嵌入式工程，使用逐飞 / ADS / TASKING 工具链，主业务代码位于：

- `code/`
- `user/`

运行结构确认如下：

- `CPU0` 负责初始化和前台主循环
- `CPU1` 负责 IMU 初始化和姿态更新
- 电机 / 舵机实时动作主要依赖定时中断，不依赖主循环阻塞执行
- 原始 `yaw` 来源于 `code/attitude_solution.c`

### 2. 发现的原始问题

原始姿态链路里，`yaw` 存在几类明显风险：

- 陀螺比例系数写死，和 IMU 库默认量程不一致
- 时间步长 `delta_T` 固定写死，不是真实循环周期
- 陀螺零偏初始化过于脆弱
- 调试信息不足，难以直接观察 `gyro_z raw / bias / corr / dt / yaw`

### 3. 第一轮姿态模块改动

涉及文件：

- `code/attitude_solution.c`
- `code/attitude_solution.h`
- `code/top.c`

主要改动：

- 增加 `attitudeDebug` 调试结构体，暴露：
  - `dt_s`
  - `gyro_z_raw_dps`
  - `gyro_z_bias_dps`
  - `gyro_z_corr_dps`
  - `yaw_continuous_deg`
  - `yaw_wrapped_deg`
  - `calibrated`
  - `stationary`
- 陀螺和加速度换算改为直接使用 IMU 库宏：
  - `imu963ra_gyro_transition(...)`
  - `imu963ra_acc_transition(...)`
- 时间步长改为使用 `STM` 实测，并加入上下限钳位
- 启动零偏校准改为固定 `2 秒窗口`
- 静止判据改为：
  - `|gyro_x/y/z| < 3 dps`
  - 加速度模长在 `0.95g ~ 1.05g`
- 校准成功后给出 `calib=OK`，否则显示 `calib=NO`

### 4. 第一轮显示页改动

将 `top.c` 的调试显示切换为姿态调试页，显示：

- `yaw`
- `yaw_cont`
- `gyro_z`
- `bias_z`
- `corr_z`
- `dt`
- `calib`
- `still`
- `pitch`
- `roll`

### 5. 当日验证结果

完成了文本级检查：

- 新增符号导出正确
- STM 接口名存在
- IMU 转换宏存在
- 显示页引用正常

但**没有完成真实编译验证**，原因是当前命令行环境缺少 TASKING 编译器 `cctc` 的 PATH。  
`mingw32-make.exe -C Debug all -j2` 报错为：

- `CreateProcess(... cctc ...) failed`
- `系统找不到指定的文件`

### 6. 当日结果总结

到 `2026-04-13` 结束时，项目已具备：

- 更可靠的启动零偏校准
- 更正确的陀螺物理量换算
- 更真实的积分 `dt`
- 更好的姿态调试显示

但当时还没有把 `yaw` 真正切到**智能车控制专用的相对转角主线**。

---

## 2026-04-14 版本记录

### 1. 需求进一步明确

今天明确了两个关键需求：

- 当前主要用于**智能车控制**
- 当前主要只需要 **`yaw` 值**

进一步讨论后，确定现在更需要的是：

- **相对转角型 `yaw`**

而不是：

- 长时间不漂移的绝对航向

因此今天的开发重点从“通用姿态角显示”转向“智能车控制专用相对 `yaw`”。

### 2. 版本 A 与版本 B 的演进

今天实际上走了两轮方案：

#### 版本 A

核心思路：

- 直接使用 `gyro_z_corr_dps * dt_s` 积分为连续相对角度
- `attitudeDebug.yaw_continuous_deg` 作为主控制值
- `yaw_wrap` 仅作为显示值
- 舵机控制直接读取连续相对 `yaw`

优点：

- 链路短
- 手感直接
- 转角控制响应好

缺点：

- 控制方向容易和传感器符号绑定
- 静止时仍可能出现慢漂

你在现场使用后反馈：**这个版本“感觉效果很好”**。

#### 版本 B

在版本 A 基础上，继续加入工程保护：

- 控制侧和显示侧符号解耦
- 小角速度积分死区
- 静止时 `Z` 轴运行中慢速 bias 修正

你后来决定：

- **最终采用版本 B 作为项目最终的 `yaw` 解法**

### 3. 最终选定的 `yaw` 方案：版本 B

当前最终方案如下：

#### 3.1 控制主值

主控制角仍然是：

- `attitudeDebug.yaw_continuous_deg`

它来源于：

- `gyro_z_corr_dps * dt_s` 连续积分

#### 3.2 显示与控制解耦

为了保证：

- 屏幕读数保持直观
- 控制方向单独修正

当前做法是：

- 屏幕上 `yaw_rel` 继续显示连续相对角
- 控制侧通过 `YAW_CONTROL_SIGN (-1.0f)` 单独取反后送进舵机

这样做的目的：

- 保留“实际左转时数值变化”的调试直觉
- 不让控制方向问题和显示方向问题绑死在一起

涉及文件：

- `code/Servo.c`

#### 3.3 积分死区

新增：

- `ATTITUDE_YAW_INTEGRATION_DEADBAND_DPS = 0.20f`

含义：

- 当 `corr_z` 很小的时候，先不把这些微小噪声积分进 `yaw_rel`

目的：

- 减少静止时微小抖动被长期积分成角度漂移

#### 3.4 静止时 Z 轴运行中慢速 bias 修正

新增：

- `ATTITUDE_RUNTIME_BIAS_TC_S = 5.0f`
- `ATTITUDE_RUNTIME_BIAS_ENABLE_DPS = 1.0f`

含义：

- 当系统判断当前静止，且 `corr_z` 足够小，就缓慢修正 `GyroOffset.Zdata`

作用：

- 让静止时 `corr_z` 慢慢更接近 0
- 让 `yaw_rel` 的静止漂移速度降低

### 4. 三圈计数功能

今天新加入了比赛相关的计圈逻辑。

#### 4.1 新增状态结构

在 `code/attitude_solution.h` 中新增：

- `attitude_lap_state_t`

包含字段：

- `lap_count`
- `target_laps`
- `race_finished`
- `start_zone_gate_enabled`
- `start_zone_active`
- `lap_base_yaw_deg`

并导出全局状态：

- `attitudeLapState`

#### 4.2 计圈规则

计圈逻辑放在 `yaw` 更新链路中，与 `ICM_getEulerianAngles()` 同步执行。

基本规则：

- 重置时记录 `lap_base_yaw_deg`
- 后续按连续相对角与起始值的偏差进行计圈
- 每圈目标角度为：
  - 第 1 圈：`360°`
  - 第 2 圈：`720°`
  - 第 3 圈：`1080°`
- 容差窗口：
  - `ATTITUDE_LAP_YAW_WINDOW_DEG = 20°`
- 当前目标圈数：
  - `ATTITUDE_DEFAULT_TARGET_LAPS = 3`

到达第 3 圈时：

- `lap_count` 封顶
- `race_finished = true`

不会自动做这些事：

- 不自动停电机
- 不自动修改速度
- 不自动退出控制

#### 4.3 当前方向判定特性

当前代码使用了：

- `fabsf(attitudeDebug.yaw_continuous_deg - attitudeLapState.lap_base_yaw_deg)`

这意味着：

- 计圈按**累计转角绝对值**判断
- 顺时针和逆时针都能加圈
- 不是“只允许逆时针计圈”

### 5. 起点区域门控接口

为了防止：

- 起点在线段中间
- 赛道布局导致只靠 yaw 判圈可能误判

今天额外预留了 3 个接口：

- `attitude_lap_reset()`
- `attitude_lap_set_start_zone(bool active)`
- `attitude_lap_enable_start_zone_gate(bool enable)`

这些接口都已经写入 `code/attitude_solution.h`，并附带了**面向后续维护的中文说明**。

接口设计意图：

- 现在先按 yaw-only 自动计圈
- 以后如果需要接入：
  - 摄像头
  - 路径状态机
  - 地磁点
  - 红外
  - 地标检测
- 可以直接把“是否进入起点区域”的判断结果喂给这个接口，而不用重构计圈主逻辑

### 6. 调试显示页扩展

`top.c` 保留了原有姿态调试内容，并新增：

- `lap=%u/%u`
- `finish=YES/NO`

这样现在屏幕上可以同时看：

- `yaw_rel`
- `yaw_wrap`
- `gyro_z`
- `bias_z`
- `corr_z`
- `dt`
- `calib`
- `still`
- `pitch`
- `roll`
- `lap`
- `finish`

### 7. 今天涉及的主要文件

- `code/attitude_solution.c`
- `code/attitude_solution.h`
- `code/Servo.c`
- `code/top.c`

### 8. 今天的状态总结

到 `2026-04-14` 结束时，当前项目的最终状态是：

- 最终 `yaw` 方案采用 **版本 B**
- `yaw_rel` 仍是智能车主控制角
- 控制方向和显示方向解耦
- 具备积分死区和静止慢速 `bias_z` 修正
- 已加入三圈计数状态
- 已预留起点区域门控接口
- 已在显示页加入圈数和比赛完成标志

---

## 当前代码最终状态（截至 2026-04-14）

### 当前最终功能

- 启动 `2 秒` 陀螺零偏校准
- 使用实测 `dt`
- `yaw_rel` 作为连续相对角
- `yaw_wrap` 作为显示包角
- 版本 B 的静止慢漂抑制
- 三圈计数
- 起点门控预留接口

### 当前已修改但未在此环境中完成真实编译验证的文件

- `code/Servo.c`
- `code/attitude_solution.c`
- `code/attitude_solution.h`
- `code/top.c`

### 当前仍然存在的环境限制

当前命令行环境仍无法完成真实构建验证，原因不变：

- `TASKING` 编译器 `cctc` 不在当前终端 PATH 中

因此当前能确认的是：

- 代码逻辑已经按方案落地
- 文本级检查已完成

但还不能在本终端里确认：

- 完整编译通过
- 下载通过
- 上板运行通过

---

## 后续建议

### 比赛前建议重点验证

1. `yaw_rel` 的正负方向是否和你的赛道控制逻辑一致
2. 静止时 `yaw_rel` 的慢漂是否在可接受范围内
3. 三圈计数是否会在你的赛道实际跑法下正确触发
4. 如果起点在线段中间，是否需要尽快把赛道状态接入 `start_zone` 接口

### 后续可能继续改的方向

- 将顺 / 逆时针计圈规则分开
- 将 `start_zone` 接口接入真实赛道检测
- 第 3 圈完成后自动做停车或减速动作
- 根据比赛规则增加计圈去抖与防重复触发逻辑

---

## 备注

- 本文件为截至 `2026-04-14` 的中文完整版本日志。
- 昨天的英文日志 `VERSION_LOG_2026-04-13.md` 仍保留，作为原始记录。
- 以后如继续开发，建议在本文件后续追加新日期段落，保持同一份总日志连续维护。

---

## 2026-04-22 编码器迁移与验证准备

### 任务目标
- 将 `car` 工程中的旧编码器读取资源从与电机 PWM 冲突的引脚组合中迁出。
- 保持原有上层 PID、PWM、电机控制、舵机控制和控制周期不变。
- 只替换编码器初始化、读取、清零和速度反馈这一层。
- 为上板联调准备左右轮增量、左右轮速度和控制周期的 LCD 观察入口。

### 修改文件列表
- `code/Encoder.h`
- `code/Encoder.c`
- `code/common.h`
- `code/protocol.c`
- `code/top.c`

### 关键改动说明
- `code/Encoder.h`
  - 将右轮编码器资源从 `TIM2/P33_7/P33_6` 改为 `TIM5/P10_3/P10_1`，避开 `P33_6` 与右电机 PWM 的冲突。
  - 保留左轮编码器资源 `TIM4/P02_8/P00_9` 不变。
  - 新增 `ENCODER_RIGHT_DIR_SIGN` 与 `ENCODER_LEFT_DIR_SIGN`，用于统一处理方向反向问题。
- `code/Encoder.c`
  - 保持 `ENCODER_Init()` 和 `ENCODER_RevSample()` 原函数名不变。
  - `ENCODER_RevSample()` 继续在同一入口完成左右轮计数读取、清零、PID 反馈写入和整车速度反馈更新。
  - 左右轮编码器增量改为通过方向符号宏统一处理，便于上板后快速修正方向。
- `code/common.h`
  - 同步更新重复定义的编码器资源宏，避免不同头文件中的资源定义不一致。
- `code/protocol.c`
  - 启动命令 `START_CMD` 下将编码器清零从单轮改为左右轮同时清零，避免首次启动时左右反馈不同步。
- `code/top.c`
  - 复用已有 `ips200pro_label_printf` 屏幕输出接口。
  - 在 `control()` 中加入左右轮编码器增量 `encL/encR`、左右轮速度 `spdL/spdR`、PID 参考值与反馈值、PWM 值以及控制周期状态 `motor_dt/cnt` 的实时显示。
  - 额外新增本地函数 `encoder_delta_to_speed_mps()`，仅用于显示层换算左右轮速度，不改变上层控制逻辑。

### 未完成事项
- 尚未在当前终端环境中完成真实 ADS/TASKING 编译验证。
- 尚未在实车上确认左右轮方向符号是否与当前 `ENCODER_RIGHT_DIR_SIGN = -1`、`ENCODER_LEFT_DIR_SIGN = -1` 一致。
- `icarStr.SpeedFeedback` 仍沿用旧逻辑，仅使用左轮反馈代表整车速度，本次未改上层约定。
- 现有源码文件存在历史编码差异，部分中文注释显示可能异常，但本次逻辑改动点已单独确认。

### 上板验证步骤
1. 上电后观察 LCD：
   - `encL`、`encR`
   - `spdL`、`spdR`
   - `fb`、`set`
   - `motor_dt=10.0ms cnt=x`
2. 静止不动时确认：
   - `encL`、`encR` 接近 `0`
   - `spdL`、`spdR` 接近 `0`
3. 手动或低速驱动车辆前进时确认：
   - 左右轮计数都在变化
   - 左右轮符号与预期前进方向一致
4. 手动或低速驱动车辆反转时确认：
   - 左右轮计数符号相对前进时整体翻转
5. 观察控制节拍：
   - `motor_dt` 应保持 `10.0ms`
   - `cnt` 应在 `0~9` 循环变化
6. 若某一侧方向相反：
   - 仅修改 `code/Encoder.h` 中对应的 `ENCODER_RIGHT_DIR_SIGN` 或 `ENCODER_LEFT_DIR_SIGN`
   - 不要改 PID 参数和上层控制流程






   
后来根据你上板反馈继续改的内容
这些目前还没提交，工作区里有 4 个文件是改动状态：

car/code/Encoder.h

根据你实际接线，改成：

#define ENCODER_1                   (TIM6_ENCODER)
#define ENCODER_1_A                 (TIM6_ENCODER_CH1_P20_3)
#define ENCODER_1_B                 (TIM6_ENCODER_CH2_P20_0)

#define ENCODER_3                   (TIM5_ENCODER)
#define ENCODER_3_A                 (TIM5_ENCODER_CH1_P10_3)
#define ENCODER_3_B                 (TIM5_ENCODER_CH2_P10_1)

#define ENCODER_RIGHT_DIR_SIGN      (1)
#define ENCODER_LEFT_DIR_SIGN       (-1)
也就是现在定义为：

右轮 A=P20_3，B=P20_0，用 TIM6
左轮 A=P10_3，B=P10_1，用 TIM5
右轮方向已经从 -1 改成 1，因为你说右轮正转时 encR 是负的
左轮方向暂时还是 -1，等左轮稳定后再看符号是否需要改
car/code/common.h

同步改了同一组编码器宏。这个文件里也有一份编码器定义，为了避免以后有人 include 到 common.h 时还是旧资源，所以一起保持一致。

car/code/Encoder.c

一开始右轮 TIM6 用 encoder_quad_init() 时，LCD 上 pinR A/B 会变但 encR 不变。查驱动后发现库里注释写着“完全正交采集仅 TIM2/TIM3/TIM4 支持”，所以把右轮改成方向计数模式：

encoder_dir_init(ENCODER_1, ENCODER_1_A, ENCODER_1_B);
后来你说右轮数值正常但符号反，说明这个方案对右轮有效。

左轮慢转直接跳到 3900 左右，说明 TIM5 的正交模式也不可靠，所以也把左轮从：

encoder_quad_init(ENCODER_3, ENCODER_3_A, ENCODER_3_B);
改成：

encoder_dir_init(ENCODER_3, ENCODER_3_A, ENCODER_3_B);
读取逻辑没改，仍然是：

motorStr.EncoderValue_R = ENCODER_RIGHT_DIR_SIGN * encoder_get_count(ENCODER_1);
encoder_clear_count(ENCODER_1);

motorStr.EncoderValue_L = ENCODER_LEFT_DIR_SIGN * encoder_get_count(ENCODER_3);
encoder_clear_count(ENCODER_3);
所以 PID 反馈入口没变：

L_motor.vi_FeedBack = motorStr.EncoderValue_L;
R_motor.vi_FeedBack = motorStr.EncoderValue_R;
car/code/top.c

为了查右轮为什么 pinR 有变化但 encR 没变化，我加了原始引脚电平显示：

uint8 enc_r_a = gpio_get_level(P20_3);
uint8 enc_r_b = gpio_get_level(P20_0);
uint8 enc_l_a = gpio_get_level(P10_3);
uint8 enc_l_b = gpio_get_level(P10_1);
LCD 现在会显示：

pinR A=%u B=%u
pinL A=%u B=%u
这个很有用：
如果 pinR A/B 变但 encR 不变，就是定时器计数问题；如果 pinR A/B 都不变，就是接线/供电/共地问题。

今天解决的问题

找到了旧右轮编码器和 PWM 的资源冲突。
把编码器读取入口保留在原 ENCODER_Init() / ENCODER_RevSample()，没有动上层 PID。
根据你的实际接线，把左右轮资源映射改成：
右：TIM6/P20_3/P20_0
左：TIM5/P10_3/P10_1
通过 LCD 原始 pin 显示，确认右轮信号进了单片机。
把右轮从正交模式改成方向计数模式后，encR 已经能正常显示两三位数。
修正了右轮方向符号。
左轮因为 TIM5 正交跳变，也已经计划性改成了方向计数模式。
现在还没完全验证的点

左轮改成 encoder_dir_init() 后，还需要你上板确认 encL 是否不再跳到 3900。
左轮方向还没确认。如果左轮正转显示负数，就只改 ENCODER_LEFT_DIR_SIGN。
速度物理量还没重新标定。现在只是先让编码器增量稳定，spdL/spdR 的绝对值是否准确还要后面再校。
当前 4 个文件还没提交：
M car/code/Encoder.c
M car/code/Encoder.h
M car/code/common.h
M car/code/top.c
一句话总结：今天我们把编码器迁移从“接上但读不准”推进到了“右轮已经能正常计数，只剩左轮方向计数模式上板确认”。当前最关键的下一步就是烧录后看左轮 encL 是否稳定，如果稳定，再只调 ENCODER_LEFT_DIR_SIGN 和后续速度标定。