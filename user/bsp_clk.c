
#include "bsp_clk.h"

void Systimeclk(void)
{
	
    stc_sysctrl_clk_cfg_t stcCfg;
    stc_sysctrl_pll_cfg_t stcPLLCfg;    
    if(Sysctrl_GetHClkFreq()==48000000u&&Sysctrl_GetPClkFreq()==48000000u)
		{
			
		}
		else
		{	
				///< 开启FLASH外设时钟
				Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash, TRUE); 
				Flash_WaitCycle(FlashWaitCycle2);                           // 因将要倍频的PLL作为系统时钟HCLK会达到48MHz：所以此处预先设置FLASH 读等待周期为1 cycle(默认值为0 cycle)
				
		//    Sysctrl_SetRCHTrim(SysctrlRchFreq4MHz);                   // 时钟初始化前，优先设置要使用的时钟源          

				Sysctrl_SetXTHFreq(SysctrlXthFreq6_12MHz);                 //切换时钟前（根据外部高速晶振）设置XTH频率范围,配置晶振参数，使能目标时钟，此处为8MHz
				Sysctrl_XTHDriverCfg(SysctrlXtalDriver3);
				Sysctrl_SetXTHStableTime(SysctrlXthStableCycle16384);
				Sysctrl_ClkSourceEnable(SysctrlClkXTH, TRUE);
				
		//    Sysctrl_SysClkSwitch(SysctrlClkXTH);										 //< 时钟切换
		//    Sysctrl_ClkSourceEnable(SysctrlClkRCH, FALSE);
				
				stcPLLCfg.enInFreq    = SysctrlPllInFreq6_12MHz;         //XTH 8MHz
				stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;       //PLL 输出48MHz
				stcPLLCfg.enPllClkSrc = SysctrlPllXthXtal;               //输入时钟源选择XTH
				stcPLLCfg.enPllMul    = SysctrlPllMul6;                  //8MHz x 6 = 48MHz
				Sysctrl_SetPLLFreq(&stcPLLCfg);       
				Sysctrl_SetPLLStableTime(SysctrlPllStableCycle16384);
				Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
				
				stcCfg.enClkSrc  = SysctrlClkPLL;                        //< 选择PLL作为HCLK时钟源
				stcCfg.enHClkDiv = SysctrlHclkDiv1;                      //< HCLK SYSCLK/1
				stcCfg.enPClkDiv = SysctrlPclkDiv1;                      //< PCLK 为HCLK/1
				Sysctrl_ClkInit(&stcCfg);                                //< 系统时钟初始化
				
				Sysctrl_SysClkSwitch(SysctrlClkPLL);							       //< 时钟切换	
	}
	delay1ms(20);
}

