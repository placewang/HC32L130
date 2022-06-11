#ifndef  __UART__
#define  __UART__

#include "ddl.h"

#include "uart.h"
#include "sysctrl.h"
#include "lpuart.h"
#include "lpm.h"
#include "gpio.h"


#define DEFAULTBOUD                         12500                                 //Ä¬ÈÏ²¨ÌØÂÊ                      
extern volatile  uint8_t u8TxCnt;
extern volatile  uint8_t u8RxCnt;
extern volatile uint8_t u8RxData;

void Uart0_init(uint32_t br);
void Lpuart_init(uint32_t br);
void UART0_H(void);

#endif


