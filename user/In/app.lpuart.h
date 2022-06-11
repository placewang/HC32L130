
#ifndef _APP_LPUART_H
#define _APP_LPUART_H


#include "ddl.h"



extern uint16_t LPrevouttiem;                  //接收超时检测

extern uint8_t  Getpack_end;                   //取包完成标志位
//extern uint8_t  GprsData_pack[17];             //协议头 数据类型 转发数据包，一次转发7字节，预留7个字节确保数据转义后发送不被拆开 协议尾
extern uint8_t  LPSendLen;                     //转义后实际发送长度
extern uint16_t DataCountRev;                  //数据接收计数




uint8_t GPRSRev_data(void);
#endif





