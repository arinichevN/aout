#include "servo.h"

static int servo_map(double x, double in_min, double in_max,  unsigned long out_min,  unsigned long out_max){
  return (int) (x - in_min) * ((double)out_max - (double)out_min) / (in_max - in_min) + (double)out_min;
}

void servo_control(Servow *item, double goal){
	if(item->last_goal == goal) return;
	int tms = servo_map(goal, item->in_min, item->in_max, item->pw_min, item->pw_max);
	item->device.writeMicroseconds(tms);printd(" servo pw: ");printd(tms);printd("\n");
	item->last_goal = goal;
}

void servo_setParam(Servow *item, unsigned long pw_min, unsigned long pw_max, double in_min, double in_max){
	item->pw_min = pw_min;
	item->pw_max = pw_max;
	item->in_min = in_min;
	item->in_max = in_max;
}

void servo_setPin(Servow *item, int pin){
	item->pin = pin;
}

int servo_getPin(Servow *item){
	return item->pin;
}

void servo_begin (Servow *item){
	item->last_goal = -0.10203;
	item->device.attach(item->pin);
}

void servo_stop(Servow *item){
	item->device.detach();
}

int servo_normalizeInput(Servow *item, double *v){
	if(*v > item->in_max) {
		*v = item->in_max;
		return 1;
	}
	if(*v < item->in_min) {
		*v = item->in_min;
		return 1;
	}
	return 0;
}
