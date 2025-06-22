#ifndef __AUTOTUNE_H__
#define __AUTOTUNE_H__

#include "wheel.h"
#include "rolling_average.h"

void tune_ff(Wheel* wheel, double max_voltage) {
    double data[VOLTAGE_STEPS][2];// = {0}{0};

    for (int i = 2; i <= VOLTAGE_STEPS; i++) {
        double voltage = (max_voltage / VOLTAGE_STEPS) * i;
        wheel->set_voltage(voltage);

        RollingAverage<int32_t, 50> acceleration_average = RollingAverage<int32_t, 50>();
        // RollingAverage<double, 20> voltage_average = RollingAverage<double, 20>(battery_voltage());
        RollingAverage<uint32_t, 20> speed_average = RollingAverage<uint32_t, 20>();

        uint32_t prev_rpm = wheel->tach.get_rpm();
        unsigned long end_time = millis() + 10000;
        // Loop for 3 seconds or until acceleration has stabilized, whichever comes later
        while(millis() < end_time || abs(acceleration_average.get_average()) < 2) {
            delay(5);
            
            acceleration_average.update((int32_t) wheel->tach.get_rpm() - (int32_t) prev_rpm);
            speed_average.update(wheel->tach.get_rpm());
            prev_rpm = wheel->tach.get_rpm();
            
            // Serial.print("Average acceleration: ");
            // Serial.println(acceleration_average.get_average());
            
            // voltage_average.update(battery_voltage());
            // if (voltage_average.get_average() < voltage) {
                //     Serial.println("Battery voltage too low");
                //     return;
                // }
                
            wheel->update();
        }
        
        data[i-1][0] = (double) speed_average.get_average();
        data[i-1][1] = voltage;

        Serial.print("RPM at ");
        Serial.print(voltage);
        Serial.print(" volts is: ");
        Serial.println(speed_average.get_average());
    }

    wheel->set_voltage(0);

    // Print the data as a csv
    for (auto point : data) {
        Serial.print(point[1]);
        Serial.print(',');
        Serial.println(point[0]);
    }

    // Serial.println("Computing regression");
    // LinearRegression regression(data, VOLTAGE_STEPS);

    // Serial.print("Model: V = ");
    // Serial.print(regression.getSlope());
    // Serial.print("x + ");
    // Serial.println(regression.getIntercept());
}

#endif