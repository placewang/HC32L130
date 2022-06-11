#ifndef  APP__GPRSCMD__H
#define  APP__GPRSCMD__H
#include "ddl.h"

extern uint8_t  GPHeadbit;                                //取到包头标志位
extern uint16_t GPRevoutTime;   													//包接收超时计数	
extern uint8_t  GpTouchRbit;                              //触控读数据标志位
extern uint8_t  GpVersion_Inquire;                        //版本查询标志位
extern uint16_t Gps_RevCount;                             //有效数据接收计数
extern uint8_t GpW_Flash_s;                               //读写Flash 成功标志位 																						
extern uint8_t GpR_Flash_s;																					
extern uint8_t Penetratebit;   													  //透传标志位
extern uint8_t GpKey_Inquire; 					  								//按键查询标志位
extern uint8_t  Gps_TranslatePackDtata[200];              //数据翻译后包指令缓存
extern uint16_t NumberEscapes;													  //转义次数	
extern uint8_t  GpTogshakeTime;                           //主控与屏切换波特率握手计时   
extern uint8_t  GpSystemRest;                             //系统复位重启标志位
//uint8_t GprsCmd_Task(void);

void GprsCmd_Task(void);
void GPclearData(void);
void ToggleConfirmationHandshake(void);

#endif


