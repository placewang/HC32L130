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

uint8_t  g_iLcdType;                                            //LCD屏种类
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
  Sysctrl_SetPeripheralGate(SysctrlPeripheralWdt,TRUE);       //开启WDT外设时钟
  Wdt_Init(WdtResetEn,WdtT1s64);															//WDT 初始化	
	Wdt_Start();
}

int main()
{ 
	Systimeclk();
	Bsp_Init();	                                                 //外设初始化																							 
	while(Ok != Flash_Init(12, TRUE)){;}                         //确保初始化正确执行后方能进行FLASH编程操作，FLASH初始化（编程时间,休眠模式配置）
	g_iLcdType=GetLcdType();																		 //获取触屏版本
	SendInformation();		                                       //上电发送版本信息
	while(1)
	{
		Wdt_Feed();                                                // 喂狗
		Systme_led();
		if(Toggle==0)
		{
			Rev_Touch_XY();
			Cmd_Task();
			KeyScanTask();
			Send_Data_Uart();
		}
		else if(Toggle==2)//GPRS
		{
			 Rev_Touch_XY();	
			 KeyScanTask();
			 GprsCmd_Task();
			 GpsSend_Data_Uart();
			 ScanInteractiveState();
			 GPRSRev_data(); 
		}
		else if(Toggle==3||Toggle==4)//logica
		{
			if(Toggle==3){Backlight_ON();}
			LogicaRev_Touch_XY();	
			Cmd_Task();
			KeyScanTask();
			Send_Data_Uart();
		}
		ToggleConfirmationHandshake();
	}
	return 0;
}

