#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; // I2C

//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino


//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value
int light; //Stores light sensor value
int pin = 8;
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 2000; 
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;
float pressure;
float altitude;

void setup() {
  Serial.begin(9600);
  pinMode(A1,INPUT);
  pinMode(8,INPUT);
//starttime = millis();
  dht.begin();
  bmp.begin(); 
}


void loop() {
    delay(2000);
    //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp= dht.readTemperature();
    light=analogRead(A1);
    duration = pulseIn(pin, LOW);
    lowpulseoccupancy = lowpulseoccupancy+duration;
    if ((millis()-starttime) >= sampletime_ms) //if the sampel time = = 30s
    {
      ratio = lowpulseoccupancy/(sampletime_ms*10.0);  
      concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; 
      lowpulseoccupancy = 0;
      starttime = millis();
    }
    pressure = bmp.readPressure();
    altitude = bmp.readAltitude(1013.25);
    String str_Payload;

    str_Payload += String(temp);
    str_Payload += " " + String(hum);
    str_Payload += " " + String(light);
    str_Payload += " " + String(concentration);
    str_Payload += " " + String(pressure);
    str_Payload += " " + String(altitude);
    str_Payload += " ";
    
    byte Payload[str_Payload.length()];
    int LengthFrameAPI = 18 + sizeof(Payload);
    int LengthPayload = sizeof(Payload);
  
    Serial.println(str_Payload);

    delay(1000); //Delay 2 sec.
}
