/* How to use the DHT-22 sensor with Arduino uno
   Temperature and humidity sensor
*/

//Libraries
#include <DHT.h>
#include <LiquidCrystal_PCF8574.h>
#include <stdlib.h>
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino


//Variables
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
    delay(1000);
}

   
