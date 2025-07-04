#ifndef __UTIL_H__
#define __UTIL_H__

/// Equivalent to the Arduino `map()` function, but for doubles instead of ints.
double double_map(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif