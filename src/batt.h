#ifndef __BATT_H__
#define __BATT_H__

#include <Arduino.h>

#include "util.h"
#include "config.h"

double battery_voltage() {
    return double_map((double) analogRead(VOLTAGE_DIVIDER_PIN), 0.0, 4096.0, 0, 3.3) * (1.0/VOLTAGE_DIVIDER_RATIO);
}

/**
 * Use the battery voltage to calculate the number of series cells in the battery.
 */
int cell_count() {
    int cell_count = 0;
    double voltage = battery_voltage();
    while (4.3 * cell_count < voltage) {
        cell_count++;
    }
    return cell_count;
}

bool is_battery_low(int cell_count, double cell_voltage_threshold = 3.5) {
    return battery_voltage() <= cell_count * cell_voltage_threshold;
}

#endif