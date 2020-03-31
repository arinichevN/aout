#ifndef MODEL_SERVO_H
#define MODEL_SERVO_H

#include <Servo.h>

typedef struct {
	Servo device;
	int pin;
	unsigned long pw_min;//pulse width, microseconds
	unsigned long pw_max;//pulse width, microseconds
	double in_min;
	double in_max;
	double last_goal;
} Servow;

extern void servo_control(Servow *item, double goal);
extern void servo_setParam(Servow *item, unsigned long pw_min, unsigned long pw_max, double in_min, double in_max);
extern void servo_setPin(Servow *item, int pin);
extern void servo_begin (Servow *item);
extern void servo_stop(Servow *item);
extern int servo_normalizeInput(Servow *item, double *v);

#endif 
