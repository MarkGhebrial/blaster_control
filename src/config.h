#define REV_SWITCH_PIN 12

#define VOLTAGE_DIVIDER_PIN A2
#define VOLTAGE_DIVIDER_RATIO (22.0/122.0)

#define TACH_A_PIN 7
#define TACH_B_PIN 10
#define EDGES_PER_REVOLUTION 2

#define WHEEL_A_PIN 11
#define WHEEL_B_PIN 9

#define ADC_RESOLUTION 12
#define PWM_RESOLUTION 12

#define WHEEL_KP 0.002
#define WHEEL_KI 0.000000005
#define WhEEL_KD 0.003
// TODO: 10000 rpm is a temporary fix to account for innacurate feedforward on motor b. Set this back to 4000 when that issue is resolved.
#define WHEEL_INTEGRAL_THRESHOLD 10000