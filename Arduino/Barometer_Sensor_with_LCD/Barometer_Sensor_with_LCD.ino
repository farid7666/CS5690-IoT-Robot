#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_PCF8574.h>
#include <stdlib.h>
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Adafruit_BMP280 bmp; // I2C

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
    delay(1000);
    
    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); // this should be adjusted to your local forcase
    Serial.println(" m");
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Approx altitude = ");
    lcd.print(bmp.readAltitude(1013.25));
    lcd.print(" m");
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    delay(1000);
    
    Serial.println();
    delay(2000);
}
