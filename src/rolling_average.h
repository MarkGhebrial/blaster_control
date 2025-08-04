#ifndef __ROLLING_AVERAGE_H__
#define __ROLLING_AVERAGE_H__

#include <cstddef>

template <class T, int N>
class RollingAverage {
    public:
        RollingAverage() {
            this->current_index = 0;
        }

        RollingAverage(T initial_value) {
            this->current_index = 0;
            for (int i = 0; i < N; i++) {
                this->buffer[i] = initial_value;
            }
        }

        void update(T new_value) {
            this->buffer[current_index] = new_value;
            current_index = (current_index + 1) % N;
        }

        T get_average() {
            T min_outlier = this->buffer[0];
            T max_outlier = this->buffer[0];

            T sum = 0;
            for (int i = 0; i < N; i++) {
                // Reject min and max outliers
                // TODO: Make outlier rejection optional
                if (this->buffer[i] < min_outlier) {
                    min_outlier = this->buffer[i];
                }
                if (this->buffer[i] > max_outlier) {
                    max_outlier = this->buffer[i];
                }
                sum += this->buffer[i];
            }
            return (sum - min_outlier - max_outlier) / (N - 2);
        }

    private:
        T buffer[N] = {0};
        uint32_t current_index = 0;
};

#endif