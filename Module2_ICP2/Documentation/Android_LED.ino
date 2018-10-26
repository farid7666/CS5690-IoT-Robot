// Starting of Program

int led = 8;
char val;
int flag =0;
void setup()
{

pinMode(led, OUTPUT);  // Digital pin 13 set as output Pin
Serial.begin(9600);
}

void loop()
{
  while (Serial.available() > 0)
  {
  val = Serial.read();
  Serial.println(val);
  }
  if( val == '1') // Forward
    {
     
      digitalWrite(led, HIGH);
      Serial.println("LED is ON");
      delay(1000);
    }
  else if(val == '2') // Backward
    {
 
      digitalWrite(led, LOW);
      Serial.println("LED is OFF");
      delay(1000);
    }

   
}

// End of program
