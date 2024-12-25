/**
 * This file contains all the code for the optical tachometers
 */

#ifndef __TACH_H__
#define __TACH_H__

#include <Arduino.h>

#include "rolling_average.h"

class Tachometer {
    public:
        Tachometer(uint32_t edges_per_revolution) {
            this->edges_per_revolution = edges_per_revolution;
            time_of_last_interrupt = 0;

            this->filter = RollingAverage<uint32_t, 4>();
        }

        void handle_interrupt() {
            unsigned long current_time = micros();
            unsigned long elapsed_micros = micros() - time_of_last_interrupt;
            time_of_last_interrupt = current_time;

            this->rpm = (1 / (double) elapsed_micros) *
                        (1.0 / (double) this->edges_per_revolution) *
                        (60000000); // 60000000 is the number of microseconds in a minute

            filter.update(this->rpm);
            this->rpm = filter.get_average();
        }

        uint32_t get_rpm() {
            return this->rpm;
        }

    private:
        unsigned long time_of_last_interrupt;
        uint32_t edges_per_revolution;
        uint32_t rpm;
        RollingAverage<uint32_t, 4> filter;
};

#endif