#ifndef MODEL_HPWM_H
#define MODEL_HPWM_H

#include <Servo.h>
#include "../interface/iDevice.h"
#include "param.h"

typedef struct {
	Servo device;
	int pin;
	unsigned long pw_min;//pulse width, microseconds
	unsigned long pw_max;//pulse width, microseconds
	double in_min;
	double in_max;
	double last_goal;
	iDevice im_device;
} Hpwm;

extern void hpwm_setParam(Hpwm *self, const HpwmParam *param);
extern void hpwm_setPin (Hpwm *self, int pin);

#endif 
