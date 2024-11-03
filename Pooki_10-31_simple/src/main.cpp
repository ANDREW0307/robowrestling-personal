#include <Arduino.h>
#include <Servo.h>
#include <MPU6050_light.h>

/*    TODOS

  - make sure all functions in testing mode are working (find in start_routine() case 8)
  - make sure check_boundary works
  - when check_boundary works, create a check_boundary that counts iterations (sensor reinforcement)

*/


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

// ir sensor configurations
const bool irconfig_mid_only[5] = {false, false, true, false, false};
const bool irconfig_mid_three[5] = {false, true, true, true, false};
const bool irconfig_left_only[5] = {true, false, false, false, false};
const bool irconfig_right_only[5] = {false, false, false, false, true};
const bool irconfig_mid_midleft[5] = {false, true, true, false, false};
const bool irconfig_mid_midright[5] = {false, false, true, true, false};

// ========== general forward delcarations ==========
void setPinModes();
int getStrat();
void start_routine();
void wrap_around(bool direction); // 0 for wrap around left, 1 for wrap around right

// sensors
bool get_at_boundary();
bool startmod_started();

// motor movement
void move_forwards(int speed);
void move_forwards(int speed, int time);

void move_backwards(int speed);
void move_backwards(int speed, int time);

void move_turn_forwards(int left_speed, int right_speed);
void move_turn_forwards(int left_speed, int right_speed);
void move_turn_backwards(int left_speed, int right_speed, int time);
void move_turn_backwards(int left_speed, int right_speed, int time);

void turn_right(int speed, int time);
void turn_left(int speed, int time);

void brake_high();

// misc
void debug_mode_loop(int device);


// ========== function implementations 

// movement functions
void move_forwards(int speed) {
  analogWrite(L_MOTOR_FWD, speed);
  analogWrite(L_MOTOR_REV, 0);
  analogWrite(R_MOTOR_FWD, speed);
  analogWrite(R_MOTOR_REV, 0);
}
void move_forwards(int speed, int time) {
  analogWrite(L_MOTOR_FWD, speed);
  analogWrite(L_MOTOR_REV, 0);
  analogWrite(R_MOTOR_FWD, speed);
  analogWrite(R_MOTOR_REV, 0);
  delay(time);
  brake_high();
}
void move_backwards(int speed) {
  analogWrite(L_MOTOR_FWD, 0);
  analogWrite(L_MOTOR_REV, speed);
  analogWrite(R_MOTOR_FWD, 0);
  analogWrite(R_MOTOR_REV, speed);
} 
void move_backwards(int speed, int time) {
  analogWrite(L_MOTOR_FWD, 0);
  analogWrite(L_MOTOR_REV, speed);
  analogWrite(R_MOTOR_FWD, 0);
  analogWrite(R_MOTOR_REV, speed);
  delay(time);
  brake_high();
}
void move_turn_forwards(int left_speed, int right_speed) {
  analogWrite(L_MOTOR_FWD, left_speed);
  analogWrite(L_MOTOR_REV, 0);
  analogWrite(R_MOTOR_FWD, right_speed);
  analogWrite(R_MOTOR_REV, 0);
}
void move_turn_forwards(int left_speed, int right_speed, int time) {
  analogWrite(L_MOTOR_FWD, left_speed);
  analogWrite(L_MOTOR_REV, 0);
  analogWrite(R_MOTOR_FWD, right_speed);
  analogWrite(R_MOTOR_REV, 0);
  delay(time);
  brake_high();
}
void move_turn_backwards(int left_speed, int right_speed) {
  analogWrite(L_MOTOR_FWD, 0);
  analogWrite(L_MOTOR_REV, left_speed);
  analogWrite(R_MOTOR_FWD, 0);
  analogWrite(R_MOTOR_REV, right_speed);
}
void move_turn_backwards(int left_speed, int right_speed, int time) {
  analogWrite(L_MOTOR_FWD, 0);
  analogWrite(L_MOTOR_REV, left_speed);
  analogWrite(R_MOTOR_FWD, 0);
  analogWrite(R_MOTOR_REV, right_speed);
  delay(time);
  brake_high();
}
void turn_right(int speed, int time) {
  analogWrite(L_MOTOR_FWD, 0);
  analogWrite(L_MOTOR_REV, speed);
  analogWrite(R_MOTOR_FWD, speed);
  analogWrite(R_MOTOR_REV, 0);
  delay(time);
  brake_high();
}
void turn_left(int speed, int time) {
  analogWrite(L_MOTOR_FWD, 0);
  analogWrite(L_MOTOR_REV, speed);
  analogWrite(R_MOTOR_FWD, speed);
  analogWrite(R_MOTOR_REV, 0);
  delay(time);
  brake_high();
}
void brake_high() {
  digitalWrite(L_MOTOR_FWD, HIGH);
  digitalWrite(L_MOTOR_REV, HIGH);
  digitalWrite(R_MOTOR_FWD, HIGH);
  digitalWrite(R_MOTOR_REV, HIGH);
}


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
      // testing mode
      /*
        things to test in this order
        - move_forwards(int speed);
        - brake_high();
        - move_forwards(int speed, int time);
        - move_backwards(int speed);
        - move_backwards(int speed, int time);
        - turn_left(int speed, int time); turn_right(int speed, int time)
        - move_turn_forwards(int left_speed, int right_speed); move_turn_backwards(int left_speed, int right_speed);
        - move_turn_forwards(int left_speed, int right_speed, int time); move_turn_backwards(int left_speed, int right_speed, int time);
        - wrap_around();
      */
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
bool check_ir(bool desired_state[5]) {
  bool current_state[5];
  int ir_pins[] = {IR_LEFT, IR_MIDLEFT, IR_MID, IR_MIDRIGHT, IR_RIGHT};
  for (int i = 0; i < 5; i++) {
    current_state[i] = digitalRead(ir_pins[i]);
    if (current_state[i] != desired_state[i]) {
      return false;
    }
  }
  return true;
}
void wrap_around(bool direction) {
  int first_turn_speed = 100;
  int first_turn_time = 100;
  int move_turn_speed_faster = 255;
  int move_turn_speed_slower = 100;
  int move_turn_speed_time = 250;
  int second_turn_speed = 255;
  int second_turn_time = 150;
  if (direction == 0) {
    turn_left(first_turn_speed, first_turn_time);
    move_turn_forwards(move_turn_speed_faster, move_turn_speed_slower, move_turn_speed_time);
    turn_right(second_turn_speed, second_turn_time);
  } else {
    turn_right(first_turn_speed, first_turn_time);
    move_turn_forwards(move_turn_speed_slower, move_turn_speed_faster, move_turn_speed_time);
    turn_left(second_turn_speed, second_turn_time);
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
