#include <Arduino.h>
#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>
#include "motor.h"

static constexpr int LEFT_PWM = 240;
static constexpr int RIGHT_PWM = 255;

void setup() {
    Serial.begin(115200);
    motor_init();
    Dabble.begin("ESP32_UGV");
    Serial.println("ESP32 UGV ready — connect with Dabble GamePad");
}

void loop() {
    Dabble.processInput();

    int leftSpeed = 0;
    int rightSpeed = 0;
    uint8_t control = 0; // STOP

    if (GamePad.isUpPressed()) {
        leftSpeed = LEFT_PWM;
        rightSpeed = RIGHT_PWM;
        control = 1; // UP
    } else if (GamePad.isDownPressed()) {
        leftSpeed = -LEFT_PWM;
        rightSpeed = -RIGHT_PWM;
        control = 2; // DOWN
    } else if (GamePad.isLeftPressed()) {
        leftSpeed = -LEFT_PWM;
        rightSpeed = RIGHT_PWM;
        control = 3; // LEFT
    } else if (GamePad.isRightPressed()) {
        leftSpeed = LEFT_PWM;
        rightSpeed = -RIGHT_PWM;
        control = 4; // RIGHT
    }

    static uint8_t previousControl = 255;
    if (control != previousControl) {
        const char* names[] = {"STOP", "UP", "DOWN", "LEFT", "RIGHT"};
        Serial.printf("Dabble input: %s (left=%d, right=%d)\n",
                      names[control], leftSpeed, rightSpeed);
        previousControl = control;
    }

    motor_set_speeds(leftSpeed, rightSpeed);
}
