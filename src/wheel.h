#ifndef __WHEEL_H__
#define __WHEEL_H__

#include <Arduino.h>

#include "config.h"

#include "batt.h"
#include "pid.h"
#include "tach.h"
#include "util.h"

enum WheelMode {
    PWM_MODE,
    VOLTAGE_MODE,
    PID_MODE,
};

class Wheel {
    public:
        Wheel() {}

        Wheel(Tachometer tach, PIDController pid, uint32_t mosfet_pin, double max_voltage = 12, bool inverted = false) {
            this->tach = tach;
            this->pid = pid;

            this->mosfet_pin = mosfet_pin;
            this->inverted = inverted;
            this->max_voltage = max_voltage;

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
            if (this->mode != WheelMode::PID_MODE) {
                this->pid.reset();
                this->mode = WheelMode::PID_MODE;
            }

            this->pid.set(rpm);
        }

        /// @brief Returns true if the wheel is in PID control mode and its speed is within the rpm setpoint plus or minus the tolerance.
        /// @param tolerance
        /// @return If the wheel speed is within the tolerance.
        bool is_up_to_speed(uint32_t tolerance = 500) {
            return this->mode == WheelMode::PID_MODE && abs(this->tach.get_rpm() - this->pid.get_setpoint()) < tolerance;
        }

        /**
         * This function must be called at a regular interval.
         */
        void update() {
            this->tach.update();

            // double feedforward;
            switch (this->mode) {
                case WheelMode::PID_MODE:
                    this->pid.update((double) tach.get_rpm());
                    // feedforward = 0.00306967 * pow(EULER, pid.get_setpoint()*0.000206605) + 1.42467;
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
            pwm = constrain(pwm, 0, 255);
            analogWrite(this->mosfet_pin, this->inverted ? 255 - pwm : pwm);
        }
        void set_voltage_dont_change_state(double voltage) {
            uint32_t pwm = double_map(constrain(voltage, 0.0, this->max_voltage), 0.0, battery_voltage(), 0.0, 255);
            this->set_pwm_dont_change_state(pwm);
        }

        uint32_t mosfet_pin;
        bool inverted;
        double max_voltage = 12.0;

        double target_voltage;
        WheelMode mode = WheelMode::PWM_MODE;
};

#endif