
#ifndef _APP_LPUART_H
#define _APP_LPUART_H


#include "ddl.h"



extern uint16_t LPrevouttiem;                  //���ճ�ʱ���

extern uint8_t  Getpack_end;                   //ȡ����ɱ�־λ
//extern uint8_t  GprsData_pack[17];             //Э��ͷ �������� ת�����ݰ���һ��ת��7�ֽڣ�Ԥ��7���ֽ�ȷ������ת����Ͳ����� Э��β
extern uint8_t  LPSendLen;                     //ת���ʵ�ʷ��ͳ���
extern uint16_t DataCountRev;                  //���ݽ��ռ���




uint8_t GPRSRev_data(void);
#endif





