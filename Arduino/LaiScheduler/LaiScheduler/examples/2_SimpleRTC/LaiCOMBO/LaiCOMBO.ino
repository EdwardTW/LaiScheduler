#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <DHT.h>
#include "LaiSch.h"
//======================================================
#define Ver  "Ver 1.0"
const char * compiledAt = "Date: " __DATE__ " Time:" __TIME__;
//======================================================

#define LED_BUILTIN 2
//#define LEDD_Neg 
#define LEDD_Pos

//Work Mode define 
typedef enum {DebugMode=1,NormalMode,UpdateMode}Work_Mode;

#define WorkMode NormalMode

#define LocalName "esp32"
const char* host = LocalName;
//const char* ssid = "TP-Link_E630";
//const char* password = "0933199775";

//const char* ssid = "apquadlink";
//const char* password = "quadlink2015";

const char* ssid = "CHT-5-wifi";
const char* password = "0925257825";

#define MyServerIP "59.126.165.144"
#define MyServerPort 3990

#define ServerPort 8080
#define RemotePort 9062
WebServer server(ServerPort);
//#define ServerPort 8086
//WebServer server(ServerPort);
DHT dht(26, DHT11);
#define DHTServerPATH "/DHT"

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
#define LED_D34 25
#define LED_D35 27
//======================================================
uint16_t  deg,debugCount;
uint8_t PWM_SetValue;  //the LED lightness 1/64 step
uint8_t timer_minute,timer_hour,timer_second;
//======================================================
//===============================================================
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
//===============================================================
/*
 * Login page
 */

const char* loginIndex =
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
             "<td>Username:</td>"
             "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";
//=============================================
/*
 * Server Index Page
 */
const char* serverIndex =
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"          
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";
 
//=============================================

//TaskHandle_t Task1;   //宣告任務變數Task1

//TaskHandle_t Task2;   //宣告任務變數Task2

void RealTime(void)
{
  LaiSch(_32768Hz)
  {
    digitalWrite(LED_D35,LaiPWM(PWM_SetValue)); //PWM_Freq:32768/64=512 Hz
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
    digitalWrite(LED_BUILTIN, !LaiBlinkTimes(WorkMode));
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
 //============================================================
void  sendMyNAME(void)
{
const char* host = MyServerIP;
const uint16_t port = MyServerPort;
//char k[50];

  WiFiClient client;
//  sprintf(k,"%s@%s:%s,remote_port[OTA:%d]",LocalName,WiFi.localIP(),ServerPort,RemotePort);
  client.connect(host, port);
  if (client.connected())
  {
//     sprintf(k,"%s@%s:%s,remote_port[OTA:%d]",LocalName,WiFi.localIP(),ServerPort,RemotePort);
//     client.println(k);
    client.print(LocalName);
    client.print("@");
    client.print(WiFi.localIP());
    client.print(':');
    client.print(ServerPort);
    client.print(",remote_port[OTA:");
    client.print(RemotePort);
    client.print("]");
    client.print(",Ver[");
    client.print(compiledAt);
    client.println("]");
    Serial.println("sending data to server");
  }
  client.stop();  
}
//=============================================

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
//=============================================
/*
 * setup function
 */
void setup(void) {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode( LED_D34, OUTPUT);
  pinMode( LED_D35, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("");
  Serial.println(__FILE__);
  Serial.println(Ver);
  Serial.print("Compile ");
  Serial.println(compiledAt);
  Serial.println("Booting Sketch...");
//-------------------------------------
  Serial.print("WorkMode is ");
  Serial.println(WorkMode);
 //-------------------------------------  
  LaiSchCoreInit();//make 61us Interrupt(32768Hz)
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
//    delay(500);
    delay(1); 
    LaiSch(_2Hz)
    Serial.print(".");
    digitalWrite(LED_BUILTIN, LaiBlink(_4Hz));
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address  ");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.print(ServerPort);
  Serial.println(DHTServerPATH);
  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) 
    {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on(DHTServerPATH, handleRoot);
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  server.begin();                   
    Serial.print("Task Loop() running on core ");
    Serial.println(xPortGetCoreID());    //輸出執行此函式的核心編號
    delay(5000);
    sendMyNAME(); 
}
//=============================================
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
  LaiSch(_4096Second)sendMyNAME();
}
