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
int led = 8;
char val;
int flag =0;


void setup() {
  pinMode(led, OUTPUT);  // Digital pin 13 set as output Pin
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





void loop() {
  while (Serial.available() > 0)
  {
  val = Serial.read();
  Serial.println(val);
  }
  if( val == '1') // Forward
    {
     
      forward();
      delay(1000);
	  haltMotors;
    }
  else if(val == '2') // Backward
    {
 
      backward();
      delay(1000);
	  haltMotors;
    }
  else if(val == '3') //right
  {
	  turnRight();
	  delay(1000);
	  haltMotors;
  }
  else if(val == '4') //left
  {
	  turnLeft();
	  delay(1000);
	  haltMotors;
  }
}

