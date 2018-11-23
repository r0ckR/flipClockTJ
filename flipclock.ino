
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUDP.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#include "A4988.h"

#define MOTOR_ACCEL 2000
#define MOTOR_DECEL 1000

#define RPM 80
#define MICROSTEPS 8

ESP8266WebServer server(80);

// using a 200-step motor (most common)
#define MOTOR_STEPS 48
// configure the pins connected
#define DIR 8
#define STEP 9
#define MS1 10
#define MS2 11
#define MS3 12
A4988 stepper(MOTOR_STEPS, 5, 12, 14);

typedef struct _fcTime {
  uint8_t hour;
  uint8_t minute;
} fcTime;

fcTime flipTime;

// Replace with your network credentials
#define SSID            "Hungriger Kater"
#define SSIDPWD         "3ApfelKuchenfuer1Kater"

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;


const String HTTP_HEAD           = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>{bedLight}</title>";
const String HTTP_STYLE  = "<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;}  input{width:90%;}  body{text-align: center;font-family:verdana;background-color:#262626; color: white;} button{border:0;border-radius:0.6rem;background-color:#1fb3ec;color:#fdd;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;} .button2 {background-color: #008CBA;} .button3 {background-color: #f44336;} .button4 {background-color: #515151; color: black;} .button5 {background-color: #555555;} .button6 {background-color: #d1b183;} .checkmark {height: 25px;width: 25px ;background-color: #eee; vertical-align: middle;}</style>";
const String HTTP_SCRIPT        = "<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>";
const String HTTP_HEAD_END      = "</head><body><div style='text-align:left;display:inline-block;min-width:260px;'>";

const String HOMEPAGE = "<form action=\"/cmd1\" method=\"get\"><button class=\"button3\">Red</button></form><br/><form action=\"/cmd2\" method=\"get\"><button class=\"button6\">Bright Warmwhite</button></form><br/> <form action=\"/cmd3\" method=\"get\"><button class=\"button2\">Blue</button></form><br/><form action=\"/cmd4\" method=\"get\"><button class=\"button4\">Off</button><br></form><br/>    ";

char htmlResponse[3000];

void handleRoot2() {

  snprintf ( htmlResponse, 3000,
"<!DOCTYPE html>\
<html lang=\"en\">\
  <head>\
    <meta charset=\"utf-8\">\
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
  </head>\
  <body>\
          <h1>Whats the clock displaying?:</h1>\
          <input type='text' name='date_hh' id='date_hh' size=2 autofocus> hh \
          <input type='text' name='date_mm' id='date_mm' size=2 autofocus> mm \
          <input type='text' name='date_ss' id='date_ss' size=2 autofocus> ss \
          <div>\
          <br><button id=\"save_button\">Lets make it roll!</button>\
          </div>\
    <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js\"></script>\    
    <script>\
      var hh;\
      var mm;\
      var ss;\
      $('#save_button').click(function(e){\
        e.preventDefault();\
        hh = $('#date_hh').val();\
        mm = $('#date_mm').val();\
        ss = $('#date_ss').val();\        
        $.get('/save?hh=' + hh + '&mm=' + mm + '&ss=' + ss, function(data){\
          console.log(data);\
        });\
      });\      
    </script>\
  </body>\
</html>"); 

   server.send ( 200, "text/html", htmlResponse );  

}

void setTime (int h, int m, int s) {
  flipTime.hour = h;
  flipTime.minute = m;
}


void handleSave() {
  if (server.arg("hh")!= ""){
   // Serial.println("Hours: " + server.arg("hh"));
  }

  if (server.arg("mm")!= ""){
    //Serial.println("Minutes: " + server.arg("mm"));
  }

  if (server.arg("ss")!= ""){
   // Serial.println("Seconds: " + server.arg("ss"));
  }

  Serial.println("Time: " + server.arg("hh")+":" + server.arg("mm")+":" + server.arg("ss"));
  setTime(server.arg("hh").toInt(),server.arg("mm").toInt(),server.arg("ss").toInt());
}

void handleRoot() {
 String s =HTTP_HEAD;
       s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s += "<H3>bedLight Control Panel</H3>";
      s+=HOMEPAGE;
  server.send(200, "text/html", s);
 
}

void cmd1() {
 String s =HTTP_HEAD;
       s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s += "<H3>bedLight Control Panel</H3>";
      s+=HOMEPAGE;
  server.send(200, "text/html", s);

}
void cmd2() {
 String s =HTTP_HEAD;
       s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s += "<H3>bedLight Control Panel</H3>";
      s+=HOMEPAGE;
  server.send(200, "text/html", s);

}
void cmd3() {
 String s =HTTP_HEAD;
       s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s += "<H3>bedLight Control Panel</H3>";
      s+=HOMEPAGE;
  server.send(200, "text/html", s);

}
void cmd4() {
 String s =HTTP_HEAD;
       s += HTTP_STYLE;
      s += HTTP_SCRIPT;  
      s += HTTP_HEAD_END;
      s += "<H3>bedLight Control Panel</H3>";
      s+=HOMEPAGE;
  server.send(200, "text/html", s);
  

     
}



void setup() {
  
  Serial.begin(115200);
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, SSIDPWD);
  // don't wait, observe time changing when ntp timestamp is received

  stepper.begin(RPM, MICROSTEPS);
    stepper.enable();

   // stepper.setSpeedProfile(stepper.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL);

  
  Serial.println(WiFi.localIP());

// Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600);
  timeClient.setUpdateInterval(3600000);

   if (!MDNS.begin("flipclock")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  // Start TCP (HTTP) server
  server.on("/", handleRoot2);
  server.on("/cmd1", cmd1);
  server.on("/cmd2", cmd2);
  server.on("/cmd3", cmd3);
  server.on("/cmd4", cmd4);
  server.on ("/save", handleSave);
  server.begin();
  Serial.println("TCP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  


 timeClient.update();

}

int16_t timeRegler(uint8_t sollH, uint8_t sollM, uint8_t istH, uint8_t istM) {
   float gesamtSoll = sollH*60+sollM;
   float gesamtIst = istH*60+istM;

   float diff = 6*(gesamtSoll-gesamtIst);

   if (diff<0) 
   {
    gesamtSoll += 24*60;
    diff = 6*(gesamtSoll-gesamtIst);
   }
   
   return diff;
}

bool first = false;

void loop() {
  server.handleClient();
  timeClient.update();
 
  // 2018-05-28T16:00:13Z
  formattedDate = timeClient.getFormattedDate();
  int splitT = formattedDate.indexOf("T");
 // dayStamp = formattedDate.substring(0, splitT);
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);

  uint8_t sollHour = timeStamp.substring(0,2).toInt();
  uint8_t sollMinute = timeStamp.substring(3,5).toInt();

  if (first == false) {
    flipTime.hour = sollHour;
    if (sollMinute > 0)
    flipTime.minute = sollMinute-1;
    else {
      flipTime.minute = 59;
      flipTime.hour -=1;
    }
    first = true;
  }

  //Serial.println(sollHour);
  //Serial.println(sollMinute);
  
  int16_t speedval = timeRegler(sollHour, sollMinute, flipTime.hour, flipTime.minute);
  Serial.print("speed="); Serial.println(speedval);
  if (speedval >5)
  {
     stepper.enable();
    //6 speedval entspricht einer minute
    // if (speedval >50) speedval = 50; // overflow protextion
    speedval = 6;
    //speedval -= speedval%6;
    uint16_t minuteSpeed = speedval/6;//1;
    if (flipTime.minute+minuteSpeed<=59) flipTime.minute+=minuteSpeed; else {
       flipTime.minute=flipTime.minute+minuteSpeed-60;
       flipTime.hour++;
       if (flipTime.hour>23) flipTime.hour = 0;
    }

    if (((flipTime.minute+2) % 5) == 0) speedval+=1;
    stepper.rotate(speedval);     
    
  } else { delay(2); stepper.disable(); }

}
