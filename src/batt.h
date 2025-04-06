#ifndef __BATT_H__
#define __BATT_H__

#include <Arduino.h>

#include "util.h"
#include "config.h"

double battery_voltage() {
    return double_map((double) analogRead(VOLTAGE_DIVIDER_PIN), 0.0, 4096.0, 0, 3.3) * (1.0/VOLTAGE_DIVIDER_RATIO);
}

#endif