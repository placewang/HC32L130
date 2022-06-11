#include "app_gpsuart0.h"
#include "app_gprsCmd.h"
#include "queue.h"
#include "app_Cmd.h"
#include "bsp_gpio.h"
#include "bsp_touch.h"
#include "bsp_flash.h"
#include "bsp_uart.h"

#define GpPOLY16 0x1021

uint8_t  Gps_PackDtata[200]={0};                    //���ݰ�ָ���
uint8_t  Gps_TranslatePackDtata[200]={0};           //���ݷ�����ָ���
uint16_t Gps_RevCount=0;                            //��Ч���ݽ��ռ��� 
uint8_t  GPHeadbit=0;                               //ȡ����ͷ��־λ
uint16_t GPRevoutTime=0;														//�����ճ�ʱ����	
uint8_t  GpTogshakeTime=0;                          //���������л����������ּ�ʱ   

uint8_t GpTouchRbit=0;                                 //���ض����ݱ�־λ
uint8_t GpVersion_Inquire=0;                           //�汾��ѯ��־λ
static uint8_t GetPackbit=0;                           //����һ����־λ
uint8_t GpW_Flash_s=0;                                 //��дFlash �ɹ���־λ 																						
uint8_t GpR_Flash_s=0;																					
uint8_t Penetratebit=0;																 //͸����־λ
uint8_t GpKey_Inquire=0;															 //������ѯ��־λ
uint16_t NumberEscapes=0;															 //ת�����	
uint8_t  GpSystemRest=0;                               //ϵͳ��λ������־λ
/****************��������ʾ��CMDִ��********************************/
void GpCmd_char(uint8_t * CmdData)	
{
	switch(*CmdData)
  {
				case 0xA0:	//��������
					BUZZ_ON();
					break;
				case 0x0A:	//��LCD�����
					Backlight_ON();
					break;
				case 0x50:  //�ط�����
					BUZZ_OFF();
					break;
				case 0x05: 	//��LCD�����
					Backlight_OFF();
					break;
				case 0x29:	//��ѯ����״̬
					GpKey_Inquire=1;
					TouchRbit=0;
					Rev_Touch_XY();
					break;
				case 0x77:
					GpVersion_Inquire=1;
					break;
				case 0x51:
					Toggle=2;
					Lpuart_init(115200);
//					ToggleAN=1;                                                               //�汾ת��Ӧ�� 				
					break;
    		case 0x52:
					GpSystemRest=1;
					break;
		}
}


/********��ʱ�����ݴ��������********/
void GPclearData(void)
{
		memset(Gps_PackDtata,0,sizeof(Gps_PackDtata));
		Gps_RevCount=0;
		GPHeadbit=0;
		GPRevoutTime=0;
		GetPackbit=0;
}	


/************���ݷ���******************/
uint8_t GPdataTranslate(ElemType* Source,ElemType* Target)
{
	uint16_t i=0,t=0; 
	for(i=0;i<(Gps_RevCount-1-2);i++)                          //-1 �õ�����β��
	{
				if(Source[2+i]==0x80)
				{	
						if(Source[2+i+1]==0x01)
						{
							Target[t]=0x5A;
							i++;
							t++;
							NumberEscapes++;
						}
						else if(Source[2+i+1]==0x02)
						{
							Target[t]=0xA5;
							i++;
							t++;
							NumberEscapes++;
						}
						else if(Source[2+i+1]==0x03)
						{
							Target[t]=0x80;
							i++;
							t++;
							NumberEscapes++;
						}
				}
				else if(Source[2+i]!=0x80)
				{
						Target[t]=Source[2+i];
						t++;
				}	
	}
	return 0;
}
/************���ݰ�����***************/

uint8_t Pack_Classification(void)
{
	
	if(Gps_PackDtata[1]==0x01)                        //���ز�������/����/����/����  
	{
		return 1;
	}
	else if(Gps_PackDtata[1]==0x02)							  		//Flash��д			
	{
		return 2;
	}
	else if(Gps_PackDtata[1]==0x03)										//������GPRS͸��
	{
		return 3;
	}
	else																							//���ʹ��󶪰�	
	{
			GPclearData();
	}		
	return 0;
} 

/*************ȡһ������**************/
uint8_t GetGpsPack(ElemType* GpsPdata)
{

	if(GPHeadbit==0&&*GpsPdata==0x5A)
	{
		Gps_PackDtata[Gps_RevCount]=*GpsPdata;
		Gps_RevCount++;
		GPHeadbit=1;
	}	
	else if(GPHeadbit==1&&*GpsPdata!=0xA5)
	{
		Gps_PackDtata[Gps_RevCount]=*GpsPdata;
		Gps_RevCount++;
	}
	else if(GPHeadbit==1&&*GpsPdata==0xA5)
	{
			Gps_PackDtata[Gps_RevCount]=*GpsPdata;
			Gps_RevCount++;
			GPHeadbit=2;
			return 1;
	}
	return 0;
}
/**********������ȡ�����࣬���룩**************/
uint8_t GpRsacketProcessing(void)
{
	uint8_t FW=0;
	ElemType  gprsdata;
	if(GetPackbit==0)
	{
			if(DeQueue(&Uart_revQueuebuff,&gprsdata))
			{
					GPRevoutTime=0;	
					if( GetGpsPack(&gprsdata))
					{
						GetPackbit=1;
						return 0;
					}
			}
			else if(GPHeadbit==1&&GPRevoutTime>=300)
			{
					GPclearData();
			}
	}
	else if(GetPackbit==1)
	{
			FW=Pack_Classification();
			if(FW!=0)
			{
					GPdataTranslate(Gps_PackDtata,Gps_TranslatePackDtata);
					return FW;	
			}
			else
			{
				GetPackbit=0;
			}
	}
	return 0;
}	

/*************Flashд����*********************/
uint8_t GpFlashW(void)
{
	uint16_t GpCrc_val=0;
	uint16_t GetCRC16Len=0;
//	uint8_t i;
	GetCRC16Len|=(Gps_TranslatePackDtata[2]<<8);
	GetCRC16Len|=Gps_TranslatePackDtata[3];
	GpCrc_val=CRC16(&Gps_TranslatePackDtata[4],GetCRC16Len,GpPOLY16,0); 
	if(GpCrc_val!=0x0000&&Gps_TranslatePackDtata[GetCRC16Len+5]==(GpCrc_val&0xff)&&Gps_TranslatePackDtata[GetCRC16Len+4]==(GpCrc_val>>8))
	{
			Flash_SaveErase(&Gps_TranslatePackDtata[2],GetCRC16Len+4,flashaddr);
			return 1;
	}
	return 0;	
}
/*************Flash������*********************/
void GpFlash_r(void)
{
	uint16_t i;
	uint16_t GpFlashLen=0;
	GpFlashLen|=	((uint16_t)Flash_readBy(flashaddr))<<8;
	GpFlashLen|=	(uint16_t)Flash_readBy(flashaddr+1);
	for(i=0;i<GpFlashLen+4;i++)
	{
			Rflashdtata[i]=Flash_readBy(flashaddr+i);
	}
	Rflashdtata[GpFlashLen+4]='\0';
}


void GprsCmd_Task(void)
{
		static uint8_t Cmd_exe=0;
		Cmd_exe=GpRsacketProcessing();
		if(Cmd_exe==1)
		{
				if(Gps_TranslatePackDtata[0]==0xFE)
				{
					Gps_TranslatePackDtata[0]=Gps_TranslatePackDtata[1];
				}
				 GpCmd_char(&Gps_TranslatePackDtata[0]);
				 Cmd_exe=0;
				 memset(Gps_TranslatePackDtata,0,sizeof(Gps_TranslatePackDtata));
				 GPclearData();
		}
		else if(Cmd_exe==2)
		{
				if(Gps_TranslatePackDtata[0]==0xFE&&Gps_TranslatePackDtata[1]==0x01)
				{
							if(GpFlashW())
							{
									GpW_Flash_s=1;
							}
							Cmd_exe=0;
							memset(Gps_TranslatePackDtata,0,sizeof(Gps_TranslatePackDtata));
							GPclearData();
				}
				else if(Gps_TranslatePackDtata[0]==0xFE&&Gps_TranslatePackDtata[1]==0x03)
				{
						memset(Rflashdtata,0,sizeof(Rflashdtata));
						GpR_Flash_s=1;
						GpFlash_r();
						Cmd_exe=0;
						memset(Gps_TranslatePackDtata,0,sizeof(Gps_TranslatePackDtata));
						GPclearData();
				}
		}
		else if(Cmd_exe==3)
		{
						Penetratebit=1;
						Cmd_exe=0;
		}
}
/*��ʾ��û���յ����صĻط�����,��ÿ��200ms�ط�һ��5A 01 FE 51 A5 
�ط�5�κ�,��Ȼû���յ����ػط�,���˻ؾ�Э��Ͳ�����*/
void ToggleConfirmationHandshake(void)
{
		static uint8_t CoutShake=0;
		if(Toggle==1)
		{

				GprsCmd_Task();
				GpsSend_Data_Uart();
				if(GpTogshakeTime>=100)
				{
					ToggleAN=1;
					GpTogshakeTime=0;
					CoutShake++;
				}
				if(CoutShake==150)
				{
					Uart0_init(DEFAULTBOUD);
					Toggle=0;
					GpTogshakeTime=0;
					CoutShake=0;
				}
			 
		}

	}









