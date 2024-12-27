#ifndef __FEEDFORWARD_H__
#define __FEEDFORWARD_H__

#include <Arduino.h>

#include "tach.h"
#include "rolling_average.h"
#include "linear_regression.h"

#define VOLTAGE_STEPS 9

void tune_ff(Tachometer *tach, void (*set_voltage)(double), double max_voltage, double* kS, double* kV) {
    double data[VOLTAGE_STEPS][2];// = {0}{0};

    for (int i = 1; i <= VOLTAGE_STEPS; i++) {
        double voltage = (max_voltage / VOLTAGE_STEPS) * i;

        RollingAverage<int32_t, 20> roc_average = RollingAverage<int32_t, 20>();

        uint32_t prev_rpm = tach->get_rpm();
        unsigned long end_time = millis() + 2000;
        while(millis() < end_time || roc_average.get_average() < 20) {
            delay(5);
            roc_average.update((int32_t) prev_rpm - (int32_t) tach->get_rpm());
            prev_rpm = tach->get_rpm();

            set_voltage(voltage);
        }
        
        data[i-1][0] = (double) tach->get_rpm();
        data[i-1][1] = voltage;

        Serial.print("RPM at ");
        Serial.print(voltage);
        Serial.print(" volts is: ");
        Serial.println(tach->get_rpm());
    }

    Serial.println("Computing regression");
    LinearRegression regression((double**) &data, VOLTAGE_STEPS);

    Serial.print("Model: V = ");
    Serial.print(regression.getSlope());
    Serial.print("x + ");
    Serial.println(regression.getIntercept());
}

#endif