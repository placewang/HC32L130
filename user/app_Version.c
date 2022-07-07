#include "app_Version.h"

extern uint8_t g_iLcdType;

//发送基本信息
void SendInformation(void)
{
		unsigned char TPS=1;
		unsigned char LCD_TYPE=0;
		g_iLcdType = GetLcdType();
		if(g_iLcdType!=0&&g_iLcdType&0x80)
		{
			TPS=4;
			LCD_TYPE=g_iLcdType&0x7F;
		}
		else
		{
			LCD_TYPE=g_iLcdType;
		}
		Uart_SendDataPoll(M0P_UART0,'H');
		Uart_SendDataPoll(M0P_UART0,HardWare_Ver);
		Uart_SendDataPoll(M0P_UART0,SoftWare_Ver);
		Uart_SendDataPoll(M0P_UART0,TPS);
		Uart_SendDataPoll(M0P_UART0,LCD_TYPE);
		Uart_SendDataPoll(M0P_UART0,0x52);
		Uart_SendDataPoll(M0P_UART0,'Q');
}

//发送基本信息
void GpSendInformation(void)
{
	
		unsigned char GPTPS=1;
		unsigned char GPLCD_TYPE=0;
		g_iLcdType = GetLcdType();
		if(g_iLcdType!=0&&g_iLcdType&0x80)
		{
			GPTPS=4;
			GPLCD_TYPE=g_iLcdType&0x7F;
		}
		else
		{
			GPLCD_TYPE=g_iLcdType;
		}
		Uart_SendDataPoll(M0P_UART0,0x5A);
		Uart_SendDataPoll(M0P_UART0,0x01);
		Uart_SendDataPoll(M0P_UART0,'H');
		Uart_SendDataPoll(M0P_UART0,HardWare_Ver);
		Uart_SendDataPoll(M0P_UART0,SoftWare_Ver);
		Uart_SendDataPoll(M0P_UART0,GPTPS);
		Uart_SendDataPoll(M0P_UART0,GPLCD_TYPE);
		Uart_SendDataPoll(M0P_UART0,0x52);
		Uart_SendDataPoll(M0P_UART0,'Q');
		Uart_SendDataPoll(M0P_UART0,0xA5);
}











