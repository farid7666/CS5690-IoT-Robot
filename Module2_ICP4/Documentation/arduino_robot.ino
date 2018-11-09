/* This is the code for a Fire fighting Robot for smart home
 *  It can notify the tenant when it detects fire and put out the fire
*/
//importing libraires
#include<SoftwareSerial.h>
#include<AFMotor.h>


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


void setup() {
  Serial.begin(9600); //baudrate for Serial communication
  motor3.setSpeed(158); // set left DC motor speed 
  motor4.setSpeed(190); // set right DC motor speed
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
	  haltMotors();
   delay(1000);
    }
  else if(val == '3') // Backward
    {
 
      backward();
      delay(1000);
	  haltMotors();
   delay(1000);
    }
  else if(val == '4') //right
  {
	  turnRight();
	  delay(1000);
	  haltMotors();
   delay(1000);
  }
  else if(val == '2') //left
  {
	  turnLeft();
	  delay(1000);
	  haltMotors();
   delay(1000);
  }
  val = 0;
}

