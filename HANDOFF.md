# Handoff

## 这次迁移完成了什么
- 右轮编码器资源从与 PWM 冲突的 `TIM2/P33_7/P33_6` 迁移到 `TIM5/P10_3/P10_1`。
- 左轮编码器继续使用 `TIM4/P02_8/P00_9`。
- 保留了原有 `ENCODER_Init()`、`ENCODER_RevSample()`、PID 和 10 ms 控制周期，不改上层流程。
- 在 `Encoder.h` 中加入了左右轮统一方向符号入口，方便上板后只改符号不改逻辑。
- `START_CMD` 时改为左右轮同时清零。
- LCD 已接入左右轮增量、左右轮速度、PID 反馈和控制周期显示，方便上板验证。

## 没完成什么
- 还没有在 ADS/TASKING 环境完成真实编译验证。
- 还没有上板确认左右轮方向符号是否正确。
- `icarStr.SpeedFeedback` 仍然沿用旧逻辑，只取左轮作为整车速度反馈。
- 旧文件存在历史编码差异，部分中文注释显示可能异常。

## 下一步先看哪里
- 先看 `code/Encoder.h`：确认编码器资源和 `ENCODER_RIGHT_DIR_SIGN` / `ENCODER_LEFT_DIR_SIGN`。
- 再看 `code/Encoder.c`：确认读取、清零、PID 反馈和速度反馈链路。
- 上板后优先看 `code/top.c` 对应的 LCD 显示：
  - `encL` / `encR`
  - `spdL` / `spdR`
  - `motor_dt=10.0ms cnt=x`
- 如果方向反了，只改 `Encoder.h` 里的方向宏，不要先动 PID。
