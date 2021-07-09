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

    delay_init();	    	      //��ʱ������ʼ��
    NVIC_Configuration(); 	  //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    uart_init(115200);	 	    //����1��ʼ��Ϊ115200
    USART2_init(115200);	 	  //����2��ʼ��Ϊ115200
		uart3_init(115200);	 	    //����3��ʼ��Ϊ115200	
    TIM3_Int_Init(4999,7199); //500ms
    printf("\r\n ############ http://www.csgsm.com/ EC20&EC200 ############\r\n ############("__DATE__ " - " __TIME__ ")############\r\n");
    LED_Init();								//��ʼ��PD2
    JDQ_Init();								//��ʼ��PC9
		RS485_MODE=1;
		Usart3_Send_Array("===RS485-4G-SERVER===\r\n",23);
		RS485_MODE=0;
    Pro_LTEState = LTE_DataUP();
    LTE_Check=1;
    while(1)
    {
				length = Usart3_ReadFrame(data,500);  	//485������������
        if(length) //�õ�485������������ ת����
        {
            printf("From 485 data��%s\r\n",data);
            LTE_SendData(data,strlen((const char *)data));
            length=0;
            memset(data,0X00,512);
        }

        if(Pro_LTEState==0xAA)	//ע�ᵽƽ̨�ɹ�
        {
            if(LTE_Check==1)		//��ʱ5S���� ׼���յ��·�����
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
            LTE_Close();						//�ر�TCP����
            LTE_Power_Off();				//�ر�ģ���Դ
            LTE_Delay_ms(1000);
            LTE_Delay_ms(1000);
            Pro_LTEState = LTE_DataUP();	//�������ӵ�ƽ̨
        }

        //      receive =	LTE_ReceiveData_Process(Receive_data,20);  //���շ�����������������
        //      LTE_Frame_Process(Receive_data,receive.r_length);	  //���ݲ�ͬ�����ִ����ز���
        //      memset(Receive_data,0,receive.r_length);
    }
}

