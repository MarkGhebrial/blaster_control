/**
 * This file contains all the code for the optical tachometers
 */

#ifndef __TACH_H__
#define __TACH_H__

#include <Arduino.h>

#include "rolling_average.h"

class Tachometer {
    public:
        Tachometer() {}

        Tachometer(const Tachometer& t) {
            *this = t;
        }

        Tachometer(uint32_t edges_per_revolution) {
            this->edges_per_revolution = edges_per_revolution;
            time_of_last_sample = 0;

            this->filter = RollingAverage<uint32_t, 20>();
        }

        void handle_interrupt() {
            // Compute the time since the last interrupt
            unsigned long current_time = micros();
            unsigned long elapsed_micros = micros() - time_of_last_sample;
            // time_of_last_sample = current_time;

            // if (elapsed_micros <= 1500) {
            //     edges_per_sample++;
            // }
            // if (edges_per_sample > 1 && elapsed_micros >= 2000) {
            //     edges_per_sample--;
            // }

            edges_since_last_sample++;
            if (edges_since_last_sample >= edges_per_sample) {
                
                double new_rpm = (1 / (double) elapsed_micros) *
                    (edges_since_last_sample) * 
                    (1.0 / (double) this->edges_per_revolution) *
                    (60000000); // 60000000 is the number of microseconds in a minute
                
                time_of_last_sample = current_time;
                edges_since_last_sample = 0;

                // Ignore the new reading if the difference from the last one is absurdly large
                if (abs(new_rpm - this->rpm) > 50000) {
                    return;
                }

                // Update the rolling average filter
                filter.update(new_rpm);
                this->rpm = filter.get_average();
            } 
        }

        void update() {
            unsigned long elapsed_micros = micros() - time_of_last_sample;

            // If no edge has been detected for more than half a second, set the rpm to zero
            if (elapsed_micros > 500000) {
                this->rpm = 0;
            }
        }

        uint32_t get_rpm() {
            return this->rpm;
        }

    private:
        unsigned long time_of_last_sample;
        uint32_t edges_per_revolution;
        uint32_t rpm;
        RollingAverage<uint32_t, 20> filter;

        // Higher RPMs mean less elapsed time between sensor readings. Since the timer is
        // only accurate to within a microsecond, the rpm resolution decreases significantly
        // as speeds increase. (+- 200ish RPM at 40k RPM). To reduce this effect, we can
        // increase the amount of time between readings by skipping some sensor edges.
        //
        // The RPM reading is only updated when edges_since_last_sample >= edges_per_sample.
        // edges_per_sample is dynamically updated based on the time between samples. That
        // ensures that the RPM reading is accurate to within 30ish RPM.
        //
        // TODO: All this is currently broken and makes the RPM readings much less consistent.
        // For now, edges_per_sample is fixed at 1.
        uint32_t edges_per_sample = 1;
        uint32_t edges_since_last_sample = 0;
};

#endif