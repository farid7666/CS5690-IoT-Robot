#include <SoftwareSerial.h>
#define DEBUG true
SoftwareSerial esp8266(9,10); 
#include <LiquidCrystal_PCF8574.h>
#include <stdlib.h>
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define SSID "OnePlus 5T"     // "SSID-WiFiname" 
#define PASS "buet177buet136"       // "password"
//#define IP "192.168.43.198"
#define IP "184.106.153.149"// thingspeak.com ip
String msg = "GET /update?key=S8NGVSFAE3O9FVAJ"; //change it with your api key like "GET /update?key=Your Api Key"
//String msg = "POST /test";

int error;
int light;
char val;

void setup()
{
  //lcd.setBacklight(255);
  //lcd.begin(16, 2);
  //lcd.print("circuitdigest.com");
  //delay(100);
  //lcd.setCursor(0,1);
  //lcd.print("Connecting...");
  Serial.begin(9600);
  pinMode(A0,INPUT);
  pinMode(18,OUTPUT);
  pinMode(15,OUTPUT);
  esp8266.begin(115200);
  Serial.println("AT");
  esp8266.println("AT");
  delay(6000);
  if(esp8266.find("OK")){
    connectWiFi();
  }
}

void loop()
{
  while (Serial.available() > 0)
  {
  val = Serial.read();
  }
  if( val == '1') // Forward
    {
      digitalWrite(15, HIGH);
      light=analogRead(A0);
      Serial.println("Light = " + String(light));
      if (light<200) {
        tone(18,1000,2000);
      }
      start: //label 
      error=0;
      delay(1000);
      updatebeat(light);
   //Resend if transmission is not completed 
      if (error==1){
        goto start; //go to label "start"
      }
      delay(1000);
      }
  else if(val == '2') // Backward
    {
 
      digitalWrite(15, LOW);
      Serial.println("Data Sending Stopped");
      delay(1000);
    }
}

void updatebeat(int X){
  error=0;
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  esp8266.println(cmd);
  delay(2000);
  if(esp8266.find("Error")){
    return;
  }
  cmd = msg ;
  cmd += "&field1=";   
  cmd += light;
  cmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  esp8266.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  esp8266.println(cmd.length());
  if(esp8266.find(">")){
    Serial.print(cmd);
    esp8266.print(cmd);
  }
  else{
   Serial.println("AT+CIPCLOSE");
   esp8266.println("AT+CIPCLOSE");
    //Resend...
    error=1;
  }
}

boolean connectWiFi(){
  Serial.println("AT+CWMODE=1");
  esp8266.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  esp8266.println(cmd);
  delay(5000);
  if(esp8266.find("OK")){
    Serial.println("OK");
    return true;    
  }else{
    return false;
  }
}
