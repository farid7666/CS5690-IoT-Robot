//Libraries
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <LiquidCrystal_PCF8574.h>
#include <stdlib.h>
#include <SoftwareSerial.h>

//Setup LCD, BMP, ESP8266
LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Adafruit_BMP280 bmp; // I2C
SoftwareSerial esp8266(9,10); 

//Constants
#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define SSID "NUU A4L"     // "SSID-WiFiname" 
#define PASS "buet177buet136"       // "password"
#define IP "184.106.153.149"// thingspeak.com ip
String msg = "GET /update?key=S8NGVSFAE3O9FVAJ"; //change it with your api key like "GET /update?key=Your Api Key"
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino


//===============================================
//            START PREDECLARED VARIABLES
//===============================================

//========DHT VARIABLES
float hum;  //Stores humidity value
float temp; //Stores temperature value
int light;
int pin = 8; //RENAME
int error;


//========HEARTBEAT VARIABLES
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded! 
volatile boolean Pulse = false;     // "True" when heartbeat is detected. "False" when not a "live beat". 
volatile boolean QS = false;        // becomes true when Arduino finds a beat.

// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = true;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse 
volatile int rate[10];                    // array to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find IBI
volatile int P =512;                      // used to find peak in pulse wave, seeded
volatile int T = 512;                     // used to find trough in pulse wave, seeded
volatile int thresh = 525;                // used to find instant moment of heart beat, seeded
volatile int amp = 100;                   // used to hold amplitude of pulse waveform, seeded
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = false;      // used to seed rate array so we startup with reasonable BPM
int pulsePin = 2;                 // Pulse Sensor purple wire connected to analog pin 0

//========DUST SENSOR
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 2000; 
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

//========UV
float Vsig;

//========BUTTON VARIABLES
const int buttonPin1 = 2;     // the number of the pushbutton pin
bool flag; // on/off flag

//========LED PINS
const int greenLightPin = 12;
const int redLightPin = 13;


//===============================================
//            END PREDECLARED VARIABLES
//===============================================

void setup() {
  //Setup LCD (off to start)
  lcd.setBacklight(0);
  lcd.begin(16, 2);
  lcd.setCursor(0,1);

  //Setup serial
  Serial.begin(115200);

  //Set data pins and start dht and bmp
  dht.begin();
  pinMode(A0,INPUT);
  pinMode(pin,INPUT);
  pinMode(redLightPin,OUTPUT);
  pinMode(greenLightPin,OUTPUT);
  pinMode(buttonPin1, INPUT);
  //bmp needs to collect 30 seconds of data before reading, so start here
  starttime = millis(); 
    if (!bmp.begin()) {  
    //Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  //Setup interrupts for heartbeat sensor
  interruptSetup(); 
  //Set interrupt for on button (off button is connected to reset pin)
  attachInterrupt(digitalPinToInterrupt(2), onButton, CHANGE);

  //Start esp8266 wifi module and connect to wifi
  esp8266.begin(115200);
  //Serial.println("AT");
  esp8266.println("AT");
  if(esp8266.find("OK")){
    connectWiFi();
  }

  //On/Off Flag (Off by default)
  flag = false;
  

}

//===============================================
//            START SENSORS
//===============================================


void temp_and_hum_sensor() {
    //Read data and store it to variables hum
    hum = dht.readHumidity();
    
    //Print temp and humidity values to serial monitor
    //Serial.print("Humidity: ");
    //Serial.print(hum);
    
    //print to lcd
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Humidity = ");
    lcd.print(hum);
    delay (100);
    lcd.setCursor(0,1); // set the cursor to column 0, line 2
    sendData(hum, 1); //send humidity data
    delay(1000);

    //Warn for hum
    if (hum>60) {
        digitalWrite(redLightPin,HIGH);
        digitalWrite(greenLightPin,LOW);
        lcd.clear();
        lcd.display();
        lcd.setCursor(0, 0);
        lcd.print("Warning: Humidity");
        lcd.setCursor(0,1);
        lcd.print("is above 60%");
        delay(10000);
    }
    else {
        digitalWrite(redLightPin,LOW);
        digitalWrite(greenLightPin,HIGH);
    }

    //read temp
    temp= dht.readTemperature();

    //print to serial
    //Serial.print(" %, Temp: ");
    //Serial.print(temp);
    //Serial.println(" Celsius");

    //print to lcd
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Temperature = ");
    lcd.setCursor(0, 1);
    lcd.print(temp);
    lcd.print(" Celsius");
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    sendData(temp, 2); //send temperature data
    delay(1000);

    //Warn for temp
    if (temp>27) {
        digitalWrite(redLightPin,HIGH);
        digitalWrite(greenLightPin,LOW);
        lcd.clear();
        lcd.display();
        lcd.setCursor(0, 0);
        lcd.print("Warning: Temp");
        lcd.setCursor(0,1);
        lcd.print("is above 27*C");
        delay(10000);
    }
        else {
        digitalWrite(redLightPin,LOW);
        digitalWrite(greenLightPin,HIGH);
    }
}

void light_sensor() {
  //delay(2000);
  //read light
  light=analogRead(A0);

  //print light to serial
  //Serial.println("Light = " + String(light));

  //print light to LCD
  lcd.clear();
  lcd.display();
  lcd.setCursor(0, 0);
  lcd.print("Light = ");
  lcd.print(String(light));
  delay (100);
  lcd.setCursor(0, 1); // set the cursor to column 0, line 2
  sendData(light, 3); //send light data
  delay(1000);

  //Warn accordingly
  if (light<200) {
    digitalWrite(redLightPin,HIGH);
    digitalWrite(greenLightPin,LOW);
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Warning: light");
    lcd.setCursor(0,1);
    lcd.print("is low");
    delay(10000);
  }
  else {
    digitalWrite(redLightPin,LOW);
    digitalWrite(greenLightPin,HIGH);
  }  
}


void dust_sensor() {
  //delay(2000);
  //setup variables
  duration = pulseIn(pin, LOW); //duration since start
  lowpulseoccupancy = lowpulseoccupancy+duration;
  if ((millis()-starttime) >= sampletime_ms) //if the sample time = = 30s
  {
    //get concentration from sample
    ratio = lowpulseoccupancy/(sampletime_ms*10.0);  
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; 

    //print to serial
    //Serial.print("Concentration = ");
    //Serial.print(concentration);
    //Serial.println(" pcs/0.01cf");
    //print to lcd
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Concentration = ");
    lcd.setCursor(0, 1);
    lcd.print(concentration);
    lcd.print(" pcs/0.01cf");
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    sendData(concentration, 4); //send dust data
    delay(1000);
    lowpulseoccupancy = 0;
    starttime = millis();
  }

  //Warn for dust
  if (concentration>10000) {
    digitalWrite(redLightPin,HIGH);
    digitalWrite(greenLightPin,LOW);
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Warning: Dust");
    lcd.setCursor(0,1);
    lcd.print("Level is High");
    delay(10000);
  }
  else {
    digitalWrite(redLightPin,LOW);
    digitalWrite(greenLightPin,HIGH);
  }
}

void baro_sensor() {
  //delay(2000);
    //read pressure
    float pressure = bmp.readPressure();

    //print pressure to serial
    //Serial.print(F("Pressure = "));
    //Serial.print(pressure);
    //Serial.println(" Pa");

    //print pressure to lcd
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Pressure = ");
    lcd.print(pressure);
    lcd.print(" Pa");
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    sendData(pressure, 5); //send pressure data
    delay(1000);

    //Warn for pressure
    if (pressure>100000) {
        digitalWrite(redLightPin,HIGH);
        digitalWrite(greenLightPin,LOW);
        lcd.clear();
        lcd.display();
        lcd.setCursor(0, 0);
        lcd.print("Warning: Pressure");
        lcd.setCursor(0, 1);
        lcd.print("is high");
        delay(10000);
    }
    else {
        digitalWrite(redLightPin,LOW);
        digitalWrite(greenLightPin,HIGH);
    }

    //read altitude
    float altitude = bmp.readAltitude(1013.25);

    //print altitude to serial
    //Serial.print(F("Approx altitude = "));
    //Serial.print(altitude); // this should be adjusted to your local forcase
    //Serial.println(" m");

    //print altitude to lcd
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Approx altitude = ");
    lcd.setCursor(0, 1);
    lcd.print(altitude);
    lcd.print(" m");
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    sendData(altitude, 6); //send altitude data
    delay(1000);

    //Warn for altitude
    if (altitude>500) {
        digitalWrite(redLightPin,HIGH);
        digitalWrite(greenLightPin,LOW);
        lcd.clear();
        lcd.display();
        lcd.setCursor(0, 0);
        lcd.print("Warning: Altitude");
        lcd.setCursor(0, 1);
        lcd.print("is too high");
        delay(10000);
    }
    else {
        digitalWrite(redLightPin,LOW);
        digitalWrite(greenLightPin,HIGH);
    }
    
}

void uv_sensor() {
  //delay(2000);
    //setup variables
    int sensorValue;
    long sum=0;
    int UV_index;

    //take a a few readings of the voltage
    for(int i=0;i<1024;i++){  
        sensorValue=analogRead(A1);
        sum=sensorValue+sum;
        delay(2);
    }   

    //average out the readings for the voltage
    sum = sum >> 10;
    Vsig = sum*4980.0/1023.0; // Vsig is the value of voltage measured from the SIG pin of the Grove interface
    //print voltage to serial
    //Serial.print("The voltage value: ");
    //Serial.print(Vsig);
    //Serial.print(" mV    --   ");

    //assign the ranges of voltages a UV index number
    if (Vsig < 50)                  UV_index = 0;
    if (Vsig > 50 && Vsig < 227)    UV_index = 1;
    if (Vsig > 227 && Vsig < 318)   UV_index = 2;
    if (Vsig > 318 && Vsig < 408)   UV_index = 3;
    if (Vsig > 408 && Vsig < 503)   UV_index = 4;
    if (Vsig > 503 && Vsig < 606)   UV_index = 5;
    if (Vsig > 606 && Vsig < 696)   UV_index = 6;    
    if (Vsig > 696 && Vsig < 795)   UV_index = 7;
    if (Vsig > 795 && Vsig < 881)   UV_index = 8;
    if (Vsig > 881 && Vsig < 976)   UV_index = 9;
    if (Vsig > 976 && Vsig < 1079)  UV_index = 10;
    if (Vsig > 1079 && Vsig < 1170) UV_index = 11;
    if (Vsig > 1170)                UV_index = 12;

    //print the UV index to the LCD
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("UV Index: ");
    lcd.print(UV_index);
    delay (100);
    lcd.setCursor(0, 1); // set the cursor to column 0, line 2
    sendData(Vsig, 7);
    delay(1000);

    //Warn uv index
    if (Vsig>408) {
      digitalWrite(redLightPin,HIGH);
      digitalWrite(greenLightPin,LOW);
      lcd.clear();
      lcd.display();
      lcd.setCursor(0, 0);
      lcd.print("Warning: UV Index is above 4");
      delay(10000);
    }
    else {
      digitalWrite(redLightPin,LOW);
      digitalWrite(greenLightPin,HIGH);
    }
}

void bpm() {
  //delay(2000);
  //print bpm to the lcd
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BPM = ");
  lcd.print(BPM);
  delay (100);
  lcd.setCursor(0, 1); // set the cursor to column 0, line 2
  sendData(BPM, 8);
  delay(1000);

  //Warn accordingly
  if (BPM>200) {
    digitalWrite(redLightPin,HIGH);
    digitalWrite(greenLightPin,LOW);
    lcd.clear();
    lcd.display();
    lcd.setCursor(0, 0);
    lcd.print("Warning: BPM");
    lcd.setCursor(0, 1);
    lcd.print("is too high");
    delay(10000);
  }
  else {
    digitalWrite(redLightPin,LOW);
    digitalWrite(greenLightPin,HIGH);
  }
}


//===============================================
//            END SENSORS
//===============================================


//Button interrupt (pin2)
void onButton() {
  flag = true;
}

void loop() {
  if(flag){ //if on button has been pressed
    lcd.setBacklight(255);
    temp_and_hum_sensor();
    light_sensor();
    dust_sensor();
    baro_sensor();
    uv_sensor();
    bpm();
 }
}

void sendData(float val, int type){
  
  //build http request
  String AIOkey = "30ffc0735e414c7d897975b3dc5a9b47";
  String feed;
  //add the correct feed for whatever type of data we read
  switch(type){
    case 1: //humidity
      feed = "humidity";
      break;
    case 2: //temperature
      feed = "temperature";
      break;
    case 3: //light
      feed = "light";
      break;
    case 4: //dust
      feed = "dust";
      break;
    case 5: //pressure
      feed = "pressure";
      break;
    case 6: //altitude
      feed = "altitude";
      break;
    case 7: //vsig
      feed = "vsig";
      break;
    case 8: //BPM
      feed = "bpm";
  }
  String value = String(val);
  String data = String("{\"value\":") + "\"" + value + "\"}";
  String datalen = (String)(data.length());
  //finish building http request
  String cmd = "POST /api/feeds/" + feed + "/data?X-AIO-Key=" + AIOkey + " HTTP/1.0\n"
  +"Host: io.adafruit.com\n"
  +"Content-Type: application/json\n"
  +"Content-Length: " + datalen + "\n\n"
  + data;
  
  esp8266.print(cmd);
  
  //delay(1000);
}



//===============================================
//            DON'T TOUCH
//===============================================

void interruptSetup(){     
  TCCR2A = 0x02;     // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
  TCCR2B = 0x06;     // DON'T FORCE COMPARE, 256 PRESCALER 
  OCR2A = 0X7C;      // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
  TIMSK2 = 0x02;     // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
  sei();             // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED      
} 

ISR(TIMER2_COMPA_vect){                       // triggered when Timer2 counts to 124
  cli();                                      // disable interrupts while we do this
  Signal = analogRead(pulsePin);              // read the Pulse Sensor 
  sampleCounter += 2;                         // keep track of the time in mS
  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

    //  find the peak and trough of the pulse wave
  if(Signal < thresh && N > (IBI/5)*3){      // avoid dichrotic noise by waiting 3/5 of last IBI
    if (Signal < T){                         // T is the trough
      T = Signal;                            // keep track of lowest point in pulse wave 
    }
  }

  if(Signal > thresh && Signal > P){        // thresh condition helps avoid noise
    P = Signal;                             // P is the peak
  }                                         // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N > 250){                                   // avoid high frequency noise
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){        
      Pulse = true;                               // set the Pulse flag when there is a pulsedigitalWrite(blinkPin,HIGH);                // turn on pin 13 LED
      IBI = sampleCounter - lastBeatTime;         // time between beats in mS
      lastBeatTime = sampleCounter;               // keep track of time for next pulse

      if(secondBeat){                        // if this is the second beat
        secondBeat = false;                  // clear secondBeat flag
        for(int i=0; i<=9; i++){             // seed the running total to get a realistic BPM at startup
          rate[i] = IBI;                      
        }
      }

      if(firstBeat){                         // if it's the first time beat is found
        firstBeat = false;                   // clear firstBeat flag
        secondBeat = true;                   // set the second beat flag
        sei();                               // enable interrupts again
        return;                              // IBI value is unreliable so discard it
      }   
      word runningTotal = 0;                  // clear the runningTotal variable    

      for(int i=0; i<=8; i++){                // shift data in the rate array
        rate[i] = rate[i+1];                  // and drop the oldest IBI value 
        runningTotal += rate[i];              // add up the 9 oldest IBI values
      }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values 
      BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
      QS = true;                              // set Quantified Self flag 
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }                       
  }

  if (Signal < thresh && Pulse == true){   // when the values are going down, the beat is over
    Pulse = false;                         // reset the Pulse flag so we can do it again
    amp = P - T;                           // get amplitude of the pulse wave
    thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
    P = thresh;                            // reset these for next time
    T = thresh;
  }

  if (N > 2500){                           // if 2.5 seconds go by without a beat
    thresh = 512;                          // set thresh default
    P = 512;                               // set P default
    T = 512;                               // set T default
    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date        
    firstBeat = true;                      // set these to avoid noise
    secondBeat = false;                    // when we get the heartbeat back
  }

  sei();     
  // enable interrupts when youre done!
}// end isrvoid

boolean connectWiFi(){
  //Serial.println("AT+CWMODE=1");
  esp8266.println("AT+CWMODE=1");
  //delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  //Serial.println(cmd);
  esp8266.println(cmd);
  //delay(5000);
  if(esp8266.find("OK")){
    //Serial.println("OK");
    return true;    
  }else{
    return false;
  }
}
