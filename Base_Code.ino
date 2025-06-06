#include <Wire.h>             // I2C LIBRARY FOR SENSOR COMMUNICATION
#include <Adafruit_INA219.h>  // INA219 CURRENT SENSOR LIBRARY
#include <Servo.h>            // SERVO CONTROL LIBRARY

bool starter;
int current_ina, i;                   // CONTROL VARIABLES, CURRENT READING AND LOOP INDEX

int p_spd = 3;      // MAIN MOTOR SPEED
int p_vent = 4;     // FAN SPEED
int p_spir = 5;     // SPIRAL SPEED
int p_dir_1 = 10;   // MAIN MOTOR DIRECTION CONTROL
int p_dir_2 = 11;   // MAIN MOTOR DIRECTION CONTROL
int p_drc_1 = 17;   // DOOR SENSOR
int p_srv_1 = 6;    // SERVO 1 CONTROL
int p_srv_2 = 7;    // SERVO 2 CONTROL

Servo servo_1;            // SERVO DECLARATION
Servo servo_2;
Servo servo_3;            // SPIRAL SERVO DECLARATION
Adafruit_INA219 ina219;   // CURRENT SENSOR DECLARATION

void setup() {
  Serial.begin(115200); 

  ina219.begin();           // CURRENT SENSOR INITIALITATION
  servo_1.attach(p_srv_1);  // ATTACH SERVOS TO CONTROL PINS
  servo_2.attach(p_srv_2);
  servo_3.attach(p_spir);

  pinMode(p_spd, OUTPUT);           // PWM MAIN MOTOR SPEED
  pinMode(p_vent, OUTPUT);          // PWM FAN SPEED
  pinMode(p_spir, OUTPUT);
  pinMode(p_dir_1, OUTPUT);         // DIRECTION 1 PIN
  pinMode(p_dir_2, OUTPUT);         // DIRECTION 2 PIN
  pinMode(p_drc_1, INPUT_PULLUP);   // DOOR CLOSED SENSOR 1
  pinMode(p_grn, INPUT_PULLUP);     // GREEN BUTTON PIN
  pinMode(p_red, INPUT_PULLUP);     // RED BUTTON PIN
  pinMode(p_srv_1, OUTPUT);         // SERVO 1 CONTROL PIN
  pinMode(p_srv_2, OUTPUT);         // SERVO 2 CONTROL PIN
  pinMode(p_seta, INPUT);           // EMERGENCY BUTTON PIN

  TCCR3A = ( TCCR3A & B11111000 | B00000001 );    // MAIN MOTOR PWM TIMER CONTROL
  TCCR3B = ( TCCR3B & B11111000 | B00000010 );    // PWM PHASE CORRECT 9-BIT MODE / PRESCALER CLK/8
  
  analogWrite(p_vent, 0);     // START FAN AND SPIRAL OFF
  analogWrite(p_spd, 0);
  servo_1.write(90);          // RECLINE SERVO MOTORS
  servo_2.write(90);
  servo_3.write(90);
  starter = false;
}

void loop(){
  if(!digitalRead(p_drc_1) && starter == false){  // IF DOOR IS CLOSED, MACHINE CAN PROCEED
    digitalWrite(p_dir_1, LOW);           // SET SELECTED DIRECTION TO DIRECTION CONTROL PINS
    digitalWrite(p_dir_2, HIGH);
    for(i=50;i<250;i++){                  // ENGAGE WORKING SPEED IN LINEAR MANNER TO AVOID CURRENT PEAKS
      analogWrite(p_spd, i);
      delay(10);
    } 
    analogWrite(p_vent, 80);              // TURN ON FAN
    servo_3.write(180);                   // TURN ON SPIRAL
    starter = true;                       // INITIAL SEQUENCE STARTED
  }
  else if(digitalRead(p_drc_1)){          // IF DOOR IS OPEN, STOP ALL ACTIONS
    digitalWrite(p_dir_1, LOW);           // SET SELECTED DIRECTION TO DIRECTION CONTROL PINS
    digitalWrite(p_dir_2, LOW);
    for(i=200;i>50;i--){                  // ENGAGE WORKING SPEED IN LINEAR MANNER TO AVOID CURRENT PEAKS
      analogWrite(p_spd, i);
      delay(10);
    }
    analogWrite(p_vent, 0);               // TURN OFF FAN
    servo_3.write(90);                    // TURN OFF SPIRAL
    starter = false;                      // INITIAL SEQUENCE READY TO REPEAT
  }

  current_ina = ina219.getCurrent_mA();         // READ CURRENT SENSOR STATUS
  if(current_ina > 1600 || current_ina < -700){ // IF PEAK CURRENT SURPASSES THRESHOLD, ENGAGE ANTI-CLOGGING PROCEDURE
    digitalWrite(p_dir_1, LOW);                 // FORCE MOTOR DRIVER TO STOP
    digitalWrite(p_dir_2, LOW);
    delay(500);
    servo_1.write(110);                   // ENGAGE SERVO MOTORS IN DECLOGGING POSITION (ONLY IN BACKWARD MODE)
    servo_2.write(50);
    digitalWrite(p_dir_1, HIGH);          // SWAP MOTOR DIRECTION
    digitalWrite(p_dir_2, LOW);
    for(i=50;i<101;i++){                  // ENGAGE WORKING SPEED IN LINEAR MANNER TO AVOID CURRENT PEAKS
      analogWrite(p_spd, i);
      delay(10);
    }
    delay(500);
    digitalWrite(p_dir_1, LOW);           // SWAP MOTOR DIRECTION TO ORIGINAL DIRECTION
    digitalWrite(p_dir_2, HIGH);
    analogWrite(p_spd,100);
    delay(500);
    servo_1.write(90);                    // RECLINE SERVO MOTORS
    servo_2.write(90);
    starter = false;
  }
}
