#include "bsp_gpio.h"


/********ϵͳָʾ��*******/
void Led_init(void)
{
	  stc_gpio_cfg_t stcGpioCfg;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 	///< ��GPIO����ʱ���ſ�
    
    stcGpioCfg.enDir = GpioDirOut;														///< �˿ڷ�������->���(�������������ϣ����룩���ò���һ��)
    
		stcGpioCfg.enOD = GpioOdDisable;                					//< �˿ڿ�©�������->��©����ر�
    stcGpioCfg.enPu = GpioPuEnable;                 					//< �˿�����������->����
    stcGpioCfg.enPd = GpioPdDisable;
    
    Gpio_Init(LED0_PORT, LED0_PIN, &stcGpioCfg);	 						///< GPIO IO LED�˿ڳ�ʼ��
		Gpio_SetIO(LED0_PORT, LED0_PIN);
}

/*******������**********/
void Buzz_init(void)
{
		stc_gpio_cfg_t stcGpioCfg;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);   ///< ��GPIO����ʱ���ſ�
    
    stcGpioCfg.enDir = GpioDirOut;														///< �˿ڷ�������->���(�������������ϣ����룩���ò���һ��)	
    
    stcGpioCfg.enPu = GpioPuEnable;														///< �˿�����������->����
    stcGpioCfg.enPd = GpioPdDisable;
    
    Gpio_Init(BUZZ_PORT, BUZZ_PIN, &stcGpioCfg);							///< GPIO IO LED�˿ڳ�ʼ��
		
		Gpio_ClrIO(BUZZ_PORT, BUZZ_PIN);													//Ĭ�Ϸ�����
}
/*******�������⿪��***********/
void Backlight_init(void)
{
		stc_gpio_cfg_t stcGpioCfg;
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); ///< ��GPIO����ʱ���ſ�
    
    stcGpioCfg.enDir = GpioDirOut;													///< �˿ڷ�������->���(�������������ϣ����룩���ò���һ��)
    
    stcGpioCfg.enPu = GpioPuEnable;													///< �˿�����������->����
    stcGpioCfg.enPd = GpioPdDisable;
    
    Gpio_Init(Backlight_PORT,Backlight_PIN, &stcGpioCfg);   ///< GPIO IO LED�˿ڳ�ʼ��
		Backlight_OFF();
}
/************����**************/

void GPIO_Key_init(void)
{
	  stc_gpio_cfg_t stcGpioCfg;
		Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);                             // ��GPIO����ʱ���ſ�  
    
		stcGpioCfg.enDir = GpioDirIn;  																											//����ģʽ			
		stcGpioCfg.enOD = GpioOdDisable;                                                    //< �˿ڿ�©�������->��©����ر�
		stcGpioCfg.enDrv = GpioDrvH;                                                        //< �˿�������������->����������  
    stcGpioCfg.enPu = GpioPuEnable;                                                     //< �˿�����������->����
    stcGpioCfg.enPd = GpioPdDisable;			
	  Gpio_Init(Key_Col0_PORT ,Key_Col0_PIN,&stcGpioCfg);
	  Gpio_Init(Key_Col1_PORT ,Key_Col1_PIN,&stcGpioCfg);
	  Gpio_Init(Key_Col2_PORT ,Key_Col2_PIN,&stcGpioCfg);
	  Gpio_Init(Key_Col3_PORT ,Key_Col3_PIN,&stcGpioCfg);
	  Gpio_Init(Key_Col4_PORT ,Key_Col4_PIN,&stcGpioCfg);
	  Gpio_Init(Key_Col5_PORT ,Key_Col5_PIN,&stcGpioCfg);
	  Gpio_Init(Key_Col6_PORT ,Key_Col6_PIN,&stcGpioCfg);
	  Gpio_Init(Key_Col7_PORT ,Key_Col7_PIN,&stcGpioCfg);
		
		stcGpioCfg.enDir = GpioDirOut;
		Gpio_Init(Key_Row0_PORT ,Key_Row0_PIN,&stcGpioCfg);
		Gpio_Init(Key_Row1_PORT ,Key_Row1_PIN,&stcGpioCfg);
		Gpio_Init(Key_Row2_PORT ,Key_Row2_PIN,&stcGpioCfg);
		Gpio_Init(Key_Row3_PORT ,Key_Row3_PIN,&stcGpioCfg);
		Gpio_Init(Key_Row4_PORT ,Key_Row4_PIN,&stcGpioCfg);
		Gpio_Init(Key_Row5_PORT ,Key_Row5_PIN,&stcGpioCfg);
		Gpio_Init(Key_Row6_PORT ,Key_Row6_PIN,&stcGpioCfg);
	
}	
/************Һ��ʶ��***********/

void Screen_init(void)
{
		stc_gpio_cfg_t stcGpioCfg;
		
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE); 	///< ��GPIO����ʱ���ſ�
    
    stcGpioCfg.enDir = GpioDirOut;														///< �˿ڷ�������->���(�������������ϣ����룩���ò���һ��)
		
		stcGpioCfg.enOD = GpioOdDisable;                					//< �˿ڿ�©�������->��©����ر�
    stcGpioCfg.enPu = GpioPuEnable;                 					//< �˿�����������->����
    stcGpioCfg.enPd = GpioPdDisable;
	  
		Gpio_Init(Screen0_PORT ,Screen0_PIN,&stcGpioCfg);
	  Gpio_Init(Screen1_PORT ,Screen1_PIN,&stcGpioCfg);
		
		Gpio_SetIO(Screen0_PORT ,Screen0_PIN);
		Gpio_SetIO(Screen1_PORT ,Screen1_PIN);
}
/*****��ȡҺ�����ߴ�******/
uint8_t GetLcdType(void)
{
		if(Screen0_IN	&&Screen1_IN)
		{
			return 1;
		}
		else if(!Screen0_IN&&Screen1_IN)
		{
			return 0;
		}
		else if(Screen0_IN&&!Screen1_IN	)
		{
		
			return 3;
		}
		return 0;
}



