#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

bool clogged;
int current_ina, i;

void setup() {
  Serial.begin(115200);

  ina219.begin();

  pinMode(2, INPUT);  // CLOGGING PIN

  pinMode(3, OUTPUT); // VELOCITAT
  pinMode(10, OUTPUT); // DIRECCIÓ
  pinMode(11, OUTPUT);  //DIRECCIÓ

  TCCR3A = ( TCCR3A & B11111000 | B00000001 );    // TIMER MAIN MOTOR
  TCCR3B = ( TCCR3B & B11111000 | B00000010 );

  //EICRB |= B00000011;   // RISING EDGE INTERRUPT
  //EIMSK |= B00010000;   // ENABLE INT4 PIN 2 INTERRUPT

  digitalWrite(3, LOW); // check
}

//ISR(INT4_vect){                           // pins 2 3 18 19 20 21   CHECK
  //Clogging();
//}

void loop() {
  digitalWrite(10, LOW); // DIRECTION // STANDARD WORKING STATUS
  digitalWrite(11, HIGH); // DIRECTION
  analogWrite(3, 200); // PWM 200

  current_ina = ina219.getCurrent_mA();
  Serial.println(current_ina);
  
  //if(current_ina > 1000){
    //TCCR3B = ( TCCR3B & B11111000 | B00000000 );
  //}

  clogged = digitalRead(2);
  if(current_ina > 1000){
  //if(clogged){
    digitalWrite(10, LOW); // STOP
    digitalWrite(11, LOW);
    delay(500);
    analogWrite(3,100);
    digitalWrite(10, HIGH); // SWAP DIRECTIONS
    digitalWrite(11, LOW);
    delay(1000);
    digitalWrite(10, LOW);
    digitalWrite(11, HIGH);
    analogWrite(3,100);
    delay(500);
    for(i=100;i<201;i++){
      analogWrite(3, i);
      delay(10);
    }
  }
}