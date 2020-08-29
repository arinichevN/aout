#ifndef MODEL_CHANNEL_PARAM_H
#define MODEL_CHANNEL_PARAM_H


typedef struct {
	int id;
    int enable;
    int pin;
#if OUTPUT_MODE == OUT_PWM
	unsigned long pwm_resolution;
    unsigned long pwm_period;
    unsigned long pwm_duty_cycle_min; 
    unsigned long pwm_duty_cycle_max;
#else
#if OUTPUT_MODE == OUT_SERVO
	unsigned long servo_pw_min;
	unsigned long servo_pw_max;
	double servo_in_min;
	double servo_in_max;
#endif
#endif
    double secure_out;
    unsigned long secure_timeout;
    int secure_enable;
} ChannelParam;

#define CHANNEL_DEFAULT_PARAMS chdefps
#endif 
