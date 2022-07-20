
#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "ddl.h"
#include "gpio.h"
#include "spi.h"
#define	            TSC2046_CHANNEL_X 	                          0x90 	          //通道Y+的选择控制字	
#define	            TSC2046_CHANNEL_Y 	                          0xd0	          //通道X+的选择控制字

#define  GPIO_SPI_Touch_clk_PORT            (GpioPortA)
#define  GPIO_SPI_Touch_clk_PIN             (GpioPin5)

#define  GPIO_SPI_Touch_cs_PORT             (GpioPortA)
#define  GPIO_SPI_Touch_cs_PIN              (GpioPin4)

#define  GPIO_SPI_Touch_miso_PORT           (GpioPortA)
#define  GPIO_SPI_Touch_miso_PIN            (GpioPin6)

#define  GPIO_SPI_Touch_mosi_PORT           (GpioPortA)
#define  GPIO_SPI_Touch_mosi_PIN            (GpioPin7)

#define  GPIO_Touch_BUSY_PORT               (GpioPortA)
#define  GPIO_Touch_BUSY_PIN                (GpioPin3)

#define  GPIO_Touch_OPENRQ_PORT             (GpioPortB)
#define  GPIO_Touch_OPENRQ_PIN              (GpioPin0)


#define  Touch_cs_ON()          Gpio_SetIO(GPIO_SPI_Touch_cs_PORT ,GPIO_SPI_Touch_cs_PIN)
#define  Touch_cs_OFF()         Gpio_ClrIO(GPIO_SPI_Touch_cs_PORT ,GPIO_SPI_Touch_cs_PIN)

#define  Touch_clk_High()       Gpio_SetIO(GPIO_SPI_Touch_clk_PORT,GPIO_SPI_Touch_clk_PIN)    
#define  Touch_clk_Low()        Gpio_ClrIO(GPIO_SPI_Touch_clk_PORT,GPIO_SPI_Touch_clk_PIN)   

#define  Touch_mosi_1()         Gpio_SetIO(GPIO_SPI_Touch_mosi_PORT,GPIO_SPI_Touch_mosi_PIN)
#define  Touch_mosi_0()         Gpio_ClrIO(GPIO_SPI_Touch_mosi_PORT,GPIO_SPI_Touch_mosi_PIN)

#define  Touch_miso_IN()        Gpio_GetInputIO(GPIO_SPI_Touch_miso_PORT,GPIO_SPI_Touch_miso_PIN)

#define	 Touch_PENIRQ_ActiveLevel            0
#define  Touch_PENIRQ_Read()		Gpio_GetInputIO(GPIO_Touch_OPENRQ_PORT,GPIO_Touch_OPENRQ_PIN)

#define  Touch_BUSY_IN()          Gpio_GetInputIO(GPIO_Touch_BUSY_PORT ,GPIO_Touch_BUSY_PIN )



extern uint8_t TouchCountD; 													 //触控按下滤波
extern uint8_t TouchCountU;                            //触控抬起滤波
extern uint8_t TouchSbit;                              //触控XY数据读取完成标志位
extern uint8_t TouchUbit; 														 //触控抬起标志位	
extern uint16_t X_Touch_val;	  											 //屏幕X/Y坐标存储
extern uint16_t Y_Touch_val;				    										 
extern uint8_t TouchRbit;                              //触控读数据标志位
extern uint8_t LPSendInterruptbit;                     //触控触发 打断GPSRS与主控的当前透传标志位 

void TSC2046_Init ( void );
uint8_t Rev_Touch_XY(void);
uint8_t LogicaRev_Touch_XY(void);
void Spi_init(void);
#endif 



