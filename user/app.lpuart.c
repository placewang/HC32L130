
#include "app.lpuart.h"
#include "queue.h"
#include "uart.h"
#include "lpuart.h"
#include "bsp_touch.h"

uint16_t LPrevouttiem=0;                  //���ճ�ʱ���
uint16_t GPRSDataLen=0;                   //Э�����ݳ���
uint8_t  GPRSDatabit=0;                   //Э�����ݳ��ȱ�־λ
uint8_t  Head_bit=0;                      //�յ�ͷЭ���־
uint8_t  Tailend_bit=0;                   //�յ�Э���������־
//uint8_t  GprsData_pack[17]={0};           //Э��ͷ �������� ת�����ݰ���һ��ת��7�ֽڣ�Ԥ��7���ֽ�ȷ������ת����Ͳ����� Э��β
uint16_t DataCountRev=0;                  //���ݽ��ռ���

uint8_t  Getpack_end=0;                   //ȡ����ɱ�־λ

uint8_t  LPSendLen=0;                       //ת���ʵ�ʷ��ͳ���

/***********************�������*********************************************/
void Clearpackbit(void)
{
		DataCountRev=0;
		Head_bit=0;
		GPRSDataLen=0;
		GPRSDatabit=0;	
}


/********************ɸѡЭ��ͷ�����ݳ���************************/
uint8_t Get_Data_HTL(ElemType * htl)
{
	
	if(*htl==0xAA&&Head_bit==0)
	{
			Head_bit=1;
	}
	else if(*htl==0x55&&Head_bit==1)
	{
			Head_bit=2;
	}
	else if(Head_bit==2&&GPRSDatabit==0)
	{
			GPRSDataLen|=*htl<<8;
			GPRSDatabit=1;
	}
	else if(Head_bit==2&&GPRSDatabit==1)
	{
			GPRSDataLen|=*htl;
			GPRSDatabit=2;
	}
	else if(GPRSDataLen<4&&GPRSDatabit==2)
	{
		Clearpackbit();	
	}	
	return 0;
}
/************************����ת��***************************************/
uint8_t Data_escape(ElemType * htlZ,ElemType *forsize1,ElemType *forsize2)
{

		if(*htlZ==0x5A)
		{
				*forsize1=0x80;
				*forsize2=0x01;
				return 1;
		}
		else if(*htlZ==0xA5)
		{
					*forsize1=0x80;	
					*forsize2=0x02;
					return 1;			
		}
		else if(*htlZ==0x80)
		{
				*forsize1=0x80;
				*forsize2=0x03;
				return 1;
		}
	
	return 0;	
}

/************************ȡ��ͷ���������******************************************/
uint8_t Getpackheadlen(void)
{
		if((Head_bit==1||Head_bit==2)&&GPRSDatabit==0)
		{
			DataCountRev++;
		}
		if((GPRSDatabit==1||GPRSDatabit==2)&&DataCountRev<4)
		{
			DataCountRev++;
		}
//		if(DataCountRev==4 && Head_bit!=2)
//		{
//				Clearpackbit();	
//		}
		if(Head_bit==2&&GPRSDatabit==2&&GPRSDataLen>=4)
		{
				Getpack_end=1;
				return 1;
		}
	return 0;
}
/**********************װ�ذ�ͷ����************************************************/
void SendpackHead(void)
{
		ElemType  prsdata;
		ElemType  val1,val2;
		if(Head_bit==2&&GPRSDatabit==2)
		{
						Uart_SendDataPoll(M0P_UART0,0x5A);
						Uart_SendDataPoll(M0P_UART0,0x03);
						Uart_SendDataPoll(M0P_UART0,0xAA);
						Uart_SendDataPoll(M0P_UART0,0x55);
						prsdata=(GPRSDataLen>>8);
						if(Data_escape(&prsdata ,&val1,&val2))
						{
									Uart_SendDataPoll(M0P_UART0,val1);
									Uart_SendDataPoll(M0P_UART0,val2);
						}
						else
						{
							Uart_SendDataPoll(M0P_UART0,prsdata);
						}
						prsdata=(GPRSDataLen&0xff);
						if(Data_escape(&prsdata ,&val1,&val2))
						{
									Uart_SendDataPoll(M0P_UART0,val1);
									Uart_SendDataPoll(M0P_UART0,val2);
						}
						else
						{
							Uart_SendDataPoll(M0P_UART0,prsdata);
						}
						
						GPRSDatabit=0;
						Head_bit=0;
			}
}
/**********************���ֽڷ���**************************************/

void Sendby(	ElemType  *Pdata)
{
	ElemType Z_data1,Z_data2;
	if(Data_escape(Pdata,&Z_data1,&Z_data2))
	{
				Uart_SendDataPoll(M0P_UART0,Z_data1);
				Uart_SendDataPoll(M0P_UART0,Z_data2);
	}
	else
	{
		Uart_SendDataPoll(M0P_UART0,*Pdata);
	}


}

/**********Э��͸����ת,����עЭ�鳤�Ⱥ�����ȷ��************************/
uint8_t GPRSRev_data(void)
{
//		static uint8_t Countpack=0;
//		static ElemType  LPREVData[7]={0};
		ElemType  gprsdata;

		
		if(!Getpack_end)
		{
					if(DeQueue(&LPUart_revQueue,&gprsdata))
					{	
							Get_Data_HTL(&gprsdata);
							if(Getpackheadlen())
							{
								LPrevouttiem=0;
								return 0;
							}			
							LPrevouttiem=0;
					}
					else if(DataCountRev>0&&LPrevouttiem>=50)                                          //����һ�����ݣ�������ʱ��
					{
							Clearpackbit();	
					}				
		}
		else if(Getpack_end==1&&LPSendInterruptbit==0)
		{
				SendpackHead();
				if(DeQueue(&LPUart_revQueue,&gprsdata))
				{	
						DataCountRev++;
						Sendby(&gprsdata);	
						if(GPRSDataLen+2==DataCountRev) 
						{
									Uart_SendDataPoll(M0P_UART0,0xA5);
									Getpack_end=0;
									Clearpackbit();
									return  0;
						}
						LPrevouttiem=0;
				}
				else if(GPRSDataLen+2!=DataCountRev&&!LPUart_GetStatus(M0P_LPUART0, LPUartRC))                                          //����һ�����ݣ�������ʱ��
				{
							Uart_SendDataPoll(M0P_UART0,0xA5);
							Getpack_end=0;
							Clearpackbit();	
				}	
		}	
	return 0;	
}	





















