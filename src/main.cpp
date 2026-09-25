#include <Arduino.h>
#include <utility>

#include "config.h"

#include "autotune.h"
#include "edgedetector.h"
#include "rolling_average.h"
#include "util.h"
#include "tach.h"
#include "feedforward.h"
#include "pid.h"
#include "wheel.h"

Tachometer wheel_a_tach(config::EDGES_PER_REVOLUTION);
ExponentialFeedForwardModel wheel_a_model(0.00309105, 1.39588, 2.20258);
PIDController wheel_a_pid(config::WHEEL_KP, config::WHEEL_KI, config::WHEEL_KD, config::WHEEL_INTEGRAL_THRESHOLD, &wheel_a_model);
Wheel wheel_a(std::move(wheel_a_tach), std::move(wheel_a_pid), config::WHEEL_A_PIN, 12.0, false);

Tachometer wheel_b_tach(config::EDGES_PER_REVOLUTION);
ExponentialFeedForwardModel wheel_b_model(0.00309105, 1.39588, 2.20258);
PIDController wheel_b_pid(config::WHEEL_KP, config::WHEEL_KI, config::WHEEL_KD, config::WHEEL_INTEGRAL_THRESHOLD);
Wheel wheel_b(std::move(wheel_b_tach), std::move(wheel_b_pid), config::WHEEL_B_PIN, 12.0, false);

void tach_a_interrupt() {
    wheel_a.tach.handle_interrupt();
}
void tach_b_interrupt() {
    wheel_b.tach.handle_interrupt();
}

/**
 * Returns true if the rev trigger is pressed.
 */
bool rev() {
    return config::REV_SWITCH_INVERTED ? !digitalRead(config::REV_SWITCH_PIN) : digitalRead(config::REV_SWITCH_PIN);
}

bool read_idle_switch() {
    return config::IDLE_SWITCH_INVERTED ? !digitalRead(config::IDLE_SWITCH_PIN) : digitalRead(config::IDLE_SWITCH_PIN);
}

void setup() {
    pinMode(config::REV_SWITCH_PIN, INPUT_PULLUP);
    pinMode(config::IDLE_SWITCH_PIN, INPUT_PULLUP);
    pinMode(config::TACH_A_PIN, INPUT_PULLUP);
    pinMode(config::TACH_B_PIN, INPUT_PULLUP);

    analogReadResolution(config::_ADC_RESOLUTION);
    // analogWriteResolution(PWM_RESOLUTION);

    Serial.begin(9600);

    // Setup tachometer interrupts
    attachInterrupt(digitalPinToInterrupt(config::TACH_A_PIN), tach_a_interrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(config::TACH_B_PIN), tach_b_interrupt, CHANGE);

    // if (rev()) {
    //     // Serial.println("Tuning wheel A");
    //     // tune_ff(&wheel_a, 11);
    //     Serial.println("Tuning wheel B");
    //     tune_ff(&wheel_b, 11);
    //     // analogWrite(WHEEL_A_PIN, 255);
    //     while(!rev()) {
    //         // TODO: We probably don't need to update the wheels in this loop
    //         wheel_a.update();
    //         wheel_b.update();
    //         delay(5);
    //     }
    // }

    EdgeDetector idle_switch_edge_detector;

    int num_cells = cell_count();
    bool was_up_to_speed = false;
    bool idle_enabled = false;
    while (true) {
        idle_switch_edge_detector.update(read_idle_switch());
        if (idle_switch_edge_detector.fallen()) {
            idle_enabled = !idle_enabled;
        }

        // Print debug info
        Serial.print("Wheel a RPM: ");
        Serial.print(wheel_a.tach.get_rpm());
        Serial.print(", Wheel a pid: ");
        Serial.print(wheel_a.pid.get());
        Serial.print(", Wheel a IR: ");
        Serial.print(digitalRead(config::TACH_A_PIN));

        Serial.print(", Wheel b RPM: ");
        Serial.print(wheel_b.tach.get_rpm());
        Serial.print(", Wheel b pid: ");
        Serial.print(wheel_b.pid.get());
        Serial.print(", Wheel b IR: ");
        Serial.print(digitalRead(config::TACH_B_PIN));

        Serial.print(", Idle enabled?: ");
        Serial.print(idle_enabled);
        
        Serial.print(", Voltage: ");
        Serial.println(battery_voltage());


        if(rev()) {
            wheel_a.set_rpm(35000);
            wheel_b.set_rpm(35000);
        }
        else if (idle_enabled) {
            wheel_a.set_rpm(26000);
            wheel_b.set_rpm(26000);
        }
        else {
            wheel_a.set_voltage(0);
            wheel_b.set_voltage(0);
        }

        if (is_battery_low(num_cells)) {
            // If the battery is low, continuously sound the buzzer
            // tone(BUZZER_PIN, 3000, 10);
        } else {
            // Check if the wheels have reached their target speed. If so, briefly sound the buzzer
            bool wheels_up_to_speed = wheel_a.is_up_to_speed(2000) && wheel_b.is_up_to_speed(2000);
            if (rev() && !was_up_to_speed && wheels_up_to_speed) { // On a rising edge
                Serial.println("BEEPING");
                tone(config::BUZZER_PIN, 2000, 50);
            }
            was_up_to_speed = wheels_up_to_speed;
        }

        wheel_a.update();
        wheel_b.update();

        delay(5);
    }
}

// The main loop is in setup() so we don't need global variables to persist data between loop runs
void loop() {
}
