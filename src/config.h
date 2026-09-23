#ifndef __CONFIG_H__
#define __CONFIG_H__

namespace config {
    constexpr int REV_SWITCH_PIN = 12;
    constexpr bool REV_SWITCH_INVERTED = true;

    constexpr int VOLTAGE_DIVIDER_PIN = A2;
    constexpr double VOLTAGE_DIVIDER_RATIO = 22.0 / 122.0;

    constexpr int TACH_A_PIN = 7;
    constexpr int TACH_B_PIN = 10;
    constexpr int EDGES_PER_REVOLUTION = 2;

    constexpr int WHEEL_A_PIN = 11;
    constexpr int WHEEL_B_PIN = 9;

    constexpr int BUZZER_PIN = 5;

    // The adafruit SAMD board support library has a macro called ADC_RESOLUTION. This variable's identifier must be different, hence the leading underscore
    constexpr int _ADC_RESOLUTION = 12;
    constexpr int PWM_RESOLUTION = 12;

    constexpr double WHEEL_KP = 0.0015;
    constexpr double WHEEL_KI = 0.00000001;
    constexpr double WHEEL_KD = 0.0;
    constexpr int WHEEL_INTEGRAL_THRESHOLD = 6000;
}

#endif