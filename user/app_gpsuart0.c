#include "app_gpsuart0.h"
#include "app.lpuart.h"
#include "app_uart0.h"
#include "bsp_touch.h"	
#include "app_gprsCmd.h"
#include "uart.h"
#include "queue.h"
#include "app_Version.h"
#include "app_Cmd.h"
#include "bsp_flash.h"
#include "lpuart.h"
#define SYSFLASHADRR   0xFE00                                 //写升级标志FLAsh地址
uint8_t  GpFlashWSAsk[]={0x5A,0x02,0xFE,0xFE,0xA5};
uint8_t  GpToggleAsk[]={0x5A,0x01,0xFE,0x51,0xA5};
uint8_t  GpSysRest[]={0x5A,0x01,0x52,0xA5};
void GpSend_Uart0(const uint8_t *val,size_t num)
{
	uint8_t i;
	for(i=0;i<num;i++)
	{
			Uart_SendDataPoll(M0P_UART0,val[i]);
	}
}

void Send_LPUart0(const uint8_t *val,size_t num)
{
	uint8_t i;
	for(i=0;i<num;i++)
	{
			LPUart_SendData(M0P_LPUART0,val[i]);
	}
}
/************************数据转义***************************************/
uint8_t SendData_escape(ElemType len,ElemType *forsize1,ElemType *forsize2)
{
		uint16_t SendCout=0;
		uint16_t i=0;
	for(i=0;i<len;i++)
	{
			if(forsize1[i]==0x5A)
			{
					forsize2[SendCout]=0x80;
					SendCout++;
					forsize2[SendCout]=0x01;
					SendCout++;
			}
			else if(forsize1[i]==0xA5)
			{
						forsize2[SendCout]=0x80;
						SendCout++;
						forsize2[SendCout]=0x02;
						SendCout++;
			}
			else if(forsize1[i]==0x80)
			{
					forsize2[SendCout]=0x80;
					SendCout++;
					forsize2[SendCout]=0x03;
					SendCout++;
			}
			else
			{
				forsize2[SendCout]=forsize1[i];
				SendCout++;
			}
	 }
	return SendCout;	
}

/******发送触控数据XY到主控**********************/
void GpSend_XY(uint8_t CC)
{
		uint8_t val_Touch[]={0x00,0x00,'T',0x00,0x00,0x00,0x00,0x00,'S',0x00};
		uint8_t val_Touch2[20]={0};
		uint8_t Coutlen=0;
		if(CC=='P')
		{
				val_Touch[3]=(X_Touch_val>>8);
				val_Touch[4]=(X_Touch_val&0xff);
				val_Touch[5]=(Y_Touch_val>>8);
				val_Touch[6]=(Y_Touch_val&0xff);
		}
		else if(CC=='R')
		{
				val_Touch[3]=0x00;
				val_Touch[4]=0x00;
				val_Touch[5]=0x00;
				val_Touch[6]=0x00;
		}		
		val_Touch[7]=CC;	
		Coutlen=SendData_escape(sizeof(val_Touch),val_Touch,val_Touch2);
		val_Touch2[0]=0x5A;
		val_Touch2[1]=0x01;
		val_Touch2[Coutlen-1]=0xA5;
		GpSend_Uart0(val_Touch2,Coutlen);
}

/***********发送从Flash读出的数据到主控************/
static void GpSend_Flash_rD(void)
{
		uint8_t val_F[]={0x5A,0x02,0xFE,0x02,0xA5};
		uint16_t RCoutlen=0;
		uint16_t val_FF1=0;
		val_FF1|=((Flash_readBy(flashaddr))<<8);                                   //从Flash拿出数据长度
		val_FF1|=Flash_readBy(flashaddr+1);                                        //从Flash拿出数据长度
		val_FF1=(val_FF1+4)*2;
		uint8_t val_Tog[val_FF1];
		if(val_FF1==0xffff)
		{
			Send_Uart0(val_F,sizeof(val_F));
		}
		else
		{
			RCoutlen=SendData_escape(val_FF1/2,Rflashdtata,val_Tog);
			val_F[3]=0x01;
			GpSend_Uart0(val_F,sizeof(val_F)-1);
			GpSend_Uart0(val_Tog,RCoutlen);
			Uart_SendDataPoll(M0P_UART0,val_F[4]);
			
		}	
}	


void GpsSend_Data_Uart(void)
{
	
		if(TouchSbit==1)
		{
			
			GpSend_XY('P');
			TouchSbit=0;
		}
		else if(TouchUbit==1)
		{
			GpSend_XY('R');
			TouchUbit=0;
		}
		else if(GpVersion_Inquire==1)
		{
			GpSendInformation();
			GpVersion_Inquire=0;
		}
		else if(GpKey_Inquire==1)
		{
				GPSendKeyInfo();
				GpKey_Inquire=0;
		}
		else if(GpW_Flash_s==1)
		{
				GpSend_Uart0(GpFlashWSAsk,sizeof(GpFlashWSAsk));
				GpW_Flash_s=0;
		}
		else if(GpR_Flash_s==1)
		{
			GpSend_Flash_rD();
			GpR_Flash_s=0;
		}
		else if(ToggleAN==1)
		{
			GpSend_Uart0((const uint8_t *)GpToggleAsk,sizeof(GpToggleAsk));
			ToggleAN=0;	
		}
		else if(Penetratebit==1)
		{
				Penetratebit=0;
				Send_LPUart0(Gps_TranslatePackDtata,Gps_RevCount-NumberEscapes-3);
				memset(Gps_TranslatePackDtata,0,sizeof(Gps_TranslatePackDtata));
				GPclearData();
		}
		else if(GpSystemRest==1)
		{
				GpSystemRest=0;
				while(Ok != Flash_SectorErase(SYSFLASHADRR)){;}
				Flash_writeBy(SYSFLASHADRR,0xA0);
				Flash_writeBy(SYSFLASHADRR+1,0x50);
				Flash_writeBy(SYSFLASHADRR+2,0xAA);
				Flash_writeBy(SYSFLASHADRR+3,0x00);
				GpSend_Uart0(GpSysRest,sizeof(GpSysRest));		
				NVIC_SystemReset();
		}
			
}
/*************协议交互状态扫描打断与恢复GPRS与主控的透传******************/
uint8_t ScanInteractiveState(void)
{
	uint8_t GpStabit[]={TouchSbit,TouchUbit,GpVersion_Inquire,GpKey_Inquire,\
											         GpW_Flash_s,GpR_Flash_s,ToggleAN,Penetratebit};
	uint8_t pi=0;
	for(pi=0;pi<sizeof(GpStabit);pi++)
	{
		if(GpStabit[pi]==1)
		{
			LPSendInterruptbit=1;
			return 0;
		}
	}									
	LPSendInterruptbit=0;
	return 0;
}

