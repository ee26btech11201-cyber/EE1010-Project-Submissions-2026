#pragma once

void motor_init();
// Signed PWM values: -255 is full reverse, 0 is stopped, +255 is full forward.
void motor_set_speeds(int left_speed, int right_speed);
void motor_forward();
void motor_back();
void motor_left();
void motor_right();
void motor_stop();
