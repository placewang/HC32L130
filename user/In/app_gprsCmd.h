#ifndef  APP__GPRSCMD__H
#define  APP__GPRSCMD__H
#include "ddl.h"

extern uint8_t  GPHeadbit;                                //ȡ����ͷ��־λ
extern uint16_t GPRevoutTime;   													//�����ճ�ʱ����	
extern uint8_t  GpTouchRbit;                              //���ض����ݱ�־λ
extern uint8_t  GpVersion_Inquire;                        //�汾��ѯ��־λ
extern uint16_t Gps_RevCount;                             //��Ч���ݽ��ռ���
extern uint8_t GpW_Flash_s;                               //��дFlash �ɹ���־λ 																						
extern uint8_t GpR_Flash_s;																					
extern uint8_t Penetratebit;   													  //͸����־λ
extern uint8_t GpKey_Inquire; 					  								//������ѯ��־λ
extern uint8_t  Gps_TranslatePackDtata[200];              //���ݷ�����ָ���
extern uint16_t NumberEscapes;													  //ת�����	
extern uint8_t  GpTogshakeTime;                           //���������л����������ּ�ʱ   
extern uint8_t  GpSystemRest;                             //ϵͳ��λ������־λ
//uint8_t GprsCmd_Task(void);

void GprsCmd_Task(void);
void GPclearData(void);
void ToggleConfirmationHandshake(void);

#endif


