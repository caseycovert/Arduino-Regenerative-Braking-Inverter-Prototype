//LIBRARIES
#include <DHT.h>
#include <LiquidCrystal.h>

//PINS
const int Pot = A0;
const int Temp_Sensor = 10;
const int rs = 12;
const int en = 11;
const int d4 = 4;
const int d5 = 5;
const int d6 = 6;
const int d7 = 7;
const int button = 13;
const int A_forward = 2;
const int A_backward = 3;
const int B_forward = 8;
const int B_backward = 9;

//VARS
const int deadTime = 2;
bool motorOn = false;
bool lastButtonState = HIGH;
const int stepsPerRevolution = 200;

//DEFINING INTERFACES
DHT dht(Temp_Sensor, DHT11);
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  pinMode(A_forward, OUTPUT);
  pinMode(A_backward, OUTPUT);
  pinMode(B_forward, OUTPUT);
  pinMode(B_backward, OUTPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  dht.begin();
}

//FUNCTION TO OPEN ALL TRANSITORS 
void off()
{
  digitalWrite(A_forward, LOW);
  digitalWrite(A_backward, LOW);
  digitalWrite(B_forward, LOW);
  digitalWrite(B_backward, LOW);
}

//FUNCTION A+ PHASE
void phaseA_forward()
{
  digitalWrite(A_forward, HIGH);
  digitalWrite(A_backward, LOW);
}

//FUNCTION A- PHASE
void phaseA_reverse()
{
  digitalWrite(A_forward, LOW);
  digitalWrite(A_backward, HIGH);
}

//FUNCTION B+ PHASE
void phaseB_forward()
{
  digitalWrite(B_forward, HIGH);
  digitalWrite(B_backward, LOW);
}

//FUNCTION B- PHASE
void phaseB_reverse()
{
  digitalWrite(B_forward, LOW);
  digitalWrite(B_backward, HIGH);
}

//FUNCTION TAKES IN TEMP AND POT VAL IF MOTOR GETS TOO HOT IS FORCES IT TO RUN SLOWER
int safetyOverride(float tempF, int potValue)
{
  if (tempF < 80)
  {
    return potValue;
  }
  else if (tempF < 90)
  {
    return min(potValue, 512);
  }
  else
  {
    return 0;
  }
}

void loop() {
//CALCULATE TEMP IN F AND DISPLAY TO SECOND LINE OF LCD
  float tempF = dht.readTemperature()*9.0/5.0 + 32.0;
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(tempF);
  lcd.print(" F   ");


  int potValue = analogRead(Pot);
  int buttonState = digitalRead(button);
  int safePotValue = safetyOverride(tempF, potValue);

//TURNS ON AND OFF MOTOR USING BUTTON 
if (buttonState == LOW && lastButtonState == HIGH)
{
    motorOn = !motorOn;
    delay(50); // For Debouncing
}
lastButtonState = buttonState;
  
  //BUTTON OR GAS PEDAL OFF TURNS MOTOR OFF AND PRINTS ON LINE ONE OF LCD. THEN RETURNS TO TOP OF VOID LOOP
  if (safePotValue <= 130 || !motorOn) {
  off();   
  lcd.setCursor(0, 0);
  lcd.print("Motor Off   ");
    return;  
}

//RUNS ALL 4 PHASES. THE MORE GAS PEDAL IS ON THE LESS DELAY BETWEEN PHASES AND MOTOR RUNS FASTER
  int delayTime = map(safePotValue, 130, 1023, 200, 3);

  //A+
  phaseA_forward();
  delay(delayTime);
  off();
  delay(deadTime);

  //B+
  phaseB_forward();
  delay(delayTime);
  off();
  delay(deadTime);

  //A-
  phaseA_reverse();
  delay(delayTime);
  off();
  delay(deadTime);

  //B-
  phaseB_reverse();
  delay(delayTime);
  off();
  delay(deadTime);

//CALC RPM BY CALC TIME PER ROTATION AND 60/TIME PER ROTATION AND PRINTS TO LCD LINE 1
  float stepTimeSeconds = (delayTime+deadTime)/1000.0;
  float rpm = 60.0/(stepsPerRevolution*stepTimeSeconds);

    lcd.setCursor(0, 0);
    lcd.print("RPM:   ");
    lcd.print(rpm);
  }


