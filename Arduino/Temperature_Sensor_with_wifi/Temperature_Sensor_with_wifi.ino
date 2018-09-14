/* How to use the DHT-22 sensor with Arduino uno
   Temperature and humidity sensor
*/

//Libraries
#include <DHT.h>
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
//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino


//Variables
int error;
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value

void setup()
{
  lcd.setBacklight(255);
  lcd.begin(16, 2);
  lcd.print("circuitdigest.com");
  delay(100);
  lcd.setCursor(0,1);
  lcd.print("Connecting...");
  Serial.begin(115200);
  dht.begin();
  esp8266.begin(115200);
  Serial.println("AT");
  esp8266.println("AT");
  delay(5000);
  if(esp8266.find("OK")){
    connectWiFi();
  }
}

void loop()
{
    delay(2000);
    //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp= dht.readTemperature();
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Humidity = ");
    lcd.print(hum);
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    start1: //label 
    error=0;
    delay(1000);
    updatebeat(hum, false);
    //Resend if transmission is not completed 
    if (error==1){
      goto start1; //go to label "start"
    }
    delay(1000);
    
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Temperature = ");
    lcd.print(temp);
    lcd.print(" Celsius");
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    start2: //label 
    error=0;
    delay(1000);
    updatebeat(temp, true);
    //Resend if transmission is not completed 
    if (error==1){
      goto start2; //go to label "start"
    }
    delay(1000);
}

void updatebeat(float X, bool temp){
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
  if (temp){
  cmd += "&field3=";
  }
  else{
    cmd+= "&field2=";  
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

   
