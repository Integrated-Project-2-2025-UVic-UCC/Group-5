#include <Wire.h>
#include <Adafruit_INA219.h>
//#include <Servo.h>

bool active = false;
int current_ina, i;

int p_dir_1 = 10; // DIRECTION 1
int p_dir_2 = 11; // DIRECTION 2
int p_spd = 3;    // PWM SPEED
int p_ppr = 19;   // PAPER DETECTION INTERRUPT INT_2
//int p_srv_1 = pin;
//int p_srv_2 = pin;
//int p_vtg = pin;  // VOLTAGE CONTROL PIN (STOP DRIVER FULLY IF DOOR OPENS)
bool p_drc = true;  // DOOR CLOSE PIN

//Servo servo_1;    // DECLARE SERVOS
//Servo servo_2;
Adafruit_INA219 ina219;

void setup() {
  Serial.begin(115200);

  ina219.begin();
  // //servo_1.attach(p_srv_1);  // ATTACH SERVOS TO CONTROL PINS
  // //servo_2.attach(p_srv_2);

  pinMode(p_ppr, INPUT);  // PAPER DETECTION PIN
  pinMode(p_spd, OUTPUT); // SPEED
  pinMode(p_dir_1, OUTPUT);  // DIRECTION 1
  pinMode(p_dir_2, OUTPUT);  // DIRECTION 2

  TCCR3A = ( TCCR3A & B11111000 | B00000001 );    // TIMER MAIN MOTOR
  TCCR3B = ( TCCR3B & B11111000 | B00000010 );

  TCCR1A = ( TCCR1A & B11111100 | B00000000 );    // TIMER MOTOR OFF (CTC)
  TCCR1B = ( TCCR1B & B11101000 | B00001000 );    // PRESCALER OFF
  OCR1A = 62500;

  TIMSK1 |= B00000010;
  EIMSK |= B00000100;   // ENABLE INT1 PIN 21 INTERRUPT (PAPER DETECTION)
  EICRA |= B00100000;   // FALLING EDGE INTERRUPT
  sei();
}



ISR(INT2_vect){           // LACK OF PAPER DETECTION (PIN 19)
  TCCR1B = ( TCCR1B & B11111101 | B00000101 );  // TIMER ON MAX PRESCALER
}

ISR(TIMER1_COMPA_vect){   // 4 SECONDS OVERWORK
  TCCR1B = ( TCCR1B & B11111000 | B00000000 );  // TIMER OFF PRESCALER 0
  TCNT1 = 0;
  active = false;
}



void loop() {
  //if(p_drc){      // DOOR MUST BE CLOSED
    if(digitalRead(p_ppr)){
      active = true;
      Serial.println("detect");
    }
    if(active){   // NORMAL WORKING STATUS
      //servo_1.write(angle);       // DECLOGGING ON POSITION
      //servo_2.write(angle);
      
      digitalWrite(p_dir_1, LOW);   // DIRECTION
      digitalWrite(p_dir_2, HIGH);  // DIRECTION
      analogWrite(p_spd, 200);      // PWM 200
    }
    else{
      digitalWrite(p_dir_1, LOW);   // DIRECTION
      digitalWrite(p_dir_2, LOW);  // DIRECTION
    }

    current_ina = ina219.getCurrent_mA();
    Serial.println(current_ina);
    if(current_ina > 1000){         // ANTI-CLOGGING 
      digitalWrite(p_dir_1, LOW);   // STOP MOTOR
      digitalWrite(p_dir_2, LOW);
      delay(500);
      
      digitalWrite(p_dir_1, HIGH);  // SWAP DIRECTIONS
      digitalWrite(p_dir_2, LOW);
      for(i=50;i<101;i++){
        analogWrite(p_spd, i);
        delay(10);
      }
      delay(1000);
      
      digitalWrite(p_dir_1, LOW);   // RESTART WORKING STATUS
      digitalWrite(p_dir_2, HIGH);
      analogWrite(p_spd,100);
      delay(500);
      for(i=100;i<201;i++){
        analogWrite(p_spd, i);
        delay(10);
      }
    }
  //}
}
