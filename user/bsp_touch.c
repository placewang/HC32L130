#include "bsp_touch.h"
#include "bsp_gpio.h"
/*模拟SPI*/

uint8_t TouchRbit=0;                             //触控读数据标志位
uint8_t LPSendInterruptbit=0;                    //触控触发 打断GPSRS与主控的当前透传标志位  

uint8_t TouchSbit=0;                             //触控XY数据读取完成标志位
uint8_t TouchUbit=0;														 //触控抬起标志位		
uint8_t TouchCountD=0;													 //触控按下滤波
uint8_t TouchCountU=0;                           //触控抬起滤波

uint16_t X_Touch_val,Y_Touch_val;								 //屏幕X/Y坐标存储

void TSC2046_Init ( void )
{
	  stc_gpio_cfg_t stcGpioCfg;
		Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);                             // 打开GPIO外设时钟门控  
    
		stcGpioCfg.enDir = GpioDirOut;   
		stcGpioCfg.enOD = GpioOdDisable;                                                    //< 端口开漏输出配置->开漏输出关闭
		stcGpioCfg.enDrv = GpioDrvH;                                                        //< 端口驱动能力配置->高驱动能力  
    stcGpioCfg.enPu = GpioPuEnable;                                                     //< 端口上下拉配置->上拉
    stcGpioCfg.enPd = GpioPdDisable;
	  stcGpioCfg.enCtrlMode = GpioAHB;                                                    //< 端口输入/输出值寄存器总线控制模式配置->AHB
    Gpio_Init(GPIO_SPI_Touch_cs_PORT ,GPIO_SPI_Touch_cs_PIN ,&stcGpioCfg);
		Touch_cs_ON(); 

	  stcGpioCfg.enPu = GpioPuDisable;                                                    //< 端口上下拉配置->下拉
    stcGpioCfg.enPd = GpioPdEnable;
		Gpio_Init(GPIO_SPI_Touch_mosi_PORT,GPIO_SPI_Touch_mosi_PIN,&stcGpioCfg);
		Gpio_Init(GPIO_SPI_Touch_clk_PORT,GPIO_SPI_Touch_clk_PIN,&stcGpioCfg);  
		Touch_mosi_0();
		Touch_clk_Low(); 
	
		stcGpioCfg.enDir = GpioDirIn;                                                                      
		stcGpioCfg.enPu = GpioPuEnable;                                                     // 端口上下拉配置->上拉
    stcGpioCfg.enPd = GpioPdDisable;
		Gpio_Init(GPIO_Touch_BUSY_PORT,GPIO_Touch_BUSY_PIN,&stcGpioCfg);                     //TSC2046 BUSY端口初始化
		Gpio_Init(GPIO_SPI_Touch_miso_PORT,GPIO_SPI_Touch_miso_PIN,&stcGpioCfg);             //TSC2046 miso端口初始化 
		
//		stcGpioCfg.enPu = GpioPuDisable;                                                  // 端口上拉配
//    stcGpioCfg.enPd = GpioPdEnable;
		Gpio_Init(GPIO_Touch_OPENRQ_PORT,GPIO_Touch_OPENRQ_PIN,&stcGpioCfg);                //TSC2046 OPENRQ端口初始化
			
//	  Gpio_EnableIrq(GPIO_Touch_BUSY_PORT,GPIO_Touch_BUSY_PIN,GpioIrqFalling);         //< 打开并配置OPENRQ为下降沿中断 
//	  EnableNvic(PORTA_IRQn, IrqLevel3, TRUE);                                             //< 使能端口PORTA系统中断	
}


void Spi_init(void)
{
	  stc_gpio_cfg_t GpioInitStruct;
	
		stc_spi_cfg_t  SpiInitStruct;
    DDL_ZERO_STRUCT(GpioInitStruct);
		
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);
		Sysctrl_SetPeripheralGate(SysctrlPeripheralSpi0,TRUE);
	
    //SPI0引脚配置:主机
    GpioInitStruct.enDrv = GpioDrvH;
    GpioInitStruct.enDir = GpioDirOut;

    Gpio_Init(GpioPortA, GpioPin7,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin7,GpioAf1);        //配置引脚PA7作为SPI0_MOSI

    Gpio_Init(GpioPortA, GpioPin4,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin4,GpioAf1);         //配置引脚PA04作为SPI0_CS

    Gpio_Init(GpioPortA, GpioPin5,&GpioInitStruct);
    Gpio_SetAfMode(GpioPortA, GpioPin5,GpioAf1);         //配置引脚PA05作为SPI0_SCK

    //SPI0模块配置：主机
    SpiInitStruct.enSpiMode = SpiMskMaster;   //配置位主机模式
    SpiInitStruct.enPclkDiv = SpiClkMskDiv128;  //波特率：fsys/128
    SpiInitStruct.enCPHA    = SpiMskCphafirst;//第一边沿采样
    SpiInitStruct.enCPOL    = SpiMskcpollow;  //极性为低
    Spi_Init(M0P_SPI0, &SpiInitStruct);
//    GpioInitStruct.enDir = GpioDirIn;
//    Gpio_Init(GpioPortA, GpioPin6,&GpioInitStruct);
//    Gpio_SetAfMode(GpioPortA, GpioPin6,GpioAf1);         

}




/**
  * @brief  TSC2046 的写入命令
  * @param  ucCmd ：命令
  *   该参数为以下值之一：
  *     @arg 0x90 :通道Y+的选择控制字
  *     @arg 0xd0 :通道X+的选择控制字
  * @retval 无
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
  * @brief  TSC2046 的读取命令
  * @param  无
  * @retval 读取到的数据
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
  * @brief  对 TSC2046 选择一个模拟通道后，启动ADC，并返回ADC采样结果
  * @param  ucChannel
  *   该参数为以下值之一：
  *     @arg 0x90 :通道Y+的选择控制字
  *     @arg 0xd0 :通道X+的选择控制字
  * @retval 该通道的ADC采样结果
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
  * @brief  读取 TSC2046 的X通道和Y通道的AD值（12 bit，最大是4096）
  * @param  sX_Ad ：存放X通道AD值的地址
  * @param  sY_Ad ：存放Y通道AD值的地址
  * @retval 无
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


///< PortA中断服务函数
void PortA_IRQHandler(void)
{
    if(TRUE == Gpio_GetIrqStatus(GPIO_Touch_BUSY_PORT,GPIO_Touch_BUSY_PIN))
    {            
        Gpio_ClearIrq(GPIO_Touch_BUSY_PORT  ,GPIO_Touch_BUSY_PIN);    
    }
}  





