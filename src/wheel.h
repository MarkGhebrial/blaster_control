#ifndef __WHEEL_H__
#define __WHEEL_H__

#include <Arduino.h>

#include "config.h"

#include "util.h"
#include "batt.h"
#include "tach.h"
#include "pid.h"

enum WheelMode {
    PWM_MODE,
    VOLTAGE_MODE,
    PID_MODE,
};

class Wheel {
    public:
        Wheel() {}

        Wheel(Tachometer tach, PIDController pid, uint32_t mosfet_pin, bool inverted = false) {
            this->tach = tach;
            this->pid = pid;

            this->mosfet_pin = mosfet_pin;
            this->inverted = inverted;

            pinMode(mosfet_pin, OUTPUT);
        }

        /**
         * Set the pwm duty cycle of the motor. 0 is off, 255 is full power.
         */
        void set_pwm(uint32_t pwm) {
            this->mode = WheelMode::PWM_MODE;
            this->pid.reset();

            this->set_pwm_dont_change_state(pwm);
        }

        /**
         * Set the voltage sent to the motor. The max value is the voltage
         * of the battery.
         */
        void set_voltage(double voltage) {
            this->mode = WheelMode::VOLTAGE_MODE;
            this->pid.reset();

            this->target_voltage = voltage;

            // int pwm = double_map(voltage, 0.0, battery_voltage(), 0.0, 255);
            // this->set_pwm(pwm);
            this->set_voltage_dont_change_state(this->target_voltage);
        }

        void set_rpm(uint32_t rpm) {
            this->mode = WheelMode::PID_MODE;

            this->pid.set(rpm);
        }

        /**
         * This function must be called at a regular interval.
         */
        void update() {
            this->tach.update();
            // if (this->in_voltage_mode) {
            //     this->pid.update(tach.get_rpm());
            //     this->target_voltage = pid.get();
            //     this->set_voltage(this->target_voltage);
            // }

            switch (this->mode) {
                case WheelMode::PID_MODE:
                    Serial.println("UPDATING PID CONTROLLER");
                    this->pid.update(tach.get_rpm());
                    target_voltage = pid.get();
                case WheelMode::VOLTAGE_MODE:
                    this->set_voltage_dont_change_state(this->target_voltage);
                    break;
                case WheelMode::PWM_MODE:
                    this->target_voltage = 0;
                    break;
            }
        }

        Tachometer tach;
        PIDController pid;

    private:
        void set_pwm_dont_change_state(uint32_t pwm) {
            analogWrite(this->mosfet_pin, this->inverted ? 255 - pwm : pwm);
        }
        void set_voltage_dont_change_state(double voltage) {
            int pwm = double_map(voltage, 0.0, battery_voltage(), 0.0, 255);
            this->set_pwm_dont_change_state(pwm);
        }

        uint32_t mosfet_pin;
        bool inverted;

        double target_voltage;
        WheelMode mode = WheelMode::PWM_MODE;
};

#endif