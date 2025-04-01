void setup() {
  Serial.begin(115200);
  pinMode(3, OUTPUT); // VELOCITAT
  pinMode(10, OUTPUT); // DIRECCIÓ
  pinMode(11, OUTPUT);  //DIRECCIÓ

  TCCR3A = ( TCCR3A & B11111000 | B00000001 );    // TIMER MAIN MOTOR
  TCCR3B = ( TCCR3B & B11111000 | B00000010 );

  EICRB |= B00000011;   // RISING EDGE INTERRUPT
  EIMSK |= B00010000;   // ENABLE INT4 PIN 2 INTERRUPT

  digitalWrite(3, LOW); // check
}

ISR(INT4_COMPA_vect){                           // pins 2 3 18 19 20 21   CHECK
  Clogging();
}

void loop() {
  digitalWrite(10, LOW); // DIRECTION
  digitalWrite(11, HIGH); // DIRECTION
  analogWrite(3, 250); // PWM 255
  
  //current_sensor = analogRead(pin);       // alternative
  //if(current sensor < XXXX){
    //analogWrite(3,100);
    //digitalWrite(10, HIGH); // SWAP DIRECTIONS
    //digitalWrite(11, LOW);
    //delay(3000);
  //}
}

int Clogging(){         // testing!!!!
  analogWrite(3,100);
  digitalWrite(10, HIGH); // SWAP DIRECTIONS
  digitalWrite(11, LOW);
  delay(3000);
}