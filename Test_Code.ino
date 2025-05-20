#include <Wire.h>             // I2C LIBRARY FOR SENSOR COMMUNICATION
#include <Adafruit_INA219.h>  // INA219 CURRENT SENSOR LIBRARY
#include <Servo.h>            // SERVO CONTROL LIBRARY

bool active = false, forward = true; // CONTROL VARIABLES, STATUS AND SPINNING DIRECTION
int current_ina, i;                   // CONTROL VARIABLES, CURRENT READING AND LOOP INDEX

bool dir_1 = LOW;   // DIRECTION 1 STATUS
bool dir_2 = LOW;   // DIRECTION 2 STATUS

int p_spd = 3;
int p_dir_1 = 10;
int p_dir_2 = 11;
int p_drc_1 = 17;
int p_drc_2 = 18;
int p_grn = 22;
int p_red = 24;
int p_srv_1 = 6;
int p_srv_2 = 7;
int p_seta = 9;

Servo servo_1;            // SERVO DECLARATION
Servo servo_2;
Adafruit_INA219 ina219;   // CURRENT SENSOR DECLARATION

void setup() {
  Serial.begin(115200); 

  ina219.begin();
  servo_1.attach(p_srv_1);  // ATTACH SERVOS TO CONTROL PINS
  servo_2.attach(p_srv_2);

  pinMode(p_spd, OUTPUT);     // PWM SPEED
  pinMode(p_dir_1, OUTPUT);  // DIRECTION 1 PIN
  pinMode(p_dir_2, OUTPUT);  // DIRECTION 2 PIN
  pinMode(p_drc_1, INPUT);   // DOOR CLOSED SENSOR 1
  pinMode(p_drc_2, INPUT);   // DOOR CLOSED SENSOR 2
  pinMode(p_grn, INPUT);     // GREEN BUTTON PIN
  pinMode(p_red, INPUT);     // RED BUTTON PIN
  pinMode(p_srv_1, OUTPUT);   // SERVO 1 CONTROL PIN
  pinMode(p_srv_2, OUTPUT);   // SERVO 2 CONTROL PIN
  pinMode(p_seta, INPUT);     // EMERGENCY BUTTON PIN

  TCCR3A = ( TCCR3A & B11111000 | B00000001 );    // MAIN MOTOR PWM TIMER CONTROL
  TCCR3B = ( TCCR3B & B11111000 | B00000010 );    // PWM PHASE CORRECT 9-BIT MODE / PRESCALER CLK/8
}

void loop() {
  if(digitalRead(p_grn) && !digitalRead(p_red) && digitalRead(p_drc_1) && digitalRead(p_drc_2)){  // GREEN BUTTON STABLISHES DIRECTION, DOOR MUST BE CLOSED
    forward = !forward;   // INVERT DIRECTION TO AVOID ACCIDENTAL TRAPPING
    delay(500);
    active = true;        // SET WORKING STATUS TO TRUE
  }
  else if(!digitalRead(p_grn) && digitalRead(p_red) || !digitalRead(p_drc_1) || !digitalRead(p_drc_2) || digitalRead(p_seta)){   // RED BUTTON, OPEN DOOR AND EMERGENCY BUTTON STOP ALL ACTIONS
    forward = true;       // SET DIRECTION TO FORWARD TO INVERT ON REARMAMENT
    active = false;       // SET WORKING STATUS TO FALSE
    dir_1 = LOW;          // FORCE MOTOR DRIVER TO STOP
    dir_2 = LOW;
    servo_1.write(0);         // RECLINE SERVO MOTORS
    servo_2.write(0);
  }

  if(forward && active /*&& digitalRead(p_ppr)*/){    // MOTOR FORWARD ACTIVATION ON FORWARD ACTIVE WORKING STATUS IF PAPER IS DETECTED
    dir_1 = LOW;              // SET DIRECTION TO FORWARD ACTIVE STATUS
    dir_2 = HIGH;
    for(i=100;i<201;i++){     // ENGANGE WORKING SPEED IN LINEAR MANNER TO AVOID CURRENT PEAKS
      analogWrite(p_spd, i);
      delay(10);
    }
    servo_1.write(0);         // RECLINE SERVO MOTORS
    servo_2.write(0);
  }
  if(!forward && active){                         // MOTOR BACKWARD ACTIVATION ON NOT FORWARD ACTIVE WORKING STATUS
    dir_1 = HIGH;
    dir_2 = LOW;
    for(i=100;i<201;i++){     // ENGANGE WORKING SPEED IN LINEAR MANNER TO AVOID CURRENT PEAKS
      analogWrite(p_spd, i);
      delay(10);
    }
    servo_1.write(90);         // ENGAGE SERVO MOTORS IN DECLOGGING POSITION
    servo_2.write(90);
  }

  digitalWrite(p_dir_1, dir_1);   // SET SELECTED DIRECTION TO DIRECTION CONTROL PINS
  digitalWrite(p_dir_2, dir_2);

  current_ina = ina219.getCurrent_mA();   // READ CURRENT SENSOR STATUS
  Serial.println(current_ina);
  if(current_ina > 1000){                 // IF PEAK CURRENT SURPASSES THRESHOLD, ENGAGE ANTI-CLOGGING PROCEDURE
    if(!forward){
      servo_1.write(0);         // RECLINE SERVO MOTORS
      servo_2.write(0);
    }
    digitalWrite(p_dir_1, LOW);           // FORCE MOTOR DRIVER TO STOP
    digitalWrite(p_dir_2, LOW);
    delay(500);
    digitalWrite(p_dir_1, !dir_1);        // SWAP MOTOR DIRECTION
    digitalWrite(p_dir_2, !dir_2);
    for(i=50;i<101;i++){                  // ENGAGE WORKING SPEED IN LINEAR MANNER TO AVOID CURRENT PEAKS
      analogWrite(p_spd, i);
      delay(10);
    }
    delay(500);
    digitalWrite(p_dir_1, dir_1);         // SWAP MOTOR DIRECTION TO ORIGINAL DIRECTION
    digitalWrite(p_dir_2, dir_2);
    analogWrite(p_spd,100);
    delay(500);
    for(i=100;i<201;i++){                 // ENGAGE WORKING SPEED IN LINEAR MANNER TO AVOID CURRENT PEAKS
      analogWrite(p_spd, i);
      delay(10);
    }
    if(!forward){
      servo_1.write(90);         // ENGAGE SERVO MOTORS IN DECLOGGING POSITION (ONLY IN BACKWARD MODE)
      servo_2.write(90);
    }
  }
}
