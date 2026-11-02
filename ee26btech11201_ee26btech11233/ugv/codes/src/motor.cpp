#include "motor.h"
#include <Arduino.h>

// L293D direction inputs
#define IN1 32   // Motor A — direction pin 1
#define IN2 33   // Motor A — direction pin 2
#define IN3 25   // Motor B — direction pin 1
#define IN4 26   // Motor B — direction pin 2

// PWM outputs control the L293D enable pins (EN1 and EN2).
#define EN1_PWM 27  // Connect to L293D pin 1 (EN1)
#define EN2_PWM 14  // Connect to L293D pin 9 (EN2)

static constexpr uint32_t PWM_FREQUENCY_HZ = 20000;
static constexpr uint8_t PWM_RESOLUTION_BITS = 8; // duty range 0..255

static void set_one_motor(int speed, int input_a, int input_b, int enable_pin) {
    speed = constrain(speed, -255, 255);

    if (speed > 0) {
        digitalWrite(input_a, HIGH);
        digitalWrite(input_b, LOW);
    } else if (speed < 0) {
        digitalWrite(input_a, LOW);
        digitalWrite(input_b, HIGH);
    } else {
        digitalWrite(input_a, LOW);
        digitalWrite(input_b, LOW);
    }

    ledcWrite(enable_pin, abs(speed));
}

void motor_init() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    if (!ledcAttach(EN1_PWM, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS)) {
        Serial.println("PWM setup failed for left motor (GPIO27)");
    }
    if (!ledcAttach(EN2_PWM, PWM_FREQUENCY_HZ, PWM_RESOLUTION_BITS)) {
        Serial.println("PWM setup failed for right motor (GPIO14)");
    }

    motor_stop(); // safe default state on boot
}

void motor_set_speeds(int left_speed, int right_speed) {
    set_one_motor(left_speed, IN1, IN2, EN1_PWM);
    set_one_motor(right_speed, IN3, IN4, EN2_PWM);
}

void motor_forward() {
    motor_set_speeds(255, 255);
}

void motor_back() {
    motor_set_speeds(-255, -255);
}

void motor_left() {
    motor_set_speeds(-255, 255);
}

void motor_right() {
    motor_set_speeds(255, -255);
}

void motor_stop() {
    motor_set_speeds(0, 0);
}
