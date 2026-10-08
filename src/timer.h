#ifndef __TIMER_H__
#define __TIMER_H__

#include <cstdint>


class Timer {
public:
    explicit Timer();

    void reset();

    uint32_t elapsed_micros();

private:
    uint32_t m_start_time;
};

#endif