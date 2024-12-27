#ifndef __LINEAR_REGRESSION_H__
#define __LINEAR_REGRESSION_H__

#include <Arduino.h>

class LinearRegression {
    private:
        double m; //slope
        double b; //y-intercept

    public:
        /**
         * Compute m and b.
         * 
         * <p>I copied this code from https://algs4.cs.princeton.edu/code/edu/princeton/cs/algs4/LinearRegression.java.html
         * and don't understand any of it, but it seems to work great!
         * 
         * @param data a 2D array of x and y coordinates
         */
        LinearRegression (double** data, uint32_t len) {
            // int n = data.length;

            // first pass
            double sumx = 0.0, sumy = 0.0;
            for (uint32_t i = 0; i < len; i++) {
                sumx  += data[i][0];
                sumy  += data[i][1];
            }
            double xbar = sumx / len;
            double ybar = sumy / len;

            // second pass: compute summary statistics
            double xxbar = 0.0, xybar = 0.0;
            for (uint32_t i = 0; i < len; i++) {
                xxbar += (data[i][0] - xbar) * (data[i][0] - xbar);
                xybar += (data[i][0] - xbar) * (data[i][1] - ybar);
            }
            this->m = xybar / xxbar;
            this->b = ybar - m * xbar;
        }

        double evaluate (double x) {
            return m*x + b;
        }

        double getSlope () {
            return m;
        }

        double getIntercept () {
            return b;
        }
};

#endif