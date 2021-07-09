#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "uart2.h"
#include "usart3.h"
#include "timer.h"
#include "led.h"
#include "general.h"
#include "CAT1.h"
#include "pro_nb.h"
#include "rtc.h" 
#include "string.h"
#include "stdio.h"
#include "stm32f10x_rcc.h" 
#include "stm32f10x_pwr.h"

LTE_Receive_Struct  receive;

int main(void)
{
    u8 data[512];
    u8 length=0;

    delay_init();	    	      //延时函数初始化
    NVIC_Configuration(); 	  //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
    uart_init(115200);	 	    //串口1初始化为115200
    USART2_init(115200);	 	  //串口2初始化为115200
		uart3_init(115200);	 	    //串口3初始化为115200	
    TIM3_Int_Init(4999,7199); //500ms
    printf("\r\n ############ http://www.csgsm.com/ EC20&EC200 ############\r\n ############("__DATE__ " - " __TIME__ ")############\r\n");
    LED_Init();								//初始化PD2
    JDQ_Init();								//初始化PC9
		RS485_MODE=1;
		Usart3_Send_Array("===RS485-4G-SERVER===\r\n",23);
		RS485_MODE=0;
    Pro_LTEState = LTE_DataUP();
    LTE_Check=1;
    while(1)
    {
				length = Usart3_ReadFrame(data,500);  	//485发过来的数据
        if(length) //得到485发过来的数据 转发走
        {
            printf("From 485 data：%s\r\n",data);
            LTE_SendData(data,strlen((const char *)data));
            length=0;
            memset(data,0X00,512);
        }

        if(Pro_LTEState==0xAA)	//注册到平台成功
        {
            if(LTE_Check==1)		//定时5S发送 准备收到下发数据
            {
                RS485_MODE=1;
								Usart3_Send_Array("RS485:SEND DATA TO PC\r\n",23);
								LTE_SendData("HEARTBEAT:www.csgsm.com\r\n",25);
								RS485_MODE=0;
                LTE_Check=0;
            }
        }
        else
        {
            LTE_Close();						//关闭TCP链接
            LTE_Power_Off();				//关闭模块电源
            LTE_Delay_ms(1000);
            LTE_Delay_ms(1000);
            Pro_LTEState = LTE_DataUP();	//重新链接到平台
        }

        //      receive =	LTE_ReceiveData_Process(Receive_data,20);  //接收服务器发过来的数据
        //      LTE_Frame_Process(Receive_data,receive.r_length);	  //根据不同的命令，执行相关操作
        //      memset(Receive_data,0,receive.r_length);
    }
}

