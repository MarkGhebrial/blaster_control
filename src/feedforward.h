#ifndef __FEEDFORWARD_H__
#define __FEEDFORWARD_H__

// #include <Arduino.h>

#define VOLTAGE_STEPS 11

class FeedForwardModel {
    public:
        virtual double compute(double setpoint) {
            return 0.0;
        }
};

/**
 * A model of the form 0.001 * a * b^(value * 0.001) + c;
 */
class ExponentialFeedForwardModel : public FeedForwardModel {
    public:
        /**
         * a is the coefficient. b is the base of the exponent. c is the up/down y shift.
         */
        ExponentialFeedForwardModel(double a, double b, double c) {
            this->a = a;
            this->b = b;
            this->c = c;
        }

        double compute(double value) override {
            return 0.001 * a * pow(b, value * 0.001) + c;
        }

    private:
        double a, b, c;
};

#endif