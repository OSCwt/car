#include "PID_debug.h"
// ACTIVE_NEW_CAR: 当前仍使用该模块做 PID/编码器/电机初始化，以及上位机调参协议处理。
// KEEP_FOR_BUILD: SasuTimer.c、top.c、protocol.c 和主控状态都依赖 PID_debug 相关接口与 piddebug 全局状态。

// KEEP_FOR_BUILD: piddebug 被 Motor、RemoteControl、protocol、top、cpu0_main 等模块引用，不能直接删除。
PIDDebugStruct piddebug;

void PID_debug_Init()
{
    // ACTIVE_NEW_CAR: 当前初始化链路仍从这里初始化协议、UART0、PID、编码器和电机。
    protocol_init();
    uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);
    uart_rx_interrupt(UART_INDEX, 1);

    PID_Init();
//    LEGACY_OLD_CAR: Flash 参数加载来自旧车/旧调参流程，当前未启用。
//    FLASH_LoadAllConfig();
    ENCODER_Init();
    MOTOR_Init();
//    LEGACY_OLD_CAR: 旧车 ICAR 初始化当前未启用，速度状态由现有全局变量直接使用。
//    ICAR_Init();

    float pid_temp[3] = { R_motor.v_Kp, R_motor.v_Ki,  R_motor.v_Kd};
    set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, pid_temp, 3);
    // LEGACY_OLD_CAR: Flash 保存开关来自旧调参流程，当前未启用。
    //flashSaveEnable=true;

    piddebug.Counter = 0;
    piddebug.pidsend = false;
    piddebug.pidset = false;
    piddebug.Kp=0;
    piddebug.Ki=0;
    piddebug.Kp=0;
    piddebug.SpeedSet=0;
    piddebug.MotorEnable=false;
    piddebug.pwm=0;

}

void PID_debug_get_data(float p,float i,float d)
{
    // ACTIVE_NEW_CAR: 上位机下发 PID 参数后当前只写入右电机 PID，行为保持原样。

    piddebug.Kp=p;
    piddebug.Ki=i;
    piddebug.Kd=d;

    piddebug.pidset = true;

    //两个电机没必要同时调吧，你接的接口换一下不久行了吗？
    R_motor.v_Kp = piddebug.Kp;
    R_motor.v_Ki = piddebug.Ki;
    R_motor.v_Kd = piddebug.Kd;

}

void PID_get_target(float speed)
{
    // ACTIVE_NEW_CAR: 上位机目标速度最终写入 icarStr.SpeedSet，供 Motor.c 闭环使用。

    piddebug.SpeedSet=speed;
    icarStr.SpeedSet=piddebug.SpeedSet;

}

//void PID_WriteFlash()
//{
//    LEGACY_OLD_CAR: Flash 写入 PID 参数流程当前未启用，后续可随旧调参持久化功能隔离。
//    if(flash_check(FLASH_PID_SECTION, FLASH_PID_PAGE))                      // 判断是否有数据
//        flash_erase_page(FLASH_PID_SECTION, FLASH_PID_PAGE);                // 擦除这一页
//
//    flash_buffer_clear();
//    flash_union_buffer[PID_P].float_type = piddebug.Kp;
//    flash_union_buffer[PID_I].float_type = piddebug.Ki;
//    flash_union_buffer[PID_D].float_type = piddebug.Kd;
//    flash_union_buffer[PID_FLASH].uint8_type = FLASH_DATA_OK;
//
//    flash_write_page_from_buffer(FLASH_PID_SECTION, FLASH_PID_PAGE);
//    flash_buffer_clear();
//}

void PID_debug_Timer()
{
    // ACTIVE_NEW_CAR: 1ms 定时器中调用，用于节流上位机反馈发送周期。
    piddebug.Counter++;
    if( piddebug.Counter>=80)//80毫秒进行一次通信
    {
        piddebug.pidsend = true;
        piddebug.Counter=0;
    }
}

uint32_t a=FRAME_HEADER;

void PID_debug_Handle()
{
    // ACTIVE_NEW_CAR: 1ms 定时器中调用，处理上位机协议接收和反馈发送。
    receiving_process();
    if(piddebug.pidset == true)
    {
       //PID_WriteFlash();
       piddebug.pidset = false;
    }
    if(piddebug.pidsend ==true)
    {
        int temp1;
        int temp2;
        //set_computer_value(SEND_TARGET_CMD,CURVES_CH1,&(icarStr.SpeedSet),sizeof(icarStr.SpeedSet));
        temp1=(int)(100*icarStr.SpeedFeedback_R);
//        temp2=(int)(100*icarStr.SpeedFeedback_R);

//        if(temp1>=0)
//        {
            set_computer_value(SEND_FACT_CMD,CURVES_CH1,&temp1,sizeof(temp1));

//            temp=(int)(100*icarStr.SpeedSet);
//            set_computer_value(SEND_FACT_CMD,CURVES_CH2,&temp,sizeof(temp));
//        }
//        if(temp2>=0)
//        {
//            set_computer_value(SEND_FACT_CMD,CURVES_CH2,&temp2,sizeof(temp2));
//
////            temp=(int)(100*icarStr.SpeedSet);
////            set_computer_value(SEND_FACT_CMD,CURVES_CH2,&temp,sizeof(temp));
//        }
        temp1=0;
//        temp2=0;
        piddebug.pidsend = false;
    }

}
