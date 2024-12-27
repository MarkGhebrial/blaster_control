#include <Arduino.h>

#include "rolling_average.h"
#include "util.h"
#include "tach.h"
#include "pid.h"
#include "feedforward.h"

#define REV_SWITCH_PIN 12

#define VOLTAGE_DIVIDER_PIN A2
#define VOLTAGE_DIVIDER_RATIO (22.0/122.0)

#define TACH_A_PIN 7
#define TACH_B_PIN 999 // TODO
#define EDGES_PER_REVOLUTION 2

#define WHEEL_A_PIN 11
#define WHEEL_B_PIN 9

#define ADC_RESOLUTION 12
#define PWM_RESOLUTION 12

Tachometer wheel_a_tach(EDGES_PER_REVOLUTION);
PIDController wheel_a_pid(0.0006, 0.0000000001, 0.001);

void tach_a_interrupt() {
    wheel_a_tach.handle_interrupt();
}

double battery_voltage() {
    // return analogRead(A2);
    return double_map((double) analogRead(VOLTAGE_DIVIDER_PIN), 0.0, 4096.0, 0, 3.3) * (1.0/VOLTAGE_DIVIDER_RATIO);
}

void set_wheel_a_voltage(double volts) {
    int duty_cycle = double_map(volts, 0.0, battery_voltage(), 0.0, 255);
    analogWrite(WHEEL_A_PIN, constrain(255 - duty_cycle, 0, 255));
}

void setup() {
    pinMode(REV_SWITCH_PIN, INPUT_PULLDOWN);
    pinMode(TACH_A_PIN, INPUT_PULLUP);

    pinMode(WHEEL_A_PIN, OUTPUT);
    pinMode(WHEEL_B_PIN, OUTPUT);

    analogReadResolution(ADC_RESOLUTION);
    // analogWriteResolution(PWM_RESOLUTION);

    Serial.begin(9600);

    // Setup tachometers
    attachInterrupt(digitalPinToInterrupt(TACH_A_PIN), tach_a_interrupt, CHANGE);

    double kS, kV;
    analogWrite(WHEEL_B_PIN, 255);
    tune_ff(&wheel_a_tach, set_wheel_a_voltage, 9, &kS, &kV);
    analogWrite(WHEEL_A_PIN, 255);
    while(!digitalRead(REV_SWITCH_PIN));
}

void loop() {
    wheel_a_tach.update();

    Serial.print("RPM: ");
    Serial.print(wheel_a_tach.get_rpm());
    Serial.print(", Wheel a pid: ");
    Serial.print(wheel_a_pid.get());
    Serial.print(", Voltage: ");
    Serial.println(battery_voltage());

    if(digitalRead(REV_SWITCH_PIN)) {
        // analogWrite(WHEEL_A_PIN, 255);
        wheel_a_pid.set(40000);
        wheel_a_pid.update((double) wheel_a_tach.get_rpm());
        set_wheel_a_voltage(constrain(wheel_a_pid.get(), 0, 12));
        // analogWrite(WHEEL_A_PIN, 255 - 255);
        // analogWrite(WHEEL_B_PIN, 255 - 255);
    }
    else {
        wheel_a_pid.reset();
        analogWrite(WHEEL_A_PIN, 255);
        // set_wheel_a_voltage(0);
        analogWrite(WHEEL_B_PIN, 255);
    }

    delay(5);
}
