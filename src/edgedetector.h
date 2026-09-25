#ifndef __EDGEDETECTOR_H_
#define __EDGEDETECTOR_H_

class EdgeDetector {
public:
    bool get();
    bool fallen();
    bool risen();
    bool changed();

    void update(bool current_state);

private:
    bool m_current_state = false;
    bool m_prev_state = false;
};

#endif