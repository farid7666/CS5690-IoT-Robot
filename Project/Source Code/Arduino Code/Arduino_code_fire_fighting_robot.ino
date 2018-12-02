/* This is the code for a Fire fighting Robot for smart home
 *  It can notify the tenant when it detects fire and put out the fire
*/
//importing libraires
#include<SoftwareSerial.h>
#include <AFMotor.h>
#include <Servo.h>

//Initialize esp8266 Wifi module
SoftwareSerial esp8266(9,10);

//Credentials for wifi connection
#define SSID "*********"     // "SSID-WiFiname" 
#define PASS "*********"       // "password"
#define IP "184.106.153.149"// thingspeak.com ip
String msg = "GET /update?key=***********"; //change it with your api key like "GET /update?key=Your Api Key"

//Initialize Servo Motor
Servo myservo;  // create servo object to control a servo
Servo myservo2;

//Initialize DC motor
AF_DCMotor motor3(3, MOTOR12_1KHZ); 
AF_DCMotor motor4(4, MOTOR12_1KHZ); 

//Intialize global variables
int sensorPin = A0; // select the input pin for the LDR
int sensorValue = 1024;
int minimum;
int angle = 0;
int flag = 0;
int pos = 0;    // variable to store the servo position
int pos1 = 0;
int error;

void setup() {
  Serial.begin(115200); //baudrate for Serial communication
  esp8266.begin(115200); //baudrate for Esp8266 Wifi communication
  myservo.attach(17);  // attaches the servo on pin 17 to the servo object
  myservo2.attach(16); // attaches the servo on pin 16 to the servo object
  motor3.setSpeed(158); // set left DC motor speed 
  motor4.setSpeed(190); // set right DC motor speed
  pinMode(19,INPUT); // Declare pin 19 as input
  pinMode(18,OUTPUT); // Declare pin 18 as output
  //Chech wifi connection 
  if(esp8266.find("OK")){
    connectWiFi();
  }
   
}

void forward() {                     // This function moves all the wheels forward 
  motor3.run(FORWARD); 
  motor4.run(FORWARD); 
}

void backward()                    // This function moves all the wheels backwards 
{     
motor3.run(BACKWARD); 
motor4.run(BACKWARD); 
}

void haltMotors()                   // This function stops all the motors (It is better to stop the motors before changing direction) 
{ 
motor3.run(RELEASE); 
motor4.run(RELEASE);
} 

void turnLeft()                    // This function turns the robot left
{
motor3.run(FORWARD); 
motor4.run(BACKWARD); 
} 

void turnRight()                    // This function turns the robot right
{
motor3.run(BACKWARD); 
motor4.run(FORWARD); 
}

// Function for fire detection
void fire_detection() {
      haltMotors();
      for (pos = 0; pos <= 180; pos += 5) { // goes from 0 degrees to 180 degrees to detect the fire in steps of 5 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(100);                       // waits 100ms for the servo to reach the position
      minimum = analogRead(sensorPin);  // Read the flame sensor value
      //Saving the minimum value and position of servo motor where it gets minimum value
      if (minimum < sensorValue) { 
        sensorValue = minimum;
        angle = pos;
      }  
    }
    // Fire Detection
    if (sensorValue < 800) {
        tone(18,1000,2000); // as fire is detected turn on the buzzer
        // Uploading 5 value to thinkspeak to notify that there is a fire
        start: //label  
        error=0;
        delay(1000);
        updatebeat(5);
        if (error==1){
          goto start; //go to label "start"
        }
        myservo.write(angle); // set the servo to the angle for minimum value
        Serial.println(angle);
        flag = 1;
        // Moving the robot towards fire
          if (angle>90) {
            turnLeft();
            delay(7*(angle-90));
            haltMotors();
          }
          if (angle<90) {
            turnRight();
            delay(9*(90-angle));
            haltMotors();
          }
          // Moving the flame sensor towards fire
          myservo.write(90);
      }
}

// function for update data in think speak 
void updatebeat(int x) {
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
  cmd += "&field3=";   
  cmd += x;
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

// function for wifi connection check
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

void loop() {
  // Set the intial position for servo holding the water container
  myservo2.write(35);
  // Check if there is any fire
  if (flag == 0) {
    fire_detection();
  }
  delay(1000);
  // After detecting the fire move the robottowards the fire and check again
  // If robot detects the fire in certain distance it will pour out water 
  if (flag == 1) {
    if (digitalRead(19) == 1) {
      forward();
      delay(2000);
      haltMotors();
      delay(900);
      fire_detection();
      tone(18,1000,50); // make the buzzer sound
    }
    else {
      haltMotors();
      delay(1000);
        for (pos1 = 35; pos1 <= 120; pos1 += 1) { // goes from 35 degrees to 120 degrees
    myservo2.write(pos1);              // tell servo to go to position in variable 'pos'
    tone(18,1000,50);                     // make the buzzer sound
  }
  delay(1500);
  for (pos1 = 120; pos1 >=35; pos1 -= 1) { // goes from 120 degrees to 35 degrees
    myservo2.write(pos1);              // tell servo to go to position in variable 'pos1'
    
  }
  delay(1500);    
      flag = 2;
      // After put out the fire update value in thingspeak
        zeros: //label 
        error=0;
        delay(6000);
        updatebeat(0);
        if (error==1){
          goto zeros; //go to label "zeros"
        }
    }
  }
  delay(1000);
}

