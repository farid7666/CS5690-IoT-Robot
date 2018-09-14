#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
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

Adafruit_BMP280 bmp; // I2C

int error;

void setup() {
  lcd.setBacklight(255);
  lcd.begin(16, 2);
  lcd.print("circuitdigest.com");
  delay(100);
  lcd.setCursor(0,1);
  lcd.print("Connecting...");
  Serial.begin(115200);
  Serial.println(F("BMP280 test"));
  
  if (!bmp.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
  esp8266.begin(115200);
  Serial.println("AT");
  esp8266.println("AT");
  delay(5000);
  if(esp8266.find("OK")){
    connectWiFi();
  }
}

void loop() {
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Temperature = ");
    lcd.print(bmp.readTemperature());
    lcd.print(" *C");
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    start1: //label 
    error=0;
    delay(1000);
    updatebeat(bmp.readTemperature(), "temp");
    //Resend if transmission is not completed 
    if (error==1){
      goto start1; //go to label "start"
    }
    delay(1000);
    
    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Pressure = ");
    lcd.print(bmp.readPressure());
    lcd.print(" Pa");
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    start2: //label 
    error=0;
    delay(1000);
    updatebeat(bmp.readPressure(), "pres");
    //Resend if transmission is not completed 
    if (error==1){
      goto start2; //go to label "start"
    }    
    delay(1000);
    
    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); // this should be adjusted to your local forcase
    Serial.println(" m");
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Alt = ");
    lcd.print(bmp.readAltitude(1013.25));
    lcd.print(" m");
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    start3: //label 
    error=0;
    delay(1000);
    updatebeat(bmp.readAltitude(1013.25), "alt");
    //Resend if transmission is not completed 
    if (error==1){
      goto start3; //go to label "start"
    }
    delay(1000);
    
    Serial.println();
    delay(2000);
}

void updatebeat(float X, String type){
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
  if (type == "temp"){
  cmd += "&field3=";   
  }
  if (type == "pres"){
    cmd += "&field4=";
  }
  if (type == "alt"){
    cmd += "&field5=";
  }
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
