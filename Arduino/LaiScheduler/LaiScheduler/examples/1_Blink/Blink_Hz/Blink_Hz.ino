
/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2013-2024 Lai Yin-Chuan. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/*
  BlinkLaiS coding by Lai Yin-Chyuan(Edward Lai) this code demo 
  Lai's Scheduler  part of function

  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)

  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
*/
/*//=========================================================================
typedef enum {_32768Hz,_16384Hz,_8192Hz,_4096Hz,_2048Hz,_1024Hz,_512Hz,_256Hz
,_128Hz,_64Hz,_32Hz,_16Hz,_8Hz,_4Hz,_2Hz,_1Hz,_2Second,_4Second,_8Second
,_16Second,_32Second,_64Second,_128Second,_256Second,_512Second,_1024Second
,_2048Second,_4096Second,_8192Second,_16384Second,_32768Second}TimeFlag;
//=========================================================================*/
//======================================================
#include "LaiSch.h"
#ifdef ESP32    //use NodeMCU-32S
hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
#endif

//======================================================
#if  defined(ESP32)
#define LED_BUILTIN 2
#endif
#if  defined(ESP8266)
#define LED_BUILTIN 2
#endif
//======================================================
void RealTime(void)
{
	digitalWrite(LED_BUILTIN,LaiBlink(_1Hz));
}
//======================================================
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  LaiSchCoreInit();//make 61us Interrupt(32768Hz) 
}
//======================================================
void loop()
{
	RealTime();
}
