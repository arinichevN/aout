#ifndef MODEL_SPWM_H
#define MODEL_SPWM_H

#include "../interface/iDevice.h"
#include "param.h"

#define SPWM_PIN_BUSY LOW
#define SPWM_PIN_IDLE HIGH

typedef struct spwm_st{
	int pin;
	void (*control) (struct spwm_st *, double );
	unsigned long delay_busy;
	unsigned long delay_idle;
	unsigned long toggle_time;//time when we must change state from busy to idle or reverse
	double single_time;//(duty_cycle_max - duty_cycle_min)/resolution
	
	unsigned long resolution;//0 is 0% of power (duty_cycle will be set to PWM.duty_cycle_min) PWM.resolution is 100% of power (duty_cycle will be set to PWM.duty_cycle_max)
	unsigned long period;
	unsigned long duty_cycle_min; //0% of power
	unsigned long duty_cycle_max;//100% of power
	unsigned long duty_cycle;
	iDevice im_device;
} Spwm;

extern void spwm_setParam(Spwm *self, const SpwmParam *param);
extern void spwm_setPin(Spwm *self, int pin);
extern int spwm_getPin(Spwm *self);

#endif 
