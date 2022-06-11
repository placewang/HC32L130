
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
				///< ����FLASH����ʱ��
				Sysctrl_SetPeripheralGate(SysctrlPeripheralFlash, TRUE); 
				Flash_WaitCycle(FlashWaitCycle2);                           // ��Ҫ��Ƶ��PLL��Ϊϵͳʱ��HCLK��ﵽ48MHz�����Դ˴�Ԥ������FLASH ���ȴ�����Ϊ1 cycle(Ĭ��ֵΪ0 cycle)
				
		//    Sysctrl_SetRCHTrim(SysctrlRchFreq4MHz);                   // ʱ�ӳ�ʼ��ǰ����������Ҫʹ�õ�ʱ��Դ          

				Sysctrl_SetXTHFreq(SysctrlXthFreq6_12MHz);                 //�л�ʱ��ǰ�������ⲿ���پ�������XTHƵ�ʷ�Χ,���þ��������ʹ��Ŀ��ʱ�ӣ��˴�Ϊ8MHz
				Sysctrl_XTHDriverCfg(SysctrlXtalDriver3);
				Sysctrl_SetXTHStableTime(SysctrlXthStableCycle16384);
				Sysctrl_ClkSourceEnable(SysctrlClkXTH, TRUE);
				
		//    Sysctrl_SysClkSwitch(SysctrlClkXTH);										 //< ʱ���л�
		//    Sysctrl_ClkSourceEnable(SysctrlClkRCH, FALSE);
				
				stcPLLCfg.enInFreq    = SysctrlPllInFreq6_12MHz;         //XTH 8MHz
				stcPLLCfg.enOutFreq   = SysctrlPllOutFreq36_48MHz;       //PLL ���48MHz
				stcPLLCfg.enPllClkSrc = SysctrlPllXthXtal;               //����ʱ��Դѡ��XTH
				stcPLLCfg.enPllMul    = SysctrlPllMul6;                  //8MHz x 6 = 48MHz
				Sysctrl_SetPLLFreq(&stcPLLCfg);       
				Sysctrl_SetPLLStableTime(SysctrlPllStableCycle16384);
				Sysctrl_ClkSourceEnable(SysctrlClkPLL, TRUE);
				
				stcCfg.enClkSrc  = SysctrlClkPLL;                        //< ѡ��PLL��ΪHCLKʱ��Դ
				stcCfg.enHClkDiv = SysctrlHclkDiv1;                      //< HCLK SYSCLK/1
				stcCfg.enPClkDiv = SysctrlPclkDiv1;                      //< PCLK ΪHCLK/1
				Sysctrl_ClkInit(&stcCfg);                                //< ϵͳʱ�ӳ�ʼ��
				
				Sysctrl_SysClkSwitch(SysctrlClkPLL);							       //< ʱ���л�	
	}
	delay1ms(20);
}

