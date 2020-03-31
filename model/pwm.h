#ifndef MODEL_PWM_H
#define MODEL_PWM_H

#define PWM_PIN_BUSY LOW
#define PWM_PIN_IDLE HIGH

enum PWMstateE {
  PWM_BUSY,
  PWM_IDLE
};

typedef struct {
	int pin;
    PWMstateE state;
    unsigned long delay_busy;
    unsigned long delay_idle;
    unsigned long toggle_time;//time when we must change state from busy to idle or reverse
    double single_time;//(duty_cycle_max - duty_cycle_min)/resolution
    
    unsigned long resolution;//0 is 0% of power (duty_cycle will be set to PWM.duty_cycle_min) PWM.resolution is 100% of power (duty_cycle will be set to PWM.duty_cycle_max)
    unsigned long period;
    unsigned long duty_cycle_min; //0% of power
    unsigned long duty_cycle_max;//100% of power
    unsigned long duty_cycle;
} PWM;

extern int pwm_control(PWM *item, double goal);
extern void pwm_setParam(PWM *item, unsigned long resolution, unsigned long period, unsigned long duty_cycle_min, unsigned long duty_cycle_max);
extern void pwm_setPin(PWM *item, int pin);
extern void pwm_begin(PWM *item, int pin);
extern void pwm_stop(PWM *item);
extern int pwm_normalizeInput(PWM *item, double *v);

#endif 
