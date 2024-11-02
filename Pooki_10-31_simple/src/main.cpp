#include <Arduino.h>
#include <Servo.h>
#include <MPU6050_light.h>

// ========== pin definitions ==========

// sensors
#define IR_LEFT 7
#define IR_MIDLEFT 1 
#define IR_MID 4 
#define IR_MIDRIGHT 8
#define IR_RIGHT 2

#define LINE_LEFT A0
#define LINE_RIGHT A1

#define START_MOD 10

#define DIP_1 A6
#define DIP_2 A7
#define DIP_3 A5

// motors
#define L_MOTOR_FWD 11
#define L_MOTOR_REV 6
#define R_MOTOR_FWD 5
#define R_MOTOR_REV 3

#define SERVO 9


// ========== global variables ==========

bool debugMode = false;


// ========== forward delcarations ==========
void setPinModes();
int getStrat();
void start_routine(); 

// sensors
bool get_at_boundary();
bool startmod_started();

// motor movement
void move_forwards(int speed);
void move_forwards(int speed, int time);

void move_backwards(int speed);
void move_backwards(int speed, int time);

void move_turn(int left_speed, int right_speed);
void move_turn(int left_speed, int right_speed, int time);

void turn_right(int speed, int time);
void turn_left(int speed, int time);

// misc
void debug_mode_loop(int device);


// ========== function implementations 

void setPinModes() {
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_MIDLEFT, INPUT);
  pinMode(IR_MID, INPUT);
  pinMode(IR_MIDRIGHT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  pinMode(LINE_LEFT, INPUT);
  pinMode(LINE_RIGHT, INPUT);

  pinMode(START_MOD, INPUT);

  pinMode(DIP_1, INPUT);
  pinMode(DIP_2, INPUT);
  pinMode(DIP_3, INPUT);

  pinMode(L_MOTOR_FWD, OUTPUT);
  pinMode(L_MOTOR_REV, OUTPUT);
  pinMode(R_MOTOR_FWD, OUTPUT);
  pinMode(R_MOTOR_REV, OUTPUT);

  pinMode(SERVO, OUTPUT);
}
int getStrat() {
  // dipswitch values range from 0 (OFF) to 1024 (ON)
  bool switch_one = analogRead(DIP_1) > 500 ? true : false;
  bool switch_two = analogRead(DIP_2) > 500 ? true : false;
  bool switch_three = analogRead(DIP_3) > 500 ? true : false; 

  // shift bits and combine to form complete binary number
  int strat = (switch_one << 2) | (switch_two << 1) | switch_three;
  return strat + 1; // add 1 so 0-7 becomes 1-8
}
void start_routine() {
  switch(getStrat()) {
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    case 4:
      break;
    case 5:
      break;
    case 6:
      break;
    case 7:
      break;
    case 8:
      break;
    default:
      break;
  }
}
bool get_at_boundary() {
  const int edge_threshold = 40;
  return analogRead(LINE_LEFT) < edge_threshold || analogRead(LINE_RIGHT) < edge_threshold;
}
bool startmod_started() { 
  return digitalRead(START_MOD); 
}

void debug_mode_loop(int device) {
  switch (device) {
    case IR_LEFT:
      Serial.println("IR LEFT value: " + digitalRead(IR_LEFT));
      break;
    case IR_MIDLEFT:
      Serial.println("IR MIDLEFT value: " + digitalRead(IR_MIDLEFT));
      break;
    case IR_MID:
      Serial.println("IR MID value: " + digitalRead(IR_MID));
      break;
    case IR_MIDRIGHT:
      Serial.println("IR MIDRIGHT value: " + digitalRead(IR_MIDRIGHT));
      break;
    case IR_RIGHT:
      Serial.println("IR RIGHT value: " + digitalRead(IR_RIGHT));
    case LINE_LEFT:
      Serial.println("LINE LEFT value: " + analogRead(LINE_LEFT));
      break;
    case LINE_RIGHT:
      Serial.println("LINE RIGHT value: " + analogRead(LINE_RIGHT));
      break;
    case START_MOD:
      Serial.println("Start module value: " + digitalRead(START_MOD));
      break;
    case DIP_1:
      Serial.println("DIP 1 value: " + analogRead(DIP_1));
      break;
    case DIP_2:
      Serial.println("DIP 2 value: " + analogRead(DIP_2));
      break;
    case DIP_3:
      Serial.println("DIP 3 value: " + analogRead(DIP_3));
      break;
  }
}

void setup() {
  setPinModes();

  if (debugMode) { Serial.begin(9600); }

  if (startmod_started()) {
    start_routine();
  }
}

void loop() {
  if (debugMode) {
    debug_mode_loop(IR_LEFT);
  }

  // todo: find sampling rate of sensors
  delay(50);
}
