#ifndef __PID_H__
#define __PID_H__

#include <Arduino.h>

#include "feedforward.h"

class PIDController {
    public:
        PIDController() {}

        PIDController(const PIDController& p) {
            *this = p;
        }

        PIDController(double kP, double kI, double kD, double integralThreshold) {
            this->kP = kP;
            this->kI = kI;
            this->kD = kD;
            this->integralThreshold = integralThreshold;
            this->time_of_last_update = micros();
        }

        PIDController(double kP, double kI, double kD, double integralThreshold, FeedForwardModel* model) : PIDController(kP, kI, kD, integralThreshold) {
            this->model = model;
        }

        void reset() {
            this->error_sum = 0;
            this->time_of_last_update = micros();
        }

        void set(double setpoint) {
            this->setpoint = setpoint;
        }

        void update(double pv) {
            // Compute the error
            double error = setpoint - pv;

            // Compute the elapsed time
            unsigned long current_time = micros();
            unsigned long elapsed_time = current_time - this->time_of_last_update;
            this->time_of_last_update = current_time;

            // Integrate the error
            if (abs(error) <= this->integralThreshold) {
                this->error_sum += error * (double) elapsed_time;
            }
            
            double delta_error = previous_error - error;
            previous_error = error;

            this->output = (error * this->kP) + (error_sum * this->kI) + ((delta_error/elapsed_time) * kD);
        }

        double get() {
            if (this->model) {
                return this->output + model->compute(this->setpoint);
            } else {
                return this->output;
            }
        }

        double get_setpoint() {
            return setpoint;
        }

    protected:
        double kP;
        double kI;
        double kD;
        double integralThreshold;

        unsigned long time_of_last_update;
        double previous_error;
        double error_sum = 0;

        double setpoint = 0;
        double output = 0;

        // A pointer to a feed forward model
        FeedForwardModel* model = nullptr;
};

#endif