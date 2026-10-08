#include "timer.h"
#include "Arduino.h"

Timer::Timer()
    : m_start_time(micros())
{}

void Timer::reset() {
    m_start_time = micros();
}

uint32_t Timer::elapsed_micros() {
    return micros() - m_start_time;
}