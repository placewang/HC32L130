#include "bsp_touch.h"
#include "bsp_gpio.h"
/*ģ��SPI*/

uint8_t TouchRbit=0;                             //���ض����ݱ�־λ
uint8_t LPSendInterruptbit=0;                    //���ش��� ���GPSRS�����صĵ�ǰ͸����־λ  

uint8_t TouchSbit=0;                             //����XY���ݶ�ȡ��ɱ�־λ
uint8_t TouchUbit=0;														 //����̧���־λ		
uint8_t TouchCountD=0;													 //���ذ����˲�
uint8_t TouchCountU=0;                           //����̧���˲�

uint16_t X_Touch_val,Y_Touch_val;								 //��ĻX/Y����洢

void TSC2046_Init ( void )
{
	  stc_gpio_cfg_t stcGpioCfg;
		Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);                             // ��GPIO����ʱ���ſ�  
    
		stcGpioCfg.enDir = GpioDirOut;   
		stcGpioCfg.enOD = GpioOdDisable;                                                    //< �˿ڿ�©�������->��©����ر�
		stcGpioCfg.enDrv = GpioDrvH;                                                        //< �˿�������������->����������  
    stcGpioCfg.enPu = GpioPuEnable;                                                     //< �˿�����������->����
    stcGpioCfg.enPd = GpioPdDisable;
	  stcGpioCfg.enCtrlMode = GpioAHB;                                                    //< �˿�����/���ֵ�Ĵ������߿���ģʽ����->AHB
    Gpio_Init(GPIO_SPI_Touch_cs_PORT ,GPIO_SPI_Touch_cs_PIN ,&stcGpioCfg);
		Touch_cs_ON(); 

	  stcGpioCfg.enPu = GpioPuDisable;                                                    //< �˿�����������->����
    stcGpioCfg.enPd = GpioPdEnable;
		Gpio_Init(GPIO_SPI_Touch_mosi_PORT,GPIO_SPI_Touch_mosi_PIN,&stcGpioCfg);
		Gpio_Init(GPIO_SPI_Touch_clk_PORT,GPIO_SPI_Touch_clk_PIN,&stcGpioCfg);  
		Touch_mosi_0();
		Touch_clk_Low(); 
	
		stcGpioCfg.enDir = GpioDirIn;                                                                      
		stcGpioCfg.enPu = GpioPuEnable;                                                     // �˿�����������->����
    stcGpioCfg.enPd = GpioPdDisable;
		Gpio_Init(GPIO_Touch_BUSY_PORT,GPIO_Touch_BUSY_PIN,&stcGpioCfg);                     //TSC2046 BUSY�˿ڳ�ʼ��
		Gpio_Init(GPIO_SPI_Touch_miso_PORT,GPIO_SPI_Touch_miso_PIN,&stcGpioCfg);             //TSC2046 miso�˿ڳ�ʼ�� 
		
//		stcGpioCfg.enPu = GpioPuDisable;                                                  // �˿�������
//    stcGpioCfg.enPd = GpioPdEnable;
		Gpio_Init(GPIO_Touch_OPENRQ_PORT,GPIO_Touch_OPENRQ_PIN,&stcGpioCfg);                //TSC2046 OPENRQ�˿ڳ�ʼ��
			
//	  Gpio_EnableIrq(GPIO_Touch_BUSY_PORT,GPIO_Touch_BUSY_PIN,GpioIrqFalling);         //< �򿪲�����OPENRQΪ�½����ж� 
//	  EnableNvic(PORTA_IRQn, IrqLevel3, TRUE);                                             //< ʹ�ܶ˿�PORTAϵͳ�ж�	
}


void Spi_init(void)
{
	  stc_gpio_cfg_t GpioInitStruct;
	
		stc_spi_cfg_t  SpiInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);
		
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
		Sysctrl_SetPeripheralGate(SysctrlPeripheralSpi0,TRUE);
	
    //SPI0��������:����
    GpioInitStruct.enDrv = GpioDrvH;
    GpioInitStruct.enDir = GpioDirOut;

    Gpio_Init(GpioPortA, GpioPin7,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin7,GpioAf1);        //��������PA7��ΪSPI0_MOSI

    Gpio_Init(GpioPortA, GpioPin4,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin4,GpioAf1);         //��������PA04��ΪSPI0_CS

    Gpio_Init(GpioPortA, GpioPin5,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin5,GpioAf1);         //��������PA05��ΪSPI0_SCK

    //SPI0ģ�����ã�����
    SpiInitStruct.enSpiMode = SpiMskMaster;   //����λ����ģʽ
    SpiInitStruct.enPclkDiv = SpiClkMskDiv128;  //�����ʣ�fsys/128
    SpiInitStruct.enCPHA    = SpiMskCphafirst;//��һ���ز���
    SpiInitStruct.enCPOL    = SpiMskcpollow;  //����Ϊ��
    Spi_Init(M0P_SPI0, &SpiInitStruct);
//    GpioInitStruct.enDir = GpioDirIn;
//    Gpio_Init(GpioPortA, GpioPin6,&GpioInitStruct);
//    Gpio_SetAfMode(GpioPortA, GpioPin6,GpioAf1);         

}




/**
  * @brief  TSC2046 ��д������
  * @param  ucCmd ������
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 0x90 :ͨ��Y+��ѡ�������
  *     @arg 0xd0 :ͨ��X+��ѡ�������
  * @retval ��
  */
static void TSC2046_WriteCMD ( uint8_t ucCmd ) 
{
	uint8_t i;
	Touch_clk_Low();
	Touch_mosi_0();
	delay10us(1);
	for ( i = 0; i < 8; i ++ ) 
	{
			(( ucCmd >> ( 7 - i ) ) & 0x01 ) ? Touch_mosi_1(): Touch_mosi_0();
			Touch_clk_High();
			delay10us(1);
			Touch_clk_Low();
			delay10us(1);
	}
}

/**
  * @brief  TSC2046 �Ķ�ȡ����
  * @param  ��
  * @retval ��ȡ��������
  */
static uint16_t TSC2046_ReadCMD( void ) 
{
	uint8_t i;
	uint16_t usBuf=0, usTemp=0;
	Touch_mosi_0();
	while(!Touch_BUSY_IN())
	{
		;
	}
		Touch_clk_Low();
		delay10us(1);
		Touch_clk_High();
		delay10us(1);
		Touch_clk_Low();
	while(Touch_BUSY_IN())
	{
		;
	}
	for ( i=0;i<12;i++ )
	{
	//		Touch_clk_Low();
		
		#if 0
		Touch_clk_Low();
		usTemp=Touch_miso_IN();
		delay10us(1);
		Touch_clk_High();
		delay10us(1);
		#else
		Touch_clk_Low();
		delay10us(1);
//		Touch_clk_High();
		
//		delay10us(1);
		Touch_clk_High();
		usTemp=Touch_miso_IN();
		delay10us(1);
		#endif
		usBuf|=usTemp << ( 11 - i );
	}
	return usBuf;
}


/**
  * @brief  �� TSC2046 ѡ��һ��ģ��ͨ��������ADC��������ADC�������
  * @param  ucChannel
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 0x90 :ͨ��Y+��ѡ�������
  *     @arg 0xd0 :ͨ��X+��ѡ�������
  * @retval ��ͨ����ADC�������
  */
static uint16_t TSC2046_ReadAdc ( uint8_t ucChannel )
{
		uint16_t rev_data;	
	  Touch_cs_OFF();
		delay10us(1);
		TSC2046_WriteCMD(ucChannel);
		rev_data=TSC2046_ReadCMD();
		delay10us(1);
		Touch_cs_ON(); 
    return rev_data;
}



/**
  * @brief  ��ȡ TSC2046 ��Xͨ����Yͨ����ADֵ��12 bit�������4096��
  * @param  sX_Ad �����Xͨ��ADֵ�ĵ�ַ
  * @param  sY_Ad �����Yͨ��ADֵ�ĵ�ַ
  * @retval ��
  */
void TSC2046_ReadAdc_XY ( uint16_t* sX_Ad, uint16_t * sY_Ad )  
{ 
	int16_t sX_Ad_Temp, sY_Ad_Temp; 
	
	sX_Ad_Temp = TSC2046_ReadAdc(TSC2046_CHANNEL_X);
	sY_Ad_Temp = TSC2046_ReadAdc(TSC2046_CHANNEL_Y); 

	* sX_Ad = sX_Ad_Temp; 
	* sY_Ad = sY_Ad_Temp; 
}
//&&TouchCountD>=10

void Rev_Touch_XY(void)
{
		if(!Touch_PENIRQ_Read()&&!TouchRbit)
		{
			TouchRbit=1;
			TSC2046_ReadAdc_XY(&X_Touch_val,&Y_Touch_val);
			TouchSbit=1;
			Backlight_ON();
			LPSendInterruptbit=1;
		}
		else if(Touch_PENIRQ_Read()&&TouchRbit)
		{
			TouchRbit=0;
			TouchUbit=1;
		}
}


///< PortA�жϷ�����
void PortA_IRQHandler(void)
{
    if(TRUE == Gpio_GetIrqStatus(GPIO_Touch_BUSY_PORT,GPIO_Touch_BUSY_PIN))
    {            
        Gpio_ClearIrq(GPIO_Touch_BUSY_PORT  ,GPIO_Touch_BUSY_PIN);    
    }
}  





