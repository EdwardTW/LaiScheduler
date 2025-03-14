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
/*
//=========================================================================
typedef enum {_32768Hz,_16384Hz,_8192Hz,_4096Hz,_2048Hz,_1024Hz,_512Hz,_256Hz
,_128Hz,_64Hz,_32Hz,_16Hz,_8Hz,_4Hz,_2Hz,_1Hz,_2Second,_4Second,_8Second
,_16Second,_32Second,_64Second,_128Second,_256Second,_512Second,_1024Second
,_2048Second,_4096Second,_8192Second,_16384Second,_32768Second}TimeFlag;
//=========================================================================
#define LED_Control_Pin_In_Cathode true
#ifdef LED_Control_Pin_In_Cathode            //series with the resistor to the power terminal
typedef enum {light=0, dark=1}LED_Status;    //and the Cathode is connected to the MCU's I/O pin.
#else
typedef enum {light=1, dark=0}LED_Status;
#endif
//=========================================================================
*/
#define Ver	"Ver 1.0"
const char * compiledAt = "Date: " __DATE__ " Time:" __TIME__;
//======================================================
#include "LaiSch.h"
#ifdef ESP32
hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
#endif
//======================================================
#define LED_BUILTIN 2
#define LED_D12 12
#define LED_D13 13
//======================================================
uint8_t times=3;
uint16_t  deg;
uint16_t  deg_2;
uint8_t PWM_SetValue_1;  //mean the LED lightness only 1/64
uint8_t PWM_SetValue_2;  //mean the LED lightness only 1/64
uint8_t timer_minute,timer_hour,timer_second;
//======================================================
void RealTime(void)
{
  LaiSch(_32768Hz)
  {
    digitalWrite(LED_D13,LaiPWM(PWM_SetValue_1)); //PWM_Freq:32768/64=512 Hz
    digitalWrite(LED_D12,LaiPWM(PWM_SetValue_2)); //PWM_Freq:32768/64=512 Hz
  }
  LaiSch(_256Hz)
  {
    deg++;
    if(deg>=360)deg=0;
    float x=sin(deg*(PI/180));
    PWM_SetValue_1=(uint8_t)(x*32+32);        //0~64 change
    deg_2 += 2;
    if(deg_2>=360)deg_2=0;
    x=sin(deg_2*(PI/180));
    PWM_SetValue_2=(uint8_t)(x*32+32);        //0~64 change
  }
  LaiSch(_64Hz)
  {
//    digitalWrite(LED_D12,LaiBlink(_1Hz));
    digitalWrite(LED_BUILTIN,!LaiBlinkTimes(times));
  }
}
//======================================================
void SimpleRTC(void)
{
  timer_second++;
  if(timer_second>=60)
  {
     timer_second=0;
     timer_minute++;
     if(timer_minute>=60)
     {
        timer_minute=0;
        timer_hour++;
     }
  }
}
//======================================================
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode( LED_D13, OUTPUT);
  pinMode( LED_D12, OUTPUT);
 //-------------------------------------  
  Serial.begin(115200);
  Serial.println("");
  Serial.println(__FILE__);
  Serial.println(Ver);
  Serial.print("Compile ");
  Serial.println(compiledAt);
  Serial.println("Booting Sketch...");
//-------------------------------------- 
  LaiSchCoreInit();//make 61us Interrupt(32768Hz) 
}
//======================================================
void loop()
{
  RealTime();       //less than _1Hz call RealTime()
//  LaiSchWait(16); //61us*16≈1ms in wait also call RealTime();the wait function only in loop()
  LaiSch(_1Hz)
  {
    SimpleRTC();
    Serial.printf("%02d:%02d:%02d\r\n",timer_hour,timer_minute,timer_second);
  } 
}