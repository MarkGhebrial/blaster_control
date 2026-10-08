#ifndef __DEBOUNCE_H__
#define __DEBOUNCE_H__

#include "timer.h"
#include <functional>

/// @brief Debounce a signal by adding some latency for 
/// @tparam T 
template <typename T>
class Debounce {
public:
    explicit Debounce(T signal);

    bool get();

private:
    bool m_state = false;
    T m_signal;
    Timer m_timer;
};

template <typename T>
Debounce<T>::Debounce(T signal)
 : m_signal(std::move(signal))
{
}

// template <typename T>
// Debounce::Debounce(T signal)
//  : m_signal(std::move(signal))
// {}

void foo() {
    Debounce<std::function<bool()>> button([](){return true;});

    if (button) {
        // Yayyyyyy
    }
}

#endif