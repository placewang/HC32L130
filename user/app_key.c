#include "app_key.h"
#include "app_Cmd.h"

uint8_t count_key=0;                              //抬起滤波
uint8_t count_key2=0;                             //按下滤波
uint8_t g_bKeyPress = 0;													//按下抬起标志		
uint16_t a, b, c, a1, b1, c1;                     //键位暂存  
static uint8_t flag_key = 0;                      //按下标志
KEY key_down,key_down1;                           //按键记录

uint8_t Uart0Send_Key=0;                          //数据发送标志位

const uint16_t GPIO_COL_PROT[] = {Key_Col0_PORT,Key_Col1_PORT ,Key_Col2_PORT ,Key_Col3_PORT ,Key_Col4_PORT ,Key_Col5_PORT ,Key_Col6_PORT,Key_Col7_PORT };
const uint16_t GPIO_COL_PIN[]={Key_Col0_PIN,Key_Col1_PIN,Key_Col2_PIN,Key_Col3_PIN,Key_Col4_PIN,Key_Col5_PIN,Key_Col6_PIN,Key_Col7_PIN }; 	

const uint16_t GPIO_ROW_PROT[] ={Key_Row0_PORT,Key_Row1_PORT,Key_Row2_PORT,Key_Row3_PORT,Key_Row4_PORT,Key_Row5_PORT,Key_Row6_PORT};
const uint16_t GPIO_ROW_PIN[]={Key_Row0_PIN,Key_Row1_PIN,Key_Row2_PIN,Key_Row3_PIN,Key_Row4_PIN,Key_Row5_PIN,Key_Row6_PIN}; 

uint8_t VM_KEY_10[6][8] = {
  { VM_KEY_F1, VM_KEY_F2, VM_KEY_F3, VM_KEY_F4, VM_KEY_F5, VM_KEY_F6, VM_KEY_F8, VM_KEY_DESIGN },
  { VM_KEY_A, VM_KEY_B, VM_KEY_1, VM_KEY_2, VM_KEY_3, VM_KEY_9, VM_KEY_5, VM_KEY_DOT },
  { VM_KEY_C, VM_KEY_D, VM_KEY_4, VM_KEY_SETUP, VM_KEY_LEFT, 0x1D, VM_KEY_ESCAPE, VM_KEY_DELETE },
  { VM_KEY_E, VM_KEY_7, VM_KEY_0, VM_KEY_UP, VM_KEY_END, VM_KEY_DOWN, VM_KEY_MINUS, VM_KEY_6 },
  { VM_KEY_F, VM_KEY_8, VM_KEY_PGUP, VM_KEY_DISK, VM_KEY_RIGHT, VM_KEY_F7, VM_KEY_ENTER, VM_KEY_PGDN }
};
uint8_t VM_KEY_07[6][8]={
  { 0,              0,            VM_KEY_RIGHT,   VM_KEY_LEFT,  VM_KEY_END, VM_KEY_F6,  VM_KEY_DESIGN,  VM_KEY_UP},
  { VM_KEY_ENTER,   VM_KEY_DOWN,  VM_KEY_ESCAPE,  VM_KEY_F,     VM_KEY_E,   VM_KEY_DOT, VM_KEY_0,       VM_KEY_MINUS},
  { VM_KEY_DELETE,  VM_KEY_PGUP,  VM_KEY_PGDN,    VM_KEY_D,     VM_KEY_C,   VM_KEY_9,   VM_KEY_8,       VM_KEY_7},
  { VM_KEY_SETUP,   VM_KEY_DISK,  VM_KEY_F7,      VM_KEY_B,     VM_KEY_A,   VM_KEY_6,   VM_KEY_5,       VM_KEY_4},
  { VM_KEY_F5,      VM_KEY_F4,    VM_KEY_F3,      VM_KEY_F2,    VM_KEY_F1,  VM_KEY_3,   VM_KEY_2,       VM_KEY_1},
  { 0,              0,            0,              0,            0,          0,          0,              0 }
};

KeyScanInfo  g_KeyInfo;

void KeyInit(void)
{
  g_KeyInfo.headerFlag	= 'x';
  g_KeyInfo.ScanCode0		= 0xFF;
  g_KeyInfo.upDown		  = 'U';
  g_KeyInfo.ScanCode1   = 0xFF;
  g_KeyInfo.ScanCode2		= 0xFF;
  g_KeyInfo.keyCount		= 0x0;
  g_KeyInfo.endFlag		  = 'y'; 
}


/**********矩阵按键选择**************/
static void SelectRow(int rows)
{
  int i = 0;
  for (i = 0; i < 6; i++)
  {
    if (rows & (1 << i))
			 Gpio_ClrIO((en_gpio_port_t)GPIO_ROW_PROT[i],(en_gpio_pin_t)GPIO_ROW_PIN[i]);  
    else
			 Gpio_SetIO((en_gpio_port_t)GPIO_ROW_PROT[i],(en_gpio_pin_t)GPIO_ROW_PIN[i]);
  }  
}


/**********获取矩阵按键选择************/
static int GetSelectCol(void)
{  
  int iRet = 0;
  int i = 0;
  for (i = 0; i < 8; i++)
  {
    if (Gpio_GetInputIO((en_gpio_port_t)GPIO_COL_PROT[i],(en_gpio_pin_t)GPIO_COL_PIN[i]) == 0)
		 {		
				iRet |= (1 << i);
		 } 
	 }		
  return iRet;
}


/*********是否有按键，按下***************/
static int8_t key_is_down(void)
{
  SelectRow(0x3F);
  return GetSelectCol() != 0;
}


/*******获取被按下数量及键位*************/
static int8_t key_scan_row(KEY* Key_Down)
{
  int i;
  int row, col;
  memset(Key_Down, 0, sizeof(KEY));
  for (row = 0; row < 6; row++)
  {
    SelectRow((1 << row));
    col = GetSelectCol();
    if (col != 0)
    {
				for (i = 0; i < 8; i++)
				{
						if (col & (1 << i))
						{
								Key_Down->Num++;
								Key_Down->Row[Key_Down->Num - 1] = row;
								Key_Down->Col[Key_Down->Num - 1] = i;  
								if (Key_Down->Num >= 3)
								{
										return 1;
								}	
						}
				}
    }
  }
  return -1;
}




/*********按键是否改变*****************/
static uint8_t key_is_change(KEY* Key_Down,KEY* Key_Down1)
{
	int i;
	if (Key_Down->Num == Key_Down1->Num)
	{
		for (i = 0; i < Key_Down->Num; i++)
		{
			if (Key_Down->Row[i] != Key_Down1->Row[i] || Key_Down->Col[i] != Key_Down1->Col[i])
				return 0;
		}
    return 1;
	}
  return 0;
}




#define LCD_TYPE_10         1     // 10寸
#define LCD_TYPE_07         0     // 7寸
extern uint8_t g_iLcdType;

static uint8_t GetKeyScanCode(uint8_t row, uint8_t col)
{
  if (g_iLcdType == LCD_TYPE_10)
    return VM_KEY_10[row][col];
  else
    return VM_KEY_07[row][col];
}





void SendKeyInfo(void)
{
	Uart0Send_Key=1;
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.headerFlag);
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.ScanCode0);
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.upDown);
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.ScanCode1);
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.ScanCode0);
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.keyCount);
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.endFlag);
}

void GPSendKeyInfo(void)
{
	Uart0Send_Key=1;
	Uart_SendDataPoll(M0P_UART0,0x5A);
	Uart_SendDataPoll(M0P_UART0,0x01);
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.headerFlag);
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.ScanCode0);
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.upDown);
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.ScanCode1);
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.ScanCode0);
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.keyCount);
  Uart_SendDataPoll(M0P_UART0, g_KeyInfo.endFlag);
	Uart_SendDataPoll(M0P_UART0,0xA5);
}

void KeyScanTask(void)
{
  //----------键盘查询-------------
  if (key_is_down() == 1 && count_key2 > 20)
  {
			if (flag_key == 0)
			{
				count_key = 0;
				key_scan_row(&key_down);
				flag_key = 1;
			}
			else 
			{
						if (count_key > 20)
						{
								key_scan_row(&key_down1);
								if (key_is_change(&key_down, &key_down1) == 1)
								{
										a = GetKeyScanCode(key_down.Row[0], key_down.Col[0]);
										b = GetKeyScanCode(key_down.Row[1], key_down.Col[1]);
										c = GetKeyScanCode(key_down.Row[2], key_down.Col[2]);
										if (key_down.Num == 1)
										{
												if (a != a1)
												{
														g_KeyInfo.ScanCode0 = a;
														g_KeyInfo.ScanCode1 = 0xFF;
														g_KeyInfo.ScanCode2 = 0xFF;
														g_KeyInfo.keyCount	=	0x01;
														g_KeyInfo.upDown    = 'D';
														if(Toggle==0)
														{
															SendKeyInfo();
														}
														else if(Toggle==2)
														{
															GPSendKeyInfo();
														}
														g_bKeyPress = 1;
												}
										}
										if (key_down.Num == 2)
										{		
												if (a != a1 || b != b1)
												{
														g_KeyInfo.ScanCode0 = a;
														g_KeyInfo.ScanCode1 = b;
														g_KeyInfo.ScanCode2 = 0xFF;
														g_KeyInfo.keyCount	=	0x02;
														g_KeyInfo.upDown    = 'D';
														if(Toggle==0)
														{
															SendKeyInfo();
														}
														else if(Toggle==2)
														{
															GPSendKeyInfo();
														}
														g_bKeyPress = 1;
												}
										}
										if (key_down.Num == 3)
										{		
											if (a != a1 || b != b1 || c != c1)
											{
												g_KeyInfo.ScanCode0 = a;
												g_KeyInfo.ScanCode1 = b;
												g_KeyInfo.ScanCode2 = c;
												g_KeyInfo.keyCount	=	0x03;
												g_KeyInfo.upDown    = 'D';
												if(Toggle==0)
												{
													SendKeyInfo();
												}
												else if(Toggle==2)
												{
													GPSendKeyInfo();
												}
												g_bKeyPress = 1;
											}
										}
										
										a1 = a;
										b1 = b;
										c1 = c;
								}
								flag_key = 0;
						}
			}      
  }		
  else if (g_bKeyPress)
  {
    g_KeyInfo.headerFlag 	= 'x';
    g_KeyInfo.ScanCode0 	= a;
    g_KeyInfo.upDown		  = 'U';
    g_KeyInfo.ScanCode1	  = b;
    g_KeyInfo.ScanCode2	  = c;
    g_KeyInfo.keyCount	  = key_down.Num;
    g_KeyInfo.endFlag		  = 'y';
    if(Toggle==0)
		{
			SendKeyInfo();
		}
		else if(Toggle==2)
		{
			GPSendKeyInfo();
		}
    a1 = 0;
    b1 = 0;
    c1 = 0;
    g_bKeyPress = 0;
    count_key2 = 0;
  }  
}










