/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2013-2024 Lai Yin-Chuan. All rights reserved.                                              */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include "LaiSch.h"

#if defined(ARDUINO_HT32_USB)
#include"BMTimer.h"
BMTimer timer;
#endif

uint32_t xTSMf,xTSMt,xTSMp,xWaitTick;
uint8_t xLaiTick;
//uint8_t flash_tmp=244;
//=======================================
void LaiSchCore()
{
  xTSMp = xTSMt++;
  xTSMf |= xTSMt ^ xTSMp;
  if(xWaitTick)xWaitTick--;				//every tick is 61us
  xLaiTick=xTSMt%64;				
}
//=======================================
bool Check_TSMf(TimeFlag x)
{
  if(xTSMf&((uint32_t)0x1<<x))
    {
      xTSMf &= ~((uint32_t)0x1<<x);
      return true;
    }
    return false;
}
//=====================================
LED_Status LaiBlink(TimeFlag x)
{
  	if(xTSMt&((uint32_t)0x1<<(x-1)))
    {
      return dark;
    }
    return light;
}
//=====================================
LED_Status LaiPWM(uint8_t PWM_Value )
{
	return	(xLaiTick>PWM_Value)?dark:light;
}
//=====================================
LED_Status LaiBlinkTimes(uint8_t flash)
{
    if((xTSMt & ((uint32_t)0x1 << _4Second)-1) > ((8-flash) * ((uint32_t)(0x1 << _2Hz))))
    {
      return ((xTSMt&((0x1<<_2Hz)-1))>(0x1<<_16Hz))?dark:light;
    }
    return dark;
}
//======================================================
#if defined(ESP32)
void ARDUINO_ISR_ATTR onTimer() 
{
  portENTER_CRITICAL_ISR(&timerMux);
  LaiSchCore();
  portEXIT_CRITICAL_ISR(&timerMux);
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
}
//#endif
//======================================================
#elif defined (ESP8266)
//#define FineTune_32768Hz  141
void ICACHE_RAM_ATTR onTimer1ISR()
{
  timer1_write(141);//32768Hz
  LaiSchCore();
}
#elif defined(ARDUINO_HT32_USB)
void callLaiSchCore()
{ 
  LaiSchCore();
  timer.update();
}
#else
ISR(TIMER2_COMPA_vect)
{  
  LaiSchCore();
}
#endif
//======================================================
void LaiSchWait(uint32_t x)
{
  xWaitTick=x;
  while(xWaitTick>0)RealTime();
}
//=======================================
void LaiSchCoreInit()
{
#if defined(ESP32)
  timerSemaphore = xSemaphoreCreateBinary();
  timer = timerBegin(2000000);
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer,61, true,0);//32768Hz
//  timerAlarmEnable(timer);  

#elif defined(ESP8266)
  timer1_attachInterrupt(onTimer1ISR);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(141);//32768Hz

#elif defined(ARDUINO_HT32_USB)

  timer.every(1000000,callLaiSchCore);

#else
  cli();//stop interrupts
//set timer2 interrupt at 32768Hz
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 8khz increments
  OCR2A = 60;//249;// = (16*10^6) / (8000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  sei();//allow interrupts
#endif
}
//=======================================
