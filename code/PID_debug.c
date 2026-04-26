#include "PID_debug.h"

PIDDebugStruct piddebug;

void PID_debug_Init()
{
    protocol_init();
    uart_init(UART_INDEX, UART_BAUDRATE, UART_TX_PIN, UART_RX_PIN);
    uart_rx_interrupt(UART_INDEX, 1);

    PID_Init();
//    FLASH_LoadAllConfig();
    ENCODER_Init();
    MOTOR_Init();
//    ICAR_Init();

    float pid_temp[3] = { L_motor.v_Kp, L_motor.v_Ki,  L_motor.v_Kd};
    set_computer_value(SEND_P_I_D_CMD, CURVES_CH1, pid_temp, 3);
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

    piddebug.Kp=p;
    piddebug.Ki=i;
    piddebug.Kd=d;

    piddebug.pidset = true;

    //两个电机没必要同时调吧，你接的接口换一下不久行了吗？
    L_motor.v_Kp = piddebug.Kp;
    L_motor.v_Ki = piddebug.Ki;
    L_motor.v_Kd = piddebug.Kd;

}

void PID_get_target(float speed)
{

    piddebug.SpeedSet=speed;
    icarStr.SpeedSet=piddebug.SpeedSet;

}

//void PID_WriteFlash()
//{
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
        temp1=(int)(100*icarStr.SpeedFeedback_L);
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
