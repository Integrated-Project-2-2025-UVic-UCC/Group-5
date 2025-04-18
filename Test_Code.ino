#include <Wire.h>
#include <Adafruit_INA219.h>

bool active = false;
int current_ina, i;

int p_dir_1 = 10; // DIRECTION 1
int p_dir_2 = 11; // DIRECTION 2
int p_spd = 3;    // PWM SPEED
int p_clg = 2;    // CLOGGING ANALOG PIN
int p_ppr = 21;   // PAPER DETECTION INTERRUPT
//int p_vtg = pin; // VOLTAGE CONTROL PIN
//int p_drc = pin;  // DOOR CLOSE PIN

Adafruit_INA219 ina219;

void setup() {
  Serial.begin(115200);

  ina219.begin();

  pinMode(p_clg, INPUT);  // CLOGGING PIN
  pinMode(p_ppr, INPUT);  // CLOGGING PIN
  pinMode(p_spd, OUTPUT); // VELOCITAT
  pinMode(p_dir_1, OUTPUT); // DIRECCIÓ
  pinMode(p_dir_2, OUTPUT);  //DIRECCIÓ

  TCCR3A = ( TCCR3A & B11111001 | B00000001 );    // TIMER MAIN MOTOR 
  TCCR3B = ( TCCR3B & B11111010 | B00000010 );

  TCCR1A = ( TCCR1A & B11111100 | B00000000 );    // TIMER MOTOR OFF (CTC)
  TCCR1B = ( TCCR1B & B11101000 | B00001000 );    // PRESCALER OFF
  OCR1A = 62500;

  EIMSK |= B00000001;   // ENABLE INT1 PIN 21 INTERRUPT
  EICRA |= B00000010;   // FALLING EDGE INTERRUPT

  digitalWrite(p_spd, LOW); // check
}

isr(INT0_vect){           // LACK OF PAPER DETECTION
  TCCR1B = ( TCCR1B & B11111101 | B00000101 );  // TIMER ON MAX PRESCALER
}
isr(TIMER1_COMPA_vect){   // 4 SECONDS OVERWORK
  TCCR1B = ( TCCR1B & B11111000 | B00000000 );  // TIMER OFF PRESCALER 0
  TCNT1 = 0;
  active = false;
}

void loop() {
  if(p_drc){      // DOOR MUST BE CLOSED
    if(p_ppr){
      active = true;
    }

    if(active){   // NORMAL WORKING STATUS
      digitalWrite(p_dir_1, LOW); // DIRECTION
      digitalWrite(p_dir_2, HIGH); // DIRECTION
      analogWrite(p_spd, 200); // PWM 200
    }

    current_ina = ina219.getCurrent_mA();
    Serial.println(current_ina);
    if(current_ina > 1000){
      digitalWrite(p_dir_1, LOW); // STOP
      digitalWrite(p_dir_2, LOW);
      delay(500);
      
      digitalWrite(p_dir_1, HIGH); // SWAP DIRECTIONS
      digitalWrite(p_dir_2, LOW);
      for(i=50;i<101;i++){
        analogWrite(p_spd, i);
        delay(10);
      }
      delay(1000);
      
      digitalWrite(p_dir_1, LOW);
      digitalWrite(p_dir_2, HIGH);
      analogWrite(p_spd,100);
      delay(500);
      for(i=100;i<201;i++){
        analogWrite(p_spd, i);
        delay(10);
      }
    }
  }
}




/*
Timer 0: Controls pins 13 and 4.      -8bit
Timer 1: Controls pins 12 and 11.
Timer 2: Controls pins 10 and 9.      -8bit
Timer 3: Controls pins 5, 3, and 2.
Timer 4: Controls pins 8, 7, and 6.
Timer 5: Controls pins 44, 45, and 46
*/