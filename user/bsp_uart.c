
#include "bsp_uart.h"
#include "queue.h"

volatile ElemType u8RxData;
volatile ElemType u8LPRxData;
//volatile uint8_t u8TxCnt=0;
//volatile uint8_t u8RxCnt=0;
#define MSLEN     320                                  //�����ؽ������泤��
#define GPSLEN    2048                                 //��GPS�������泤��

Queue        Uart_revQueuebuff;                        //���ջ������
QUEUE_DATA_T Uart_MS[MSLEN];													 //������	

Queue        LPUart_revQueue;                          //GPS���ջ������
QUEUE_DATA_T LPUart_GPS[GPSLEN];											 //GPS������	

//UART1�жϺ���
void Uart0_IRQHandler(void)
{
	
	  if(Uart_GetStatus(M0P_UART0, UartFE))               //��������
    {
				Uart_ClrStatus(M0P_UART0, UartFE);              //���֡������
				Uart_ClrStatus(M0P_UART0, UartRC);  
    }
		if(Uart_GetStatus(M0P_UART0, UartPE))
		{
			Uart_ClrStatus(M0P_UART0, UartPE);                 //�����żУ�������
			Uart_ClrStatus(M0P_UART0, UartRC);  
		}
             
		if(Uart_GetStatus(M0P_UART0, UartRC)) 							 //UART1���ݽ���
		{
        Uart_ClrStatus(M0P_UART0, UartRC);              //���ж�״̬λ
        u8RxData = Uart_ReceiveData(M0P_UART0);         //���������ֽ�
				EnQueue(&Uart_revQueuebuff,u8RxData);					  //�������
		}

}



///<LPUART1 �жϷ�����
void LpUart0_IRQHandler(void)
{
    if(LPUart_GetStatus(M0P_LPUART0,  LPUartFE))    
     { 
					LPUart_ClrStatus(M0P_LPUART0, LPUartFE);
		 }
    if(LPUart_GetStatus(M0P_LPUART0,  LPUartPE))    
    { 
			LPUart_ClrStatus(M0P_LPUART0, LPUartPE);
		}
    if(LPUart_GetStatus(M0P_LPUART0, LPUartRC))    ///��������
    {
        LPUart_ClrStatus(M0P_LPUART0, LPUartRC);   ///<������ж�����
        u8LPRxData=LPUart_ReceiveData(M0P_LPUART0);///��ȡ����
				EnQueue(&LPUart_revQueue,u8LPRxData);					  //�������
               
    }
}

void Uart0_init(uint32_t br)
{
	
		 stc_gpio_cfg_t stcGpioCfg;
		 stc_uart_cfg_t    stcCfg;
		 DDL_ZERO_STRUCT(stcGpioCfg);
		 DDL_ZERO_STRUCT(stcCfg);
		 stcCfg.enRunMode        = UartMskMode1;                         //ģʽ
		 stcCfg.enStopBit        = UartMsk1bit;                          //1bitֹͣλ
//    stcCfg.enMmdorCk        = UartMskDataOrAddr;                   //�޼���
		 stcCfg.stcBaud.u32Baud  = br;                                   //������
		 stcCfg.stcBaud.enClkDiv = UartMsk16Or32Div;                     //ͨ��������Ƶ����
		 stcCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq();                //�������ʱ�ӣ�PCLK��Ƶ��ֵ
		 
	   //GPIO set
     Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);           //ʹ��GPIOģ��ʱ��
		 Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);          //ʹ��uart0ģ��ʱ��	
     ///<TX
     stcGpioCfg.enDir = GpioDirOut;
     stcGpioCfg.enPu = GpioPuEnable;
     stcGpioCfg.enCtrlMode = GpioAHB;
     stcGpioCfg.enDrv = GpioDrvH;
     stcGpioCfg.enOD = GpioOdDisable;
     stcGpioCfg.enPd = GpioPdDisable;

     Gpio_Init(GpioPortA, GpioPin9, &stcGpioCfg);
     Gpio_SetAfMode(GpioPortA, GpioPin9, GpioAf1);                   //����PA9 �˿�ΪURART0_TX
		 Uart_ClrStatus(M0P_UART0,UartTC);                                ///<�巢������
		 Uart_Init(M0P_UART0, &stcCfg);                                   //���ڳ�ʼ��	
		 ///<RX
    stcGpioCfg.enDir = GpioDirIn;
    stcGpioCfg.enPu = GpioPuEnable;
    stcGpioCfg.enCtrlMode = GpioAHB;
    stcGpioCfg.enDrv = GpioDrvL;
    stcGpioCfg.enOD = GpioOdDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    Gpio_Init(GpioPortA, GpioPin10, &stcGpioCfg);
    Gpio_SetAfMode(GpioPortA, GpioPin10, GpioAf1);                   //����PA10 �˿�ΪURART0_RX
	
		InitQueue(&Uart_revQueuebuff,Uart_MS,MSLEN);                     //������г�ʼ��

    ///<UART�ж�ʹ��
    Uart_ClrStatus(M0P_UART0,UartRC);                                //���������
    Uart_EnableIrq(M0P_UART0,UartRxIrq);                             //ʹ�ܴ��ڽ����ж�
    EnableNvic(UART0_IRQn, IrqLevel0, TRUE);                         //ϵͳ�ж�ʹ��
}


void Lpuart_init(uint32_t br)
{
		stc_gpio_cfg_t stcGpioCfg;
		stc_lpuart_cfg_t  stcCfg;
	
		DDL_ZERO_STRUCT(stcGpioCfg);
		DDL_ZERO_STRUCT(stcCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
    
    ///<TX
    stcGpioCfg.enDir =  GpioDirOut;
	  stcGpioCfg.enPu = GpioPuEnable;                  //�˿�����������->����
    stcGpioCfg.enPd = GpioPdDisable;
    Gpio_Init(GpioPortB,GpioPin10,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortB,GpioPin10,GpioAf4);     //����PB10ΪLPUART1_TX
    
    //<RX
    stcGpioCfg.enDir =  GpioDirIn;
    Gpio_Init(GpioPortB,GpioPin11,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortB,GpioPin11,GpioAf3);     //����PB11ΪLPUART1_RX

    ///<����ģ��ʱ��ʹ��
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLpUart0,TRUE);    
    
    ///<LPUART ��ʼ��
    stcCfg.enStopBit = LPUart1bit;                   //1ֹͣλ    
//    stcCfg.enMmdorCk = LPUartEven;                   //żУ��
    stcCfg.stcBaud.enSclkSel = LPUartMskPclk;        //����ʱ��Դ
    stcCfg.stcBaud.u32Sclk = Sysctrl_GetPClkFreq();  //PCLK��ȡ
    stcCfg.stcBaud.enSclkDiv = LPUartMsk16Or32Div;   //������Ƶ
    stcCfg.stcBaud.u32Baud = br;                     //������
    stcCfg.enRunMode = LPUartMskMode1;               //����ģʽ
    LPUart_Init(M0P_LPUART0, &stcCfg);
		InitQueue(&LPUart_revQueue,LPUart_GPS,GPSLEN); 
    ///<LPUART �ж�ʹ��
    LPUart_ClrStatus(M0P_LPUART0,LPUartRC);          ///<������ж�����
    LPUart_DisableIrq(M0P_LPUART0,LPUartRxIrq);      ///<��ֹ�����ж�
    LPUart_EnableIrq(M0P_LPUART0,LPUartRxIrq);       ///<ʹ�ܷ����ж�

    EnableNvic(LPUART0_IRQn,IrqLevel1,TRUE);         ///<ϵͳ�ж�ʹ��

}
/*******��ܴ��ڵ�Դ�뾧���ȶ�ǰ��״̬����******/
void UART0_H(void)
{
	
		 stc_gpio_cfg_t stcGpioCfg;
		 DDL_ZERO_STRUCT(stcGpioCfg);
	   //GPIO set
     Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);           //ʹ��GPIOģ��ʱ��
		 Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);          //ʹ��uart0ģ��ʱ��	
     ///<TX
     stcGpioCfg.enDir = GpioDirOut;
     stcGpioCfg.enPu = GpioPuEnable;
     stcGpioCfg.enCtrlMode = GpioAHB;
     stcGpioCfg.enDrv = GpioDrvH;
     stcGpioCfg.enOD = GpioOdDisable;
     stcGpioCfg.enPd = GpioPdDisable;
     Gpio_Init(GpioPortA, GpioPin9, &stcGpioCfg);
		 Gpio_SetIO(GpioPortA, GpioPin9);
		 ///<RX
		 stcGpioCfg.enDir = GpioDirIn;
		 stcGpioCfg.enPu = GpioPuEnable;
		 stcGpioCfg.enCtrlMode = GpioAHB;
		 stcGpioCfg.enDrv = GpioDrvL;
		 stcGpioCfg.enOD = GpioOdDisable;
		 stcGpioCfg.enPd = GpioPdDisable;
		 Gpio_Init(GpioPortA, GpioPin10, &stcGpioCfg);
		 Gpio_SetIO(GpioPortA,GpioPin10);

}




