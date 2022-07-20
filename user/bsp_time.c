#include "app_Cmd.h"
#include  "bsp_time.h"
#include  "bsp_gpio.h"
#include  "app_key.h"
#include  "app_gprsCmd.h"
extern uint16_t LPrevouttiem;     //���ճ�ʱ���
extern uint16_t DataoutTime;      //��ʾ��������ͨѶ��ʱ 

//static volatile uint8_t u8TrimTestFlag   = 0;

volatile uint16_t Led_timecount=0;


/*************�尴���˲�����******************/
void Clerlkey_TimeCount(void)
{
	if(count_key>40)
	{
		count_key=0;
		count_key2=0;
	}
}
/*************�����˲���������******************/
void ClerlTouch_TimeCount(void)
{
	if(TouchCountD>=60 || TouchCountU>=60)
	{
		TouchCountD=0;
		TouchCountU=0;
	}
	
}
/*************Э�齻����ʱ��������******************/
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




/*******TIM0�жϷ�����*****************/
void Tim0_IRQHandler(void)
{

    //Timer0 ģʽ0 ����ж�
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
				Bt_ClearIntFlag(TIM0,BtUevIrq); //�жϱ�־����
    }
}
/******ϵͳָʾ�Ʒ�ת********/
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
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralBaseTim, TRUE); //Base Timer����ʱ��ʹ��
    
    stcBtBaseCfg.enWorkMode = BtWorkMode0;                  //��ʱ��ģʽ
    stcBtBaseCfg.enCT       = BtTimer;                      //��ʱ�����ܣ�����ʱ��Ϊ�ڲ�PCLK
    stcBtBaseCfg.enPRS      = BtPCLKDiv256;                 //PCLK/256
    stcBtBaseCfg.enCntMode  = Bt16bitArrMode;               //�Զ�����16λ������/��ʱ��
    stcBtBaseCfg.bEnTog     = FALSE;
    stcBtBaseCfg.bEnGate    = FALSE;
		//stcBtBaseCfg.pfnTim0Cb  = &Tim0_IRQHandler;
    stcBtBaseCfg.enGateP    = BtGatePositive;
    Bt_Mode0_Init(TIM0, &stcBtBaseCfg);                     //TIM0 ��ģʽ0���ܳ�ʼ��
    
    u16ArrValue = 0x10000 - u16Period;
    Bt_M0_ARRSet(TIM0, u16ArrValue);                        //��������ֵ(ARR = 0x10000 - ����)
    
    u16CntValue = 0x10000 - u16Period;
    Bt_M0_Cnt16Set(TIM0, u16CntValue);                      //���ü�����ֵ
    
    Bt_ClearIntFlag(TIM0,BtUevIrq);                         //���жϱ�־   
    Bt_Mode0_EnableIrq(TIM0);                               //ʹ��TIM0�ж�(ģʽ0ʱֻ��һ���ж�)
    EnableNvic(TIM0_IRQn, IrqLevel3, TRUE);                 //TIM0�ж�ʹ��
		Bt_M0_Run(TIM0);
}



