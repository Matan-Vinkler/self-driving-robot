#ifndef MOTORS_H_
#define MOTORS_H_

// Motor A connections
#define MOTOR_EN_A 9
#define MOTOR_IN_1 8
#define MOTOR_IN_2 7

// Motor B connections
#define MOTOR_EN_B 3
#define MOTOR_IN_3 5
#define MOTOR_IN_4 4

void init_motors() {
  // Enable motor A pins
  pinMode(MOTOR_EN_A, OUTPUT);
  pinMode(MOTOR_IN_1, OUTPUT);
  pinMode(MOTOR_IN_2, OUTPUT);

  // Enable motor B pins
  pinMode(MOTOR_EN_B, OUTPUT);
  pinMode(MOTOR_IN_3, OUTPUT);
  pinMode(MOTOR_IN_4, OUTPUT);

  // Turn off motors A and B
  digitalWrite(MOTOR_IN_1, LOW);
  digitalWrite(MOTOR_IN_2, LOW);
  digitalWrite(MOTOR_IN_3, LOW);
  digitalWrite(MOTOR_IN_4, LOW);
}

void setspeed_motor_a(unsigned int speed) {
  analogWrite(MOTOR_EN_A, speed);
}

void setspeed_motor_b(unsigned int speed) {
  analogWrite(MOTOR_EN_B, speed);
}

void turn_forward_motor_a() {
  digitalWrite(MOTOR_IN_1, HIGH);
  digitalWrite(MOTOR_IN_2, LOW);
}

void turn_forward_motor_b() {
  digitalWrite(MOTOR_IN_3, LOW);
  digitalWrite(MOTOR_IN_4, HIGH);
}

void turn_backward_motor_a() {
  digitalWrite(MOTOR_IN_1, LOW);
  digitalWrite(MOTOR_IN_2, HIGH);
}

void turn_backward_motor_b() {
  digitalWrite(MOTOR_IN_3, HIGH);
  digitalWrite(MOTOR_IN_4, LOW);
}

void turn_off_motor_a() {
  digitalWrite(MOTOR_IN_1, LOW);
  digitalWrite(MOTOR_IN_2, LOW);
}

void turn_off_motor_b() {
  digitalWrite(MOTOR_IN_3, LOW);
  digitalWrite(MOTOR_IN_4, LOW);
}

#endif