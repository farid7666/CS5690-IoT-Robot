#include <SoftwareSerial.h>
#define DEBUG true
SoftwareSerial esp8266(9,10); 
#include <LiquidCrystal_PCF8574.h>
#include <stdlib.h>
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define SSID "NUU A4L"     // "SSID-WiFiname" 
#define PASS "buet177buet136"       // "password"
#define IP "184.106.153.149"// thingspeak.com ip
String msg = "GET /update?key=S8NGVSFAE3O9FVAJ"; //change it with your api key like "GET /update?key=Your Api Key"

int error;
int light;

void setup()
{
  lcd.setBacklight(255);
  lcd.begin(16, 2);
  lcd.print("circuitdigest.com");
  delay(100);
  lcd.setCursor(0,1);
  lcd.print("Connecting...");
  Serial.begin(115200);
  pinMode(A0,INPUT);
  pinMode(5,OUTPUT);
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
  light=analogRead(A0);
  Serial.println("Light = " + String(light));
  lcd.clear();
  lcd.display();
  lcd.setCursor(0, 0);
  lcd.print("Light = ");
  lcd.print(String(light));
  delay (100);
  lcd.setCursor(0, 1); // set the cursor to column 0, line 2
  if (light > 300)
  {
    tone(9, 1000);
    delay(1000);
    noTone(9);
    delay(1000);
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
  cmd += "&field7=";   
  cmd += X;
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
