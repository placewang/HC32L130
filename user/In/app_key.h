#ifndef __APP_GPIO__
#define  __APP_GPIO__

#include "bsp_gpio.h"
#include "uart.h"

// Scan Code
#define VM_KEY_F8       7
#define VM_KEY_F1       8
#define VM_KEY_F2       9
#define VM_KEY_F3       10
#define VM_KEY_F4       11
#define VM_KEY_F5       12
#define VM_KEY_F6       13
#define VM_KEY_F7       14
#define VM_KEY_DESIGN   15
#define VM_KEY_A        16
#define VM_KEY_B        17
#define VM_KEY_1        18
#define VM_KEY_2        19
#define VM_KEY_3        20
#define VM_KEY_9        21
#define VM_KEY_5        22
#define VM_KEY_DOT      23
#define VM_KEY_C        24
#define VM_KEY_D        25
#define VM_KEY_4        26
#define VM_KEY_SETUP    27 /*=VM_KEY_OTHER*/
#define VM_KEY_LEFT     28
#define VM_KEY_ESCAPE   30
#define VM_KEY_DELETE   31
#define VM_KEY_E        32
#define VM_KEY_7        33
#define VM_KEY_0        34
#define VM_KEY_UP       35
#define VM_KEY_END      36
#define VM_KEY_DOWN     37
#define VM_KEY_MINUS    38
#define VM_KEY_6        39
#define VM_KEY_F        40
#define VM_KEY_8        41
#define VM_KEY_PGUP     42
#define VM_KEY_DISK     43
#define VM_KEY_RIGHT    44
#define VM_KEY_ENTER    46
#define VM_KEY_PGDN     47


extern uint8_t count_key;                               //抬起滤波
extern uint8_t count_key2;															//按下滤波


/********* \brief GPIO PORT类型定义***************************/ 

typedef struct
{
	uint8_t Row[3];
	uint8_t Col[3];
	uint8_t Num;
} KEY;

typedef struct _KeyScanInfoTag_
{
	unsigned char	headerFlag;				//packet header flag,now be 'x'.	
	unsigned char	ScanCode0;				//1st key pressed.
	unsigned char	upDown;					  //key state: up/down.
	unsigned char	ScanCode1;				//2nd key pressed,only used when multiple keys pressed simultaneously.
	unsigned char	ScanCode2;				//3rd key pressed,only used when multiple keys pressed simultaneously.
	unsigned char	keyCount;				  //how many keys was pressed simultaneously.
	unsigned char	endFlag;				  //packet end flag,now be 'y'. 
} KeyScanInfo;

void KeyScanTask(void);
void SendKeyInfo(void);
void GPSendKeyInfo(void);
void KeyInit(void);
#endif


