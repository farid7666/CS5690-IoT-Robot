#include <LiquidCrystal_PCF8574.h>
#include <stdlib.h>
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display
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
  delay(1000);
}
