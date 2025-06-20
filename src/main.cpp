#include <Arduino.h>

#include "config.h"

#include "rolling_average.h"
#include "util.h"
#include "tach.h"
#include "pid.h"
#include "feedforward.h"
#include "wheel.h"

Tachometer wheel_a_tach(EDGES_PER_REVOLUTION);
PIDController wheel_a_pid(WHEEL_KP, WHEEL_KI, WhEEL_KD, WHEEL_INTEGRAL_THRESHOLD);
Wheel wheel_a(wheel_a_tach, wheel_a_pid, WHEEL_A_PIN, 12.0, false);

Tachometer wheel_b_tach(EDGES_PER_REVOLUTION);
PIDController wheel_b_pid(WHEEL_KP, WHEEL_KI, WhEEL_KD, WHEEL_INTEGRAL_THRESHOLD);
Wheel wheel_b(wheel_b_tach, wheel_b_pid, WHEEL_B_PIN, 12.0, false);

bool was_up_to_speed = false;

void tach_a_interrupt() {
    wheel_a.tach.handle_interrupt();
}
void tach_b_interrupt() {
    wheel_b.tach.handle_interrupt();
}

void setup() {
    // TODO: On startup, get battery voltage and use it to calculate the number
    // of cells. Then, while running, use the number of cells to sound an alarm
    // when the voltage drops too low;

    pinMode(REV_SWITCH_PIN, INPUT_PULLUP);
    pinMode(TACH_A_PIN, INPUT_PULLUP);
    pinMode(TACH_B_PIN, INPUT_PULLUP);

    analogReadResolution(ADC_RESOLUTION);
    // analogWriteResolution(PWM_RESOLUTION);

    Serial.begin(9600);

    // Setup tachometer interrupts
    attachInterrupt(digitalPinToInterrupt(TACH_A_PIN), tach_a_interrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(TACH_B_PIN), tach_b_interrupt, CHANGE);

    // double kS, kV;
    // analogWrite(WHEEL_B_PIN, 255);
    // if (digitalRead(REV_SWITCH_PIN)) {
    //     tune_ff(&wheel_a_tach, set_wheel_a_voltage, 9, &kS, &kV);
    //     analogWrite(WHEEL_A_PIN, 255);
    //     while(!digitalRead(REV_SWITCH_PIN));
    // }

    // tone(5, 2000);
}

void loop() {
    // Print debug info
    Serial.print("Wheel a RPM: ");
    Serial.print(wheel_a.tach.get_rpm());
    Serial.print(", Wheel a pid: ");
    Serial.print(wheel_a.pid.get());

    Serial.print(", Wheel b RPM: ");
    Serial.print(wheel_b.tach.get_rpm());
    Serial.print(", Wheel b pid: ");
    Serial.print(wheel_b.pid.get());

    Serial.print(", Voltage: ");
    Serial.println(battery_voltage());


    if(!digitalRead(REV_SWITCH_PIN)) {
        wheel_a.set_rpm(42000);
        wheel_b.set_rpm(42000);
    }
    else {
        wheel_a.set_voltage(0);
        wheel_b.set_voltage(0);
    }

    // Check if the wheels have reached their speed. If so, briefly sound the buzzer
    bool wheels_up_to_speed = wheel_a.is_up_to_speed(2000) && wheel_b.is_up_to_speed(2000);
    if (!was_up_to_speed && wheels_up_to_speed) { // On a rising edge
        Serial.println("BEEPING");
        tone(BUZZER_PIN, 2000, 50);
    }
    was_up_to_speed = wheels_up_to_speed;

    wheel_a.update();
    wheel_b.update();

    delay(5);
}
