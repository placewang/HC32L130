
#include "app_Cmd.h"
#include "queue.h"
#include "bsp_gpio.h"
#include "bsp_flash.h"	
#include "bsp_uart.h"
#include "crc16_flash.h"
#include "bsp_touch.h"
#include "wdt.h"
#define POLY16 0x1021

unsigned short CRC16(unsigned char *buf,unsigned long dlen, int poly, unsigned short CRCinit)
{
        unsigned char ch;
        unsigned short CRC = CRCinit;
        int i;
        while (dlen--)
        {
                ch = *buf++;
                CRC ^= (((unsigned short) ch) << 8);
                for (i = 0; i < 8; i++)
                {
                        if (CRC & 0x8000)
                                CRC = (CRC << 1) ^ poly;
                        else
                                CRC <<= 1;
                }
        }
        return CRC;
}


uint32_t flashaddr=0xfc00;                                      //��ĻУ׼ֵ��дflash������ʼ��ַ
uint32_t Baudddr=0xfc64;                                        //�����ʶ�дflash��ʼ��ַ
uint16_t FlashLen=0;                                            //����flash���ݳ���
uint8_t	 F_Crc16_L=0;																						//����flash����crcУ��ߵ�λ	
uint8_t  F_Crc16_H=0; 
uint8_t  flashdtata[100]={0};                                   //д��flash�����ݴ�

uint8_t  Rflashdtata[100]={0};                                  //��flash�������������ݴ�
uint8_t W_Flash_s=0;                                            //��дFlash �ɹ���־λ 																						
uint8_t R_Flash_s=0;
uint8_t Key_Inquire=0;                                          //���̲�ѯ��־λ
uint8_t Version_Inquire=0;                                      //�汾��ѯ��־λ
uint8_t Toggle=0;                                               //������GRS����Э���л���־λ
uint8_t ToggleAN=0;                                             //�汾ת��Ӧ�� 

uint16_t DataoutTime=0;                                          //������ͨѶ��ʱ  
volatile static uint8_t FE_bit=0;                                //Э��ͷɸѡ��־λ
volatile static uint8_t rev_flashcount=0;                        //���ռ���
volatile static uint16_t flashwritelen=0;												 //����	


/**************��ȡCMD����******************************************/
static int8_t GetBuffCmd(uint8_t * cmd)
{
	
	if(*cmd==0xfe&&FE_bit==0)
	{
			FE_bit=1;
			return 4;
	}
	else if(FE_bit==1)
	{
				if(*cmd==0x01)
				{
					return 1;
				}
				else if(*cmd==0x03)
				{
					return 2;
				}
				else if(*cmd==0x51)
				{
					return 3;
				}
				else
				{
					return 0;
				}
	}
	return 0;
}	


/****************��������ʾ��CMDִ��********************************/
void Cmd_char(uint8_t * CmdData)	
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
					Key_Inquire=1;
					TouchRbit=0;
					Rev_Touch_XY();
					break;
				case 0x77:
					Version_Inquire=1;
					break;
    }
}



/***************Cmd_Flash_get pack*********************************/
static uint8_t Cmd_Flash(uint8_t * CmdData)
{
		if(rev_flashcount<=0x01)                                            //��ȡ���ݳ���
		{

					flashdtata[rev_flashcount]=*CmdData;
					if(rev_flashcount==0x00)
					{
							
						flashwritelen|=((uint16_t)*CmdData)<<8;
					}
					else if(rev_flashcount==0x01)
					{
						 flashwritelen|=(uint16_t)*CmdData;
					}
					rev_flashcount++;
		}
		else
		{		
				flashdtata[rev_flashcount]=*CmdData;
				rev_flashcount++;	
				if(flashwritelen+4==rev_flashcount)
				{
						F_Crc16_H=flashdtata[rev_flashcount-2];	                         
						F_Crc16_L=flashdtata[rev_flashcount-1];													  //ȡCRC16ֵ
						flashdtata[rev_flashcount-2]='\0';                                //д�������
						rev_flashcount=0;
						return 1;	
				}

	 }	
	 return 0;
}
/*****************Flash����ǰ����ԭ��������д��****************************/
void Flash_SaveErase(uint8_t* Fval,uint16_t Flen,uint16_t FAddr)
{
	uint8_t FlASH_VAL[512]={0};
	uint16_t i=0,t=0;
	for(i=0;i<sizeof(FlASH_VAL);i++)
	{
				FlASH_VAL[i]=Flash_readBy(flashaddr+i);	
	}
	while(Ok != Flash_SectorErase(flashaddr)){;}
	
	for(i=FAddr-flashaddr;i<(Flen+FAddr-flashaddr);i++)
	{
		 FlASH_VAL[i]=Fval[t];
		 t++;
	}
	for(i=0;i<sizeof(FlASH_VAL);i++)
	{
		Flash_writeBy(flashaddr+i,FlASH_VAL[i]);
	}

}

/****************�������ݵ�Flash***********************************/
static int Flash_w(uint32_t Addr)
{
	uint16_t Crc_val;
//	uint8_t i;
	Crc_val=CRC16(&flashdtata[2],flashwritelen,POLY16,0);                                 //�˳���������  
	if(F_Crc16_L==(Crc_val&0xff)&&F_Crc16_H==(Crc_val>>8))
	{
				flashdtata[flashwritelen+2]=F_Crc16_H;
				flashdtata[flashwritelen+3]=F_Crc16_L;
				flashwritelen+=4;
				Flash_SaveErase(flashdtata,flashwritelen,Addr);
				return 1;
	}
	else                                                                                //CRCУ���������ݴ�
	{
		memset(flashdtata,0,sizeof(flashdtata));
		return 0;
	}
}	

/*******************��Flash***************************************/
static void Flash_r(void)
{
	uint16_t i;
	FlashLen=0;
	FlashLen|=	((uint16_t)Flash_readBy(flashaddr))<<8;
	FlashLen|=	(uint16_t)Flash_readBy(flashaddr+1);
	if(FlashLen!=0xffff)
	{
		for(i=0;i<FlashLen+4;i++)
		{
			 Rflashdtata[i]=Flash_readBy(flashaddr+i);		
		}
		Rflashdtata[FlashLen+4]='\0';
	}
}

uint8_t CmdTask1(void)
{
	static uint8_t Cmd_exe=0;
	static uint8_t FlashBitStu=0;
	static uint8_t FW_val=0;	
	uint32_t Boud_val=0;	
	if(FlashBitStu!=0&&DataoutTime>=3000)        //Flash������ʱ����
	{
		FlashBitStu=0;
		DataoutTime=0;
		FW_val=0;
		FE_bit=0;
		rev_flashcount=0;                          //���ռ���
		flashwritelen=0;													 //����	
		memset(flashdtata,0,sizeof(flashdtata));
	}
	
	if(DeQueue(&Uart_revQueuebuff,&Cmd_exe))    //���ݳ���
	{
			if((FW_val==0|| FW_val==4)&&FlashBitStu==0)
			{
					FW_val= GetBuffCmd(&Cmd_exe);
					if(FW_val==0)
					{
						Cmd_char(&Cmd_exe);
					}
					else if(FW_val==2)
					{
						FlashBitStu=1;
					}
					else if(FW_val==1)
					{
							FlashBitStu=1;
							DataoutTime=0;
							return 0;
					}
					else if(FW_val==3)
					{
							FlashBitStu=1;
							DataoutTime=0;
							return 0;
					}		
			}
			
			if(FlashBitStu==1)
			{
					if(FW_val==1)
					{
							if(Cmd_Flash(&Cmd_exe)&&Flash_w(flashaddr))                                   //дFlash����У׼ֵ 
							{									
									W_Flash_s=1;
									FlashBitStu=0;
									FW_val=0;
									FE_bit=0;
									flashwritelen=0;					
							}
					}
					else if(FW_val==2)                                                               //��Flash ����У׼ֵ 
					{
								Flash_r();
								R_Flash_s=1;
								FlashBitStu=0;
								FE_bit=0;
								FW_val=0;
					}		
					else if(FW_val==3)                                                                //�������޸�
					{
							if(Cmd_Flash(&Cmd_exe)&&Flash_w(Baudddr))
							{	
									Boud_val=0;
									Boud_val|=(uint32_t)flashdtata[2]<<24;
									Boud_val|=(uint32_t)flashdtata[3]<<16;
									Boud_val|=(uint32_t)flashdtata[4]<<8;
									Boud_val|=(uint32_t)flashdtata[5];
									if(Boud_val>0)
									{
										Uart0_init(Boud_val);
										Toggle=1;
										ToggleAN=1;                                                               //�汾ת��Ӧ�� 
										FlashBitStu=0;
										FW_val=0;
										FE_bit=0;
										flashwritelen=0;
									}
									else
									{
										FlashBitStu=0;
										FW_val=0;
										FE_bit=0;
										flashwritelen=0;
								  }		
					    }
					}
			}
		}	
	return 0;
}
/***********************************************************
logica�汾����
************************************************************/
signed char LogicaVersionSwitch(void)
{
	static    char  VerSwitch=0;
	unsigned  short VerSwitchOutTime=0;
	unsigned  char  dat[3]={0};
	while(VerSwitch!=10)
	{
		Wdt_Feed();
		switch(VerSwitch)
		{
			  case 0:
						if(DataoutTime>=50*1*2)
						{
							Uart_SendDataPoll(M0P_UART0,'x');
							Uart_SendDataPoll(M0P_UART0,0xFE);
							Uart_SendDataPoll(M0P_UART0,0x04);
							DataoutTime=0;
							VerSwitchOutTime++;
						}
			 		  if(VerSwitchOutTime>=5*10)                                    //��ʱ�˳�Ĭ���Ǻ��
					 {
						 VerSwitch=10;
						 return 0;
					 }
					 while(QueueLength(&Uart_revQueuebuff)>=2)
					 {
							DeQueue(&Uart_revQueuebuff,&dat[0]);
							if(dat[0]!=0xFE){continue; }
						  DeQueue(&Uart_revQueuebuff,&dat[1]);
							if(dat[0]==0xFE&&dat[1]==0x04)
							{
									VerSwitch=1;
									VerSwitchOutTime=0;
									Backlight_ON();
									 break ;
							}	
					 }
					break;
			case 1:
							Uart_SendDataPoll(M0P_UART0,'x');
							Uart_SendDataPoll(M0P_UART0,0xFE);
							Uart_SendDataPoll(M0P_UART0,0x0A);
							Uart0_init(115200);
							delay1ms(100);
							VerSwitch=2;
							
							break;
			case 2:
						if(DataoutTime>=50*1)
						{
							Uart_SendDataPoll(M0P_UART0,'x');
							Uart_SendDataPoll(M0P_UART0,0xFE);
							Uart_SendDataPoll(M0P_UART0,0x05);
							DataoutTime=0;
							VerSwitchOutTime++;
						}					
			 		  if(VerSwitchOutTime>=50*10)                                    //��ʱ �����ʻص�12500
					 {
						 VerSwitch=10;
						 Uart0_init(DEFAULTBOUD);
						 return 0;
					 }						
					while(QueueLength(&Uart_revQueuebuff)>=2)
					{
						DeQueue(&Uart_revQueuebuff,&dat[0]);
						if(dat[0]!=0xFE){continue; }
						DeQueue(&Uart_revQueuebuff,&dat[1]);
						if(dat[0]==0xFE&&dat[1]==0x05)
						{					
									VerSwitch=3;
						 			Uart_SendDataPoll(M0P_UART0,'x');
							    Uart_SendDataPoll(M0P_UART0,0xFE);
							    Uart_SendDataPoll(M0P_UART0,0x0B);	
									break ;
						}	
					}	
					break ;
			case 3:	                                                             //�������
						Flash_r();
					  R_Flash_s=1;
						VerSwitch=4;
						return 0;
			case 4:	
				if(R_Flash_s==0)
				{
					VerSwitch=10;
					Toggle=3;
					return 0;
				}
				break ;
		}
	}
	return 0;

}	

uint8_t Cmd_Task(void)
{
	LogicaVersionSwitch();
	CmdTask1();
	return 0;
}	









