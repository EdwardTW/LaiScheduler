#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>
//======================================================
#define Ver  "Ver 1.0"
const char * compiledAt = "Date: " __DATE__ " Time:" __TIME__;
//======================================================
#ifdef ESP32    //use NodeMCU-32S
hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
#endif
#include "LaiSch.h"
//======================================================
#if  defined(ESP32)
#define LED_BUILTIN 2
#endif
#if  defined(ESP8266)
#define LED_BUILTIN 2
#endif
#define LED_D34 25
#define LED_D35 27
//======================================================
uint16_t  deg,debugCount;
uint8_t PWM_SetValue;  //the LED lightness 1/64 step
uint8_t timer_minute,timer_hour,timer_second;
//======================================================
//const char *ssid = "Galaxy A8 Star043C";
//const char *password = "qzfu8501";

const char *ssid = "Guest";
const char *password = "0426872354";

#define ServerPort 8086
WebServer server(ServerPort);
DHT dht(26, DHT11);

//======================================================

void handleRoot() {
  char msg[1500];

  snprintf(msg, 1500,
           "<html>\
  <head>\
    <meta http-equiv='refresh' content='4'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>ESP32 DHT Server</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
    h2 { font-size: 3.0rem; }\
    p { font-size: 3.0rem; }\
    .units { font-size: 1.2rem; }\
    .dht-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\
    </style>\
  </head>\
  <body>\
      <h2>ESP32 DHT Server!</h2>\
      <p>\
        <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
        <span class='dht-labels'>Temperature</span>\
        <span>%.2f</span>\
        <sup class='units'>&deg;C</sup>\
      </p>\
      <p>\
        <i class='fas fa-tint' style='color:#00add6;'></i>\
        <span class='dht-labels'>Humidity</span>\
        <span>%.2f</span>\
        <sup class='units'>&percnt;</sup>\
      </p>\
  </body>\
</html>",
           readDHTTemperature(0), readDHTHumidity(0)
          );
  server.send(200, "text/html", msg);
}

float readDHTTemperature(bool p) {
  // Sensor readings may also be up to 2 seconds
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }
  else {
    if(p)
    {
      Serial.print("temperature:");
      Serial.print(t);
      Serial.print("°");
      Serial.println("C");
    }
    return t;
  }
}

float readDHTHumidity(bool p) {
  // Sensor readings may also be up to 2 seconds
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }
  else {
    if(p)
    {
      Serial.print("Humidity:");
      Serial.print(h);
      Serial.println("%");
    }
    return h;
  }
}

void RealTime(void)
{
   LaiSch(_32768Hz)
  {
    digitalWrite(LED_BUILTIN,LaiPWM(PWM_SetValue)); //PWM_Freq:32768/64=512 Hz
    debugCount++;
  }
  LaiSch(_256Hz)
  {
    deg++;
    if(deg>=360)deg=0;
    float x=sin(deg*(PI/180));
    PWM_SetValue=(uint8_t)(x*32+32);        //0~64 change
  }
  LaiSch(_64Hz)
  {
    digitalWrite(LED_D34,LaiBlink(_1Hz));
    digitalWrite(LED_D35,LaiBlinkTimes(3));
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
void setup(void) {

  Serial.begin(115200);
  dht.begin();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());
  Serial.print(':');
  Serial.println(ServerPort);

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode( LED_D34, OUTPUT);
  pinMode( LED_D35, OUTPUT);
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

void loop(void) 
{
  RealTime();
  LaiSch(_1Hz)
  {
    server.handleClient();
    SimpleRTC();
    Serial.printf("%02d:%02d:%02d   %d\r\n",timer_hour,timer_minute,timer_second,debugCount);
    debugCount=0;
  }
  LaiSch(_8Second)
  {
    readDHTTemperature(1);
    readDHTHumidity(1);
  }
  // delay(2);//allow the cpu to switch to other tasks
}
