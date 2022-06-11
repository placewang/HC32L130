
#include "app_uart0.h"

#include "app_Version.h"
#include "app_Cmd.h"
#include "bsp_flash.h"	
#include "bsp_touch.h"																				
#include "bsp_uart.h"    										 

uint8_t  ToggleAsk[]={0x5A,0x01,0xFE,0x51,0xA5};							     //切换应答	
uint8_t  FlashWSAsk[]={0xFE,0xFE};							                   //Flash写成功应答	

void Send_Uart0(const uint8_t *val,size_t num)
{
	uint8_t i;
	for(i=0;i<num;i++)
	{
			Uart_SendDataPoll(M0P_UART0,val[i]);
	}
}

/***********发送从Flash读出的数据到主控************/
static void Send_Flash_rD(void)
{
		uint8_t val_F[]={0xFE,0x02};
		uint8_t val_FF1,val_FF2;
		val_FF1=Flash_readBy(flashaddr);                                     //从Flash拿出数据长度
		val_FF2=Flash_readBy(flashaddr+1);                                   //从Flash拿出数据长度
		if(val_FF1==0xff&&val_FF2==0xff)
		{
			Send_Uart0(val_F,sizeof(val_F));
		}
		else
		{
			val_F[1]=0x01;
			Send_Uart0(val_F,sizeof(val_F));
			Send_Uart0(Rflashdtata,FlashLen+4);
		}	
}	

/******发送触控数据XY到主控**********************/
void Send_XY(uint8_t CC)
{
		uint8_t val_Touch[]={'T',0x00,0x00,0x00,0x00,0x00,'S'};
		if(CC=='P')
		{
				val_Touch[1]=(X_Touch_val>>8);
				val_Touch[2]=(X_Touch_val&0xff);
				val_Touch[3]=(Y_Touch_val>>8);
				val_Touch[4]=(Y_Touch_val&0xff);
		}
		else if(CC=='R')
		{
				val_Touch[1]=0x00;
				val_Touch[2]=0x00;
				val_Touch[3]=0x00;
				val_Touch[4]=0x00;
		}		
		val_Touch[5]=CC;
		Send_Uart0(val_Touch,sizeof(val_Touch));
}


void Send_Data_Uart(void)
{		
		if(TouchSbit==1)
		{
			TouchSbit=0;
			Send_XY('P');
		}
		if(TouchUbit==1)
		{
			Send_XY('R');
			TouchUbit=0;
			LPSendInterruptbit=0;
		}
		if(Version_Inquire==1)
		{
			SendInformation();
			Version_Inquire=0;
		}
		else if(Key_Inquire==1)
		{
			SendKeyInfo();
			Key_Inquire=0;
		}
		else if(W_Flash_s==1)
		{
				Send_Uart0(FlashWSAsk,sizeof(FlashWSAsk));
				W_Flash_s=0;
		}
		else if(R_Flash_s==1)
		{
			Send_Flash_rD();
			R_Flash_s=0;
		}
		else if(ToggleAN==1)
		{
			Send_Uart0((const uint8_t *)ToggleAsk,sizeof(ToggleAsk));
			ToggleAN=0;	
		}

		
		
}





