#include "app_Version.h"

extern uint8_t g_iLcdType;

//发送基本信息
void SendInformation(void)
{
		Uart_SendDataPoll(M0P_UART0,'H');
		Uart_SendDataPoll(M0P_UART0,HardWare_Ver);
		Uart_SendDataPoll(M0P_UART0,SoftWare_Ver);
		Uart_SendDataPoll(M0P_UART0,0);
		g_iLcdType = GetLcdType();
//		g_iLcdType=1;	
		Uart_SendDataPoll(M0P_UART0,g_iLcdType);
		Uart_SendDataPoll(M0P_UART0,0x52);
		Uart_SendDataPoll(M0P_UART0,'Q');
	
}

//发送基本信息
void GpSendInformation(void)
{
		Uart_SendDataPoll(M0P_UART0,0x5A);
		Uart_SendDataPoll(M0P_UART0,0x01);
		Uart_SendDataPoll(M0P_UART0,'H');
		Uart_SendDataPoll(M0P_UART0,HardWare_Ver);
		Uart_SendDataPoll(M0P_UART0,SoftWare_Ver);
		Uart_SendDataPoll(M0P_UART0,0);
		g_iLcdType = GetLcdType();
		g_iLcdType=1;	
		Uart_SendDataPoll(M0P_UART0,g_iLcdType);
		Uart_SendDataPoll(M0P_UART0,0x52);
		Uart_SendDataPoll(M0P_UART0,'Q');
		Uart_SendDataPoll(M0P_UART0,0xA5);
	
}











