#ifndef  __UART__
#define  __UART__

#include "ddl.h"

#include "uart.h"
#include "sysctrl.h"
#include "lpuart.h"
#include "lpm.h"
#include "gpio.h"


#define DEFAULTBOUD                         12500      //默认波特率        
#define MSLEN     320                                  //与主控交互缓存长度
#define GPSLEN    2048                                 //与GPS交互缓存长度

extern volatile  uint8_t u8TxCnt;
extern volatile  uint8_t u8RxCnt;
extern volatile uint8_t u8RxData;
extern unsigned char Uart_MS[MSLEN];													 //缓存区	
uint8_t Uart0_init(uint32_t br);
void Lpuart_init(uint32_t br);
void UART0_H(void);

#endif


