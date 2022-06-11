#ifndef __GPIO__
#define  __GPIO__

#include "ddl.h"
#include "gpio.h"
//系统指示灯
#define  LED0_PORT           (GpioPortC)
#define  LED0_PIN            (GpioPin13)
#define  LED0_TOGGLE()       (Gpio_GetInputIO(LED0_PORT, LED0_PIN))?(Gpio_ClrIO(LED0_PORT, LED0_PIN)):(Gpio_SetIO(LED0_PORT, LED0_PIN))

//蜂鸣器
#define  BUZZ_PORT          (GpioPortB)
#define  BUZZ_PIN           (GpioPin15)
#define  BUZZ_ON()           Gpio_SetIO(BUZZ_PORT, BUZZ_PIN )
#define  BUZZ_OFF()          Gpio_ClrIO(BUZZ_PORT, BUZZ_PIN )
#define  BUZZ_IN_BO0L()      Gpio_GetInputIO(BUZZ_PORT ,BUZZ_PIN )
//触屏背光开关
#define Backlight_PORT       (GpioPortA)
#define Backlight_PIN				 (GpioPin8)
#define Backlight_ON()			 Gpio_SetIO(Backlight_PORT,Backlight_PIN)
#define Backlight_OFF()			 Gpio_ClrIO(Backlight_PORT,Backlight_PIN)
//矩阵按键
#define  Key_Col0_PORT      (GpioPortB)
#define  Key_Col0_PIN		    (GpioPin9)

#define  Key_Col1_PORT      (GpioPortB)
#define  Key_Col1_PIN		    (GpioPin8)

#define  Key_Col2_PORT      (GpioPortB)
#define  Key_Col2_PIN		    (GpioPin6)

#define  Key_Col3_PORT      (GpioPortB)
#define  Key_Col3_PIN		    (GpioPin4)

#define  Key_Col4_PORT      (GpioPortB)
#define  Key_Col4_PIN		    (GpioPin3)

#define  Key_Col5_PORT      (GpioPortA)
#define  Key_Col5_PIN		    (GpioPin15)

#define  Key_Col6_PORT      (GpioPortD)
#define  Key_Col6_PIN		    (GpioPin7)

#define  Key_Col7_PORT      (GpioPortD)
#define  Key_Col7_PIN		    (GpioPin6)

#define  Key_Row0_PORT      (GpioPortC)
#define  Key_Row0_PIN		    (GpioPin14)

#define  Key_Row1_PORT      (GpioPortC)
#define  Key_Row1_PIN		    (GpioPin15)

#define  Key_Row2_PORT      (GpioPortB)
#define  Key_Row2_PIN		    (GpioPin12)

#define  Key_Row3_PORT      (GpioPortB)
#define  Key_Row3_PIN		    (GpioPin13)

#define  Key_Row4_PORT      (GpioPortB)
#define  Key_Row4_PIN		    (GpioPin14)

#define  Key_Row5_PORT      (GpioPortA)
#define  Key_Row5_PIN		    (GpioPin11)

#define  Key_Row6_PORT      (GpioPortA)
#define  Key_Row6_PIN		    (GpioPin12)

//液晶识别
#define  Screen0_PORT         (GpioPortA)
#define  Screen0_PIN	  	    (GpioPin2)

#define  Screen1_PORT         (GpioPortB)
#define  Screen1_PIN		      (GpioPin2)

#define  Screen0_IN		         Gpio_GetInputIO(Screen0_PORT ,Screen0_PIN)
#define  Screen1_IN		         Gpio_GetInputIO(Screen1_PORT ,Screen1_PIN)

void Led_init(void);
void Buzz_init(void);
void Backlight_init(void);
void GPIO_Key_init(void);
uint8_t GetLcdType(void);
void Screen_init(void);
#endif

