// define pins
#include <PWM.h>

void setup() {
  Serial.begin(115200);

  InitTimersSafe(); 

  SetPinFrequencySafe(9,100); // MODIFICAR

  pinMode(9,OUTPUT); // VELOCITAT
  pinMode(10,OUTPUT); // DIRECCIÓ

  //TCCR1A = (TCCR1A | B00000001 & B11111101);  // TIMER 1 MODE 4 CTC OCR1A
  //TCCR1B = (TCCR1B | B00000000 & B11100111);
  //TCCR1B = (TCCR1B | B00000101 & B11111101);  // PRESCALER 1024 // POSAR A 0
  //TIMSK1 |= B00000010;                        // HABILITAR OCR1A

  //OCR1A = 62500;

//  EIMSK |= B00000010; // INT 1 INTERRUPT
//  EICRA |= B00001100; // FLANC DE PUJADA INTERRUPT
//
//  sei();
}

//ISR(TIMER1_COMPA_vect){   ////////////////  // TIMER 1 INTERRUPT //  ////////////////// 
//}

void loop() {
  pwmWrite(9,255);
  delay(2000);
  wmWrite(9,127);
  delay(2000);

}
