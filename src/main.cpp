#include <Arduino.h>

#include "rolling_average.h"
#include "util.h"
#include "tach.h"

#define REV_SWITCH_PIN 12

#define VOLTAGE_DIVIDER_PIN A2
#define VOLTAGE_DIVIDER_RATIO (22.0/122.0)

#define TACH_A_PIN 7
#define TACH_B_PIN 999 // TODO
#define EDGES_PER_REVOLUTION 2

#define WHEEL_A_PIN 9
#define WHEEL_B_PIN 10 // TODO

#define ADC_RESOLUTION 12
#define PWM_RESOLUTION 12

Tachometer wheel_a_tach(EDGES_PER_REVOLUTION);
void tach_a_interrupt() {
    wheel_a_tach.handle_interrupt();
}

double battery_voltage() {
    // return analogRead(A2);
    return double_map((double) analogRead(VOLTAGE_DIVIDER_PIN), 0.0, 4096.0, 0, 3.3) * (1.0/VOLTAGE_DIVIDER_RATIO);
}

void set_wheel_a_voltage(double volts) {
    int duty_cycle = double_map(volts, 0.0, battery_voltage(), 0.0, 4096);
    Serial.print("Setting wheel a duty cycle: ");
    Serial.print(duty_cycle);
    Serial.print(", ");
    analogWrite(WHEEL_A_PIN, 4096 - duty_cycle);
}

void setup() {
    // put your setup code here, to run once:
    pinMode(REV_SWITCH_PIN, INPUT_PULLDOWN);
    pinMode(TACH_A_PIN, INPUT_PULLUP);

    analogReadResolution(ADC_RESOLUTION);
    analogWriteResolution(PWM_RESOLUTION);

    Serial.begin(9600);

    // Setup tachometers
    attachInterrupt(digitalPinToInterrupt(TACH_A_PIN), tach_a_interrupt, CHANGE);
}

void loop() {
    Serial.print("RPM: ");
    Serial.print(wheel_a_tach.get_rpm());
    Serial.print(", Voltage: ");
    Serial.println(battery_voltage());

    if(digitalRead(REV_SWITCH_PIN)) {
        // analogWrite(WHEEL_A_PIN, 255);
        set_wheel_a_voltage(2);
        analogWrite(WHEEL_B_PIN, 4096);
    }
    else {
        analogWrite(WHEEL_A_PIN, 4096);
        // set_wheel_a_voltage(0);
        analogWrite(WHEEL_B_PIN, 4096);
    }
}
