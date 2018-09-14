// Arduino code is available to download - link below the video.

// UV index explanation: http://www2.epa.gov/sunwise/uv-index-scale
// UV index explanation: http://www.epa.gov/sunwise/doc/what_is_uvindex.html

/* 
Connection:

 Arduino           Ultraviolet (UV) sensor
 A0 (Analog 0)          SIG
 +5V                    VCC
 GND                    GND
*/
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
float Vsig;

void setup(){
  lcd.setBacklight(255);
  lcd.begin(16, 2);
  lcd.print("circuitdigest.com");
  delay(100);
  
  pinMode(A0,INPUT);
  lcd.setCursor(0,1);
  lcd.print("Connecting...");
  Serial.begin(115200);
  esp8266.begin(115200);
  Serial.println("AT");
  esp8266.println("AT");
  delay(10000);
  if(esp8266.find("OK")){
    connectWiFi();
  }
}
 
void loop()
{  
  int sensorValue;
  long  sum=0;
  for(int i=0;i<1024;i++)
   {  
      sensorValue=analogRead(A0);
      sum=sensorValue+sum;
      delay(2);
   }   
  sum = sum >> 10;
  Vsig = sum*4980.0/1023.0; // Vsig is the value of voltage measured from the SIG pin of the Grove interface
  Serial.print("The voltage value: ");
  Serial.print(Vsig);
  Serial.print(" mV    --   ");

  lcd.clear();
  lcd.display();
  lcd.setCursor(0, 0);
  if (Vsig < 50) {
    Serial.print("UV Index: 0 "); Serial.println("   Exposure level - NONE (You're probably at home!) ");
    lcd.print("UV Index: 0");
  }
  if (Vsig > 50 && Vsig < 227) {
    Serial.print("UV Index: 1 "); Serial.println("   Exposure level - LOW (You're probably at home!) ");
    lcd.print("UV Index: 1");
  }
  if (Vsig > 227 && Vsig < 318) {
    Serial.print("UV Index: 2 "); Serial.println("   Exposure level - LOW (You can go outside and have fun!) ");
    lcd.print("UV Index: 2");
  }
  if (Vsig > 318 && Vsig < 408) {
    Serial.print("UV Index: 3 "); Serial.println("   Exposure level - MODERATE (Sun starts to annoy you) ");
    lcd.print("UV Index: 3");
  }
  if (Vsig > 408 && Vsig < 503) {
    Serial.print("UV Index: 4 "); Serial.println("   Exposure level - MODERATE (Sun starts to annoy you) ");
    lcd.print("UV Index: 4");
  }
  if (Vsig > 503 && Vsig < 606) {
    Serial.print("UV Index: 5 "); Serial.println("   Exposure level - MODERATE (Sun starts to annoy you) ");
    lcd.print("UV Index: 5");
  }
  if (Vsig > 606 && Vsig < 696) {
    Serial.print("UV Index: 6 "); Serial.println("   Exposure level - HIGH (Get out from the sunlight! get out now!) ");
    lcd.print("UV Index: 6");
  }
  if (Vsig > 696 && Vsig < 795) {
    Serial.print("UV Index: 7 "); Serial.println("   Exposure level - HIGH (Get out from the sunlight! get out now!) ");
    lcd.print("UV Index: 7");
  }
  if (Vsig > 795 && Vsig < 881) {
    Serial.print("UV Index: 8 "); Serial.println("   Exposure level - VERY HIGH (Get out from the sunlight! get out now!) ");
    lcd.print("UV Index: 8");
  }
  if (Vsig > 881 && Vsig < 976) {
    Serial.print("UV Index: 9 "); Serial.println("   Exposure level - VERY HIGH (If you value your health, don't go outside, just stay at home!) ");
    lcd.print("UV Index: 9");
  }
  if (Vsig > 976 && Vsig < 1079) {
    Serial.print("UV Index: 10 "); Serial.println("   Exposure level - VERY HIGH (If you value your health, don't go outside, just stay at home!) ");
    lcd.print("UV Index: 10");
  }
  if (Vsig > 1079 && Vsig < 1170) {
    Serial.print("UV Index: 11 "); Serial.println("   Exposure level - EXTREME (If you value your health, don't go outside, just stay at home!) ");
    lcd.print("UV Index: 11");
  }
  if (Vsig > 1170) {
    Serial.print("UV Index: 11+ "); Serial.println("   Exposure level - EXTREME (You will probably die in 3, 2, 1... Just JOKING, don't be scared...) - intensity of sunlight is really at maximum ");
    lcd.print("UV Index: 11");
  }
  delay (100);
  lcd.setCursor(0, 1); // set the cursor to column 0, line 2
  //start: //label 
  //error=0;
  delay(1000);
  updatebeat(Vsig);
  //Resend if transmission is not completed 
  //if (error==1){
  //  goto start; //go to label "start"
  //}
  delay(1000);
}

void updatebeat(float X){
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
  delay(3000);
  if(esp8266.find("OK")){
    Serial.println("OK");
    return true;    
  }else{
    return false;
  }
}
