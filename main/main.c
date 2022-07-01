#include "bsp_gpio.h"
#include "bsp_clk.h"
#include "bsp_time.h"
#include "bsp_uart.h"
#include "bsp_touch.h"
#include "bsp_flash.h"
#include "app_key.h"
#include "app_Version.h"
#include "app_Cmd.h"
#include "app_uart0.h"
#include "app.lpuart.h"
#include "app_gpsuart0.h"
#include "app_gprsCmd.h"
#include "wdt.h"

uint8_t  g_iLcdType;                                            //LCD������10.1/10.4 /7��

static void Bsp_Init(void)
{
	Uart0_init(DEFAULTBOUD);
	Led_init();
  Buzz_init();
	Backlight_init();
	Screen_init();
	GPIO_Key_init();
	KeyInit();
	Time_init(400);
  TSC2046_Init();
  Sysctrl_SetPeripheralGate(SysctrlPeripheralWdt,TRUE);       //����WDT����ʱ��
  Wdt_Init(WdtResetEn, WdtT1s64);															//WDT ��ʼ��	
	Wdt_Start();
}

int main()
{ 
	Systimeclk();
	Bsp_Init();																									 //�����ʼ��	
	while(Ok != Flash_Init(12, TRUE)){;}                         //ȷ����ʼ����ȷִ�к��ܽ���FLASH��̲�����FLASH��ʼ�������ʱ��,����ģʽ���ã�

	g_iLcdType=GetLcdType();																		 //��ȡ�����汾
	SendInformation();		                                       //�ϵ緢�Ͱ汾��Ϣ
	while(1)
	{
		Wdt_Feed();                                                // ι��
		Systme_led();
		Rev_Touch_XY();
		if(Toggle==0)
		{
			Cmd_Task();
			KeyScanTask();
			Send_Data_Uart();
		}
		else if(Toggle==2)
		{
			 KeyScanTask();
			 GprsCmd_Task();
			 GpsSend_Data_Uart();
			 ScanInteractiveState();
			 GPRSRev_data(); 
		}
		ToggleConfirmationHandshake();
	}
	return 0;
}

