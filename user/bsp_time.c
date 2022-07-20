#include "app_Cmd.h"
#include  "bsp_time.h"
#include  "bsp_gpio.h"
#include  "app_key.h"
#include  "app_gprsCmd.h"
extern uint16_t LPrevouttiem;     //接收超时检测
extern uint16_t DataoutTime;      //显示器与主控通讯超时 

//static volatile uint8_t u8TrimTestFlag   = 0;

volatile uint16_t Led_timecount=0;


/*************清按键滤波计数******************/
void Clerlkey_TimeCount(void)
{
	if(count_key>40)
	{
		count_key=0;
		count_key2=0;
	}
}
/*************触控滤波计数清零******************/
void ClerlTouch_TimeCount(void)
{
	if(TouchCountD>=60 || TouchCountU>=60)
	{
		TouchCountD=0;
		TouchCountU=0;
	}
	
}
/*************协议交互超时计数清零******************/
void ClerlCMD_TimeCount(void)
{
	if(DataoutTime>1000*9)
	{
			DataoutTime=0;
			LPrevouttiem=0;
	}
	if(LPrevouttiem>1000*1)
	{
			LPrevouttiem=0;
	}
	
}




/*******TIM0中断服务函数*****************/
void Tim0_IRQHandler(void)
{

    //Timer0 模式0 溢出中断
    if(TRUE == Bt_GetIntFlag(TIM0, BtUevIrq))
    {
         
				Led_timecount++;
				count_key++;	
        count_key2++;
				DataoutTime++;
				LPrevouttiem++;
				if(Toggle==1)
				{
					GpTogshakeTime++;
				}
				if(!Touch_PENIRQ_Read())
				{
					TouchCountD++;
				}
				else
				{
					TouchCountU++;
				}
				if(GPHeadbit==1)
				{
					GPRevoutTime++;
				}
				Clerlkey_TimeCount();
				ClerlTouch_TimeCount();
				ClerlCMD_TimeCount();
				Bt_ClearIntFlag(TIM0,BtUevIrq); //中断标志清零
    }
}
/******系统指示灯翻转********/
void Systme_led(void)
{
	if(Led_timecount>=150)
	{
		LED0_TOGGLE();
		Led_timecount=0;
	}
}




void Time_init(uint16_t u16Period)
{
	  uint16_t                  u16ArrValue;
    uint16_t                  u16CntValue;
    stc_bt_mode0_cfg_t     stcBtBaseCfg;
    
    DDL_ZERO_STRUCT(stcBtBaseCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer外设时钟使能
    
    stcBtBaseCfg.enWorkMode = BtWorkMode0;                  //定时器模式
    stcBtBaseCfg.enCT       = BtTimer;                      //定时器功能，计数时钟为内部PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv256;                 //PCLK/256
    stcBtBaseCfg.enCntMode  = Bt16bitArrMode;               //自动重载16位计数器/定时器
    stcBtBaseCfg.bEnTog     = FALSE;
    stcBtBaseCfg.bEnGate    = FALSE;
		//stcBtBaseCfg.pfnTim0Cb  = &Tim0_IRQHandler;
    stcBtBaseCfg.enGateP    = BtGatePositive;
    Bt_Mode0_Init(TIM0, &stcBtBaseCfg);                     //TIM0 的模式0功能初始化
    
    u16ArrValue = 0x10000 - u16Period;
    Bt_M0_ARRSet(TIM0, u16ArrValue);                        //设置重载值(ARR = 0x10000 - 周期)
    
    u16CntValue = 0x10000 - u16Period;
    Bt_M0_Cnt16Set(TIM0, u16CntValue);                      //设置计数初值
    
    Bt_ClearIntFlag(TIM0,BtUevIrq);                         //清中断标志   
    Bt_Mode0_EnableIrq(TIM0);                               //使能TIM0中断(模式0时只有一个中断)
    EnableNvic(TIM0_IRQn, IrqLevel3, TRUE);                 //TIM0中断使能
		Bt_M0_Run(TIM0);
}



