//Libraries
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <LiquidCrystal_PCF8574.h>
#include <stdlib.h>
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Adafruit_BMP280 bmp; // I2C

//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino


//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value
int light;
int pin = 8;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 2000; 
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
float Vsig;
//volatile byte onOffState = LOW;
const int buttonPin1 = 2;     // the number of the pushbutton pin
const int buttonPin2 = 3;


// variables will change:
volatile int buttonState1 = 0;         // variable for reading the pushbutton status
volatile int buttonState2 = 0;

bool flag= true;

void setup() {
  lcd.setBacklight(255);
  lcd.begin(16, 2);
  lcd.print("circuitdigest.com");
  delay(100);
  lcd.setCursor(0,1);
  lcd.print("Connecting...");
  Serial.begin(115200);
  dht.begin();
  pinMode(A0,INPUT);
  pinMode(8,INPUT);
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  // Attach an interrupt to the ISR vector
  attachInterrupt(digitalPinToInterrupt(2), pin_ISR1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), pin_ISR2, CHANGE);
  //interupt pin
  //const byte interruptPin = 2;
  //pinMode(2, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), onOff, CHANGE);
  //onOffState = true;
  //
  starttime = millis(); 
    if (!bmp.begin()) {  
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }
  

}
void light_sensor() {
  light=analogRead(A0);
  Serial.println("Light = " + String(light));
  lcd.clear();
  lcd.display();
  lcd.setCursor(0, 0);
  lcd.print("Light = ");
  lcd.print(String(light));
  delay (100);
  lcd.setCursor(0, 1); // set the cursor to column 0, line 2
  delay(1000);
  if (light<200) {
    digitalWrite(13,HIGH);
    digitalWrite(12,LOW);
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Warning: light");
    lcd.setCursor(0,1);
    lcd.print("is low");
    delay(10000);
}
    else {
        digitalWrite(13,LOW);
        digitalWrite(12,HIGH);
    }  
}


void temp_sensor() {
      //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Humidity = ");
    lcd.print(hum);
    delay (100);
    lcd.setCursor(0,1); // set the cursor to column 0, line 2
    delay(1000);
    if (hum>60) {
        digitalWrite(13,HIGH);
        digitalWrite(12,LOW);
        lcd.clear();
        lcd.display();
        lcd.setCursor(0, 0);
        lcd.print("Warning: Humidity");
        lcd.setCursor(0,1);
        lcd.print("is above 60%");
        delay(10000);
    }
    else {
        digitalWrite(13,LOW);
        digitalWrite(12,HIGH);
    }
    temp= dht.readTemperature();
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Temperature = ");
    lcd.setCursor(0, 1);
    lcd.print(temp);
    lcd.print(" Celsius");
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    delay(1000);
    if (temp>27) {
        digitalWrite(13,HIGH);
        digitalWrite(12,LOW);
        lcd.clear();
        lcd.display();
        lcd.setCursor(0, 0);
        lcd.print("Warning: Temp");
        lcd.setCursor(0,1);
        lcd.print("is above 27*C");
        delay(10000);
    }
        else {
        digitalWrite(13,LOW);
        digitalWrite(12,HIGH);
    }
}



void dust_sensor() {
    duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy+duration;
  if ((millis()-starttime) >= sampletime_ms) //if the sampel time = = 30s
  {
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; 
    Serial.print("Concentration = ");
    Serial.print(concentration);
    Serial.println(" pcs/0.01cf");
    Serial.println("\n");
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Concentration = ");
    lcd.setCursor(0, 1);
    lcd.print(concentration);
    lcd.print(" pcs/0.01cf");
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    delay(1000);
    lowpulseoccupancy = 0;
    starttime = millis();
  }
      if (concentration>10000) {
        digitalWrite(13,HIGH);
        digitalWrite(12,LOW);
        lcd.clear();
        lcd.display();
        lcd.setCursor(0, 0);
        lcd.print("Warning: Dust");
        lcd.setCursor(0,1);
        lcd.print("Level is High");
        delay(10000);
    }
        else {
        digitalWrite(13,LOW);
        digitalWrite(12,HIGH);
    }
}

void baro_sensor() {
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
    delay(1000);
        if (bmp.readTemperature()>27) {
        digitalWrite(13,HIGH);
        digitalWrite(12,LOW);
        lcd.clear();
        lcd.display();
        lcd.setCursor(0, 0);
        lcd.print("Warning: Temp");
        lcd.setCursor(0,1);
        lcd.print("is above 27*C");
        delay(10000);
    }
        else {
        digitalWrite(13,LOW);
        digitalWrite(12,HIGH);
    }
    
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
    delay(1000);
        if (bmp.readPressure()>100000) {
        digitalWrite(13,HIGH);
        digitalWrite(12,LOW);
        lcd.clear();
        lcd.display();
        lcd.setCursor(0, 0);
        lcd.print("Warning: Pressure");
        lcd.setCursor(0, 1);
        lcd.print("is high");
        delay(10000);
    }
        else {
        digitalWrite(13,LOW);
        digitalWrite(12,HIGH);
    }
    
    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); // this should be adjusted to your local forcase
    Serial.println(" m");
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Approx altitude = ");
    lcd.setCursor(0, 1);
    lcd.print(bmp.readAltitude(1013.25));
    lcd.print(" m");
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    delay(1000);
    if (bmp.readAltitude(1013.25)>250) {
        digitalWrite(13,HIGH);
        digitalWrite(12,LOW);
        lcd.clear();
        lcd.display();
        lcd.setCursor(0, 0);
        lcd.print("Warning: Altitude");
        lcd.setCursor(0, 1);
        lcd.print("is too high");
        delay(10000);
    }
        else {
        digitalWrite(13,LOW);
        digitalWrite(12,HIGH);
    }
    
    Serial.println();
    delay(2000);
}

void uv_sensor() {
  int sensorValue;
  long  sum=0;
  int UV_index;
  for(int i=0;i<1024;i++)
   {  
      sensorValue=analogRead(A1);
      sum=sensorValue+sum;
      delay(2);
   }   
 sum = sum >> 10;
 Vsig = sum*4980.0/1023.0; // Vsig is the value of voltage measured from the SIG pin of the Grove interface
 Serial.print("The voltage value: ");
 Serial.print(Vsig);
 Serial.print(" mV    --   ");
 
if (Vsig < 50) {
    //Serial.print("UV Index: 0 "); Serial.println("   Exposure level - NONE (You're probably at home!) ");
    UV_index = 0;
 }
if (Vsig > 50 && Vsig < 227) {
    //Serial.print("UV Index: 1 "); Serial.println("   Exposure level - LOW (You're probably at home!) ");
    UV_index = 1;
 }
if (Vsig > 227 && Vsig < 318) {
    //Serial.print("UV Index: 2 "); Serial.println("   Exposure level - LOW (You can go outside and have fun!) ");
    UV_index = 2;
 }
if (Vsig > 318 && Vsig < 408) {
    //Serial.print("UV Index: 3 "); Serial.println("   Exposure level - MODERATE (Sun starts to annoy you) ");
    UV_index = 3;
 }
if (Vsig > 408 && Vsig < 503) {
    //Serial.print("UV Index: 4 "); Serial.println("   Exposure level - MODERATE (Sun starts to annoy you) ");
    UV_index = 4;
 }
if (Vsig > 503 && Vsig < 606) {
    //Serial.print("UV Index: 5 "); Serial.println("   Exposure level - MODERATE (Sun starts to annoy you) ");
    UV_index = 5;
 }
if (Vsig > 606 && Vsig < 696) {
    //Serial.print("UV Index: 6 "); Serial.println("   Exposure level - HIGH (Get out from the sunlight! get out now!) ");
    UV_index = 6;
 }
if (Vsig > 696 && Vsig < 795) {
    //Serial.print("UV Index: 7 "); Serial.println("   Exposure level - HIGH (Get out from the sunlight! get out now!) ");
    UV_index = 7;
 }
if (Vsig > 795 && Vsig < 881) {
    //Serial.print("UV Index: 8 "); Serial.println("   Exposure level - VERY HIGH (Get out from the sunlight! get out now!) ");
    UV_index = 8;
 }
if (Vsig > 881 && Vsig < 976) {
    //Serial.print("UV Index: 9 "); Serial.println("   Exposure level - VERY HIGH (If you value your health, don't go outside, just stay at home!) ");
    UV_index = 9;
 }
if (Vsig > 976 && Vsig < 1079) {
    //Serial.print("UV Index: 10 "); Serial.println("   Exposure level - VERY HIGH (If you value your health, don't go outside, just stay at home!) ");
    UV_index = 10;
 }
if (Vsig > 1079 && Vsig < 1170) {
    //Serial.print("UV Index: 11 "); Serial.println("   Exposure level - EXTREME (If you value your health, don't go outside, just stay at home!) ");
    UV_index = 11;
 }
if (Vsig > 1170) {
    //Serial.print("UV Index: 11+ "); Serial.println("   Exposure level - EXTREME (You will probably die in 3, 2, 1... Just JOKING, don't be scared...) - intensity of sunlight is really at maximum ");
    UV_index = 12;
 }
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("UV Index: ");
    lcd.print(UV_index);
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    delay(1000);
     if (Vsig>408) {
        digitalWrite(13,HIGH);
        digitalWrite(12,LOW);
        lcd.clear();
        lcd.display();
        lcd.setCursor(0, 0);
        lcd.print("Warning: UV Index is above 4");
        delay(10000);
    }
        else {
        digitalWrite(13,LOW);
        digitalWrite(12,HIGH);
    }
}

void pin_ISR1() {
  flag = !flag;
  while (flag) {
   temp_sensor();
    light_sensor();
    dust_sensor();
    baro_sensor();
  }
}
void pin_ISR2() {
  flag = !flag;
  while (!flag) {
  
  }
}
//void onOff(){
//  onOffState=!onOffState;
//}

void loop() {
/*  int flag=0;
 if(onOffstate == LOW){
    flag = 1;
    while (flag == 1) {
      temp_sensor();
    light_sensor();
    dust_sensor();
    baro_sensor();
   // uv_sensor();
      }
 }*/
}
