/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2013-2022 Lai Yin-Chuan. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _LaiSch__h_
#define _LaiSch__h_

#include <Arduino.h>
//=========================================================================
typedef enum {_32768Hz,_16384Hz,_8192Hz,_4096Hz,_2048Hz,_1024Hz,_512Hz,_256Hz
,_128Hz,_64Hz,_32Hz,_16Hz,_8Hz,_4Hz,_2Hz,_1Hz,_2Second,_4Second,_8Second
,_16Second,_32Second,_64Second,_128Second,_256Second,_512Second,_1024Second
,_2048Second,_4096Second,_8192Second,_16384Second,_32768Second}TimeFlag;
//=========================================================================
#define LED_Control_Pin_In_Cathode    
#ifdef LED_Control_Pin_In_Cathode            
typedef enum {light=0, dark=1}LED_Status;   	//The LED's Anode pin is connected in 
#else											//series with the resistor to the power terminal
typedef enum {light=1, dark=0}LED_Status;		//and the Cathode is connected to the MCU's I/O pin.
#endif
//=========================================================================
#ifdef ESP32
extern hw_timer_t * timer ;
extern volatile SemaphoreHandle_t timerSemaphore;
extern portMUX_TYPE timerMux;
#endif
//=========================================================================
//extern uint32_t TSMf,TSM,TSMp,WaitTick;
//extern uint8_t LaiTick;
extern uint8_t PWM_SetValue;
//=========================================================================
void LaiSchCore(void);
void LaiSchCoreInit(void);
void LaiSchWait(uint32_t);		//waiting also call  RealTime() so only wait founction in loop()
extern void RealTime(void);	
//=========================================================================
bool Check_TSMf(TimeFlag);
//-------------------------------------
LED_Status LaiBlink(TimeFlag);
//-------------------------------------
LED_Status LaiPWM(uint8_t);
//-------------------------------------
LED_Status LaiBlinkTimes(uint8_t);//the range is 1-7
//=========================================================================
#define LaiSch(x) if(Check_TSMf(x))
//=========================================================================
#endif
