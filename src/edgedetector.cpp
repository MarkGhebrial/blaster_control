#include "edgedetector.h"

bool EdgeDetector::get() {
    return m_current_state;
}

bool EdgeDetector::fallen() {
    return !m_current_state && m_prev_state;
}

bool EdgeDetector::risen() {
    return m_current_state && !m_prev_state;
}

bool EdgeDetector::changed() {
    return m_current_state != m_prev_state;
}

void EdgeDetector::update(bool current_state) {
    m_prev_state = m_current_state;
    m_current_state = current_state;
}