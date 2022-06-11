
#include "bsp_uart.h"
#include "queue.h"

volatile ElemType u8RxData;
volatile ElemType u8LPRxData;
//volatile uint8_t u8TxCnt=0;
//volatile uint8_t u8RxCnt=0;
#define MSLEN     320                                  //与主控交互缓存长度
#define GPSLEN    2048                                 //与GPS交互缓存长度

Queue        Uart_revQueuebuff;                        //接收缓存队列
QUEUE_DATA_T Uart_MS[MSLEN];													 //缓存区	

Queue        LPUart_revQueue;                          //GPS接收缓存队列
QUEUE_DATA_T LPUart_GPS[GPSLEN];											 //GPS缓存区	

//UART1中断函数
void Uart0_IRQHandler(void)
{
	
	  if(Uart_GetStatus(M0P_UART0, UartFE))               //错误请求
    {
				Uart_ClrStatus(M0P_UART0, UartFE);              //清除帧错误标记
				Uart_ClrStatus(M0P_UART0, UartRC);  
    }
		if(Uart_GetStatus(M0P_UART0, UartPE))
		{
			Uart_ClrStatus(M0P_UART0, UartPE);                 //清除奇偶校验错误标记
			Uart_ClrStatus(M0P_UART0, UartRC);  
		}
             
		if(Uart_GetStatus(M0P_UART0, UartRC)) 							 //UART1数据接收
		{
        Uart_ClrStatus(M0P_UART0, UartRC);              //清中断状态位
        u8RxData = Uart_ReceiveData(M0P_UART0);         //接收数据字节
				EnQueue(&Uart_revQueuebuff,u8RxData);					  //数据入队
		}

}



///<LPUART1 中断服务函数
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
    if(LPUart_GetStatus(M0P_LPUART0, LPUartRC))    ///接收数据
    {
        LPUart_ClrStatus(M0P_LPUART0, LPUartRC);   ///<清接收中断请求
        u8LPRxData=LPUart_ReceiveData(M0P_LPUART0);///读取数据
				EnQueue(&LPUart_revQueue,u8LPRxData);					  //数据入队
               
    }
}

void Uart0_init(uint32_t br)
{
	
		 stc_gpio_cfg_t stcGpioCfg;
		 stc_uart_cfg_t    stcCfg;
		 DDL_ZERO_STRUCT(stcGpioCfg);
		 DDL_ZERO_STRUCT(stcCfg);
		 stcCfg.enRunMode        = UartMskMode1;                         //模式
		 stcCfg.enStopBit        = UartMsk1bit;                          //1bit停止位
//    stcCfg.enMmdorCk        = UartMskDataOrAddr;                   //无检验
		 stcCfg.stcBaud.u32Baud  = br;                                   //波特率
		 stcCfg.stcBaud.enClkDiv = UartMsk16Or32Div;                     //通道采样分频配置
		 stcCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq();                //获得外设时钟（PCLK）频率值
		 
	   //GPIO set
     Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);           //使能GPIO模块时钟
		 Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);          //使能uart0模块时钟	
     ///<TX
     stcGpioCfg.enDir = GpioDirOut;
     stcGpioCfg.enPu = GpioPuEnable;
     stcGpioCfg.enCtrlMode = GpioAHB;
     stcGpioCfg.enDrv = GpioDrvH;
     stcGpioCfg.enOD = GpioOdDisable;
     stcGpioCfg.enPd = GpioPdDisable;

     Gpio_Init(GpioPortA, GpioPin9, &stcGpioCfg);
     Gpio_SetAfMode(GpioPortA, GpioPin9, GpioAf1);                   //配置PA9 端口为URART0_TX
		 Uart_ClrStatus(M0P_UART0,UartTC);                                ///<清发送请求
		 Uart_Init(M0P_UART0, &stcCfg);                                   //串口初始化	
		 ///<RX
    stcGpioCfg.enDir = GpioDirIn;
    stcGpioCfg.enPu = GpioPuEnable;
    stcGpioCfg.enCtrlMode = GpioAHB;
    stcGpioCfg.enDrv = GpioDrvL;
    stcGpioCfg.enOD = GpioOdDisable;
    stcGpioCfg.enPd = GpioPdDisable;
    Gpio_Init(GpioPortA, GpioPin10, &stcGpioCfg);
    Gpio_SetAfMode(GpioPortA, GpioPin10, GpioAf1);                   //配置PA10 端口为URART0_RX
	
		InitQueue(&Uart_revQueuebuff,Uart_MS,MSLEN);                     //缓存队列初始化

    ///<UART中断使能
    Uart_ClrStatus(M0P_UART0,UartRC);                                //清接收请求
    Uart_EnableIrq(M0P_UART0,UartRxIrq);                             //使能串口接收中断
    EnableNvic(UART0_IRQn, IrqLevel0, TRUE);                         //系统中断使能
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
	  stcGpioCfg.enPu = GpioPuEnable;                  //端口上下拉配置->上拉
    stcGpioCfg.enPd = GpioPdDisable;
    Gpio_Init(GpioPortB,GpioPin10,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortB,GpioPin10,GpioAf4);     //配置PB10为LPUART1_TX
    
    //<RX
    stcGpioCfg.enDir =  GpioDirIn;
    Gpio_Init(GpioPortB,GpioPin11,&stcGpioCfg);
    Gpio_SetAfMode(GpioPortB,GpioPin11,GpioAf3);     //配置PB11为LPUART1_RX

    ///<外设模块时钟使能
    Sysctrl_SetPeripheralGate(SysctrlPeripheralLpUart0,TRUE);    
    
    ///<LPUART 初始化
    stcCfg.enStopBit = LPUart1bit;                   //1停止位    
//    stcCfg.enMmdorCk = LPUartEven;                   //偶校验
    stcCfg.stcBaud.enSclkSel = LPUartMskPclk;        //传输时钟源
    stcCfg.stcBaud.u32Sclk = Sysctrl_GetPClkFreq();  //PCLK获取
    stcCfg.stcBaud.enSclkDiv = LPUartMsk16Or32Div;   //采样分频
    stcCfg.stcBaud.u32Baud = br;                     //波特率
    stcCfg.enRunMode = LPUartMskMode1;               //工作模式
    LPUart_Init(M0P_LPUART0, &stcCfg);
		InitQueue(&LPUart_revQueue,LPUart_GPS,GPSLEN); 
    ///<LPUART 中断使能
    LPUart_ClrStatus(M0P_LPUART0,LPUartRC);          ///<清接收中断请求
    LPUart_DisableIrq(M0P_LPUART0,LPUartRxIrq);      ///<禁止接收中断
    LPUart_EnableIrq(M0P_LPUART0,LPUartRxIrq);       ///<使能发送中断

    EnableNvic(LPUART0_IRQn,IrqLevel1,TRUE);         ///<系统中断使能

}
/*******规避串口电源与晶振稳定前的状态拉高******/
void UART0_H(void)
{
	
		 stc_gpio_cfg_t stcGpioCfg;
		 DDL_ZERO_STRUCT(stcGpioCfg);
	   //GPIO set
     Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);           //使能GPIO模块时钟
		 Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);          //使能uart0模块时钟	
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




