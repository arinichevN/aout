#include "pwm.h"

void pwm_calc_single_time (PWM *item) {
   //next lines shortly: item->single_time = ( item->duty_cycle_max - item->duty_cycle_min ) / item->resolution;
   double dif = (double)item->duty_cycle_max - (double)item->duty_cycle_min;
   item->single_time = dif / (double) item->resolution;
}

void pwm_calc_delays(PWM *item){
	unsigned long dc = (unsigned long) (item->single_time * (double)item->duty_cycle);
	item->delay_idle = item->period - dc - item->duty_cycle_min;
	item->delay_busy = dc + item->duty_cycle_min;
}

int pwm_setDutyCycle (PWM *item, unsigned long v) {
	if(v == item->duty_cycle){
		return 0;
	}
	if (v > item->resolution) {
		item->duty_cycle=item->resolution;
	}else{
		item->duty_cycle = v;
	}
	pwm_calc_delays(item);
	//printd("output: ");printdln(v);
	//printd("delay_idle: ");printdln(item->delay_idle);
	//printd("delay_busy: ");printdln(item->delay_busy);printdln(" ");
	//item->toggle_time = millis();
	return 1;
}

void pwm_setParam(PWM *item, unsigned long resolution, unsigned long period, unsigned long duty_cycle_min, unsigned long duty_cycle_max){
	item->resolution = resolution;
	item->period = period;
	item->duty_cycle_min = duty_cycle_min;
	item->duty_cycle_max = duty_cycle_max;
}

void pwm_setPin(PWM *item, int pin){
	item->pin = pin;
	pinMode(item->pin, OUTPUT);
	digitalWrite(item->pin, PWM_PIN_IDLE);
}

void pwm_begin (PWM *item) {
   item->duty_cycle = 0;
   unsigned long  now = millis();
   item->toggle_time = now;
   pwm_calc_single_time (item);
   pwm_calc_delays(item);
   item->state = PWM_IDLE;
}

void pwm_stop(PWM *item){
	digitalWrite(item->pin, PWM_PIN_IDLE);
	item->state = PWM_IDLE;
}

int pwm_normalizeInput(PWM *item, double *v){
	if(*v > (double) item->resolution) {
		*v = (double) item->resolution;
		return 1;
	}
	if(*v < 0.0) {
		*v = 0.0;
		return 1;
	}
	return 0;
}

int pwm_control (PWM *item, double goal) {
	pwm_setDutyCycle (item, (unsigned long) goal);
	unsigned long now = millis();
	switch (item->state) {
	  case PWM_BUSY: {
			if (now > item->toggle_time) {
			   item->toggle_time = now + item->delay_idle;
			   if(item->delay_idle > 0){
				   digitalWrite(item->pin, PWM_PIN_IDLE);
			   }
			   item->state = PWM_IDLE;
			}
			break;
		 }
	  case PWM_IDLE: {
			if (now > item->toggle_time) {
			   item->toggle_time = now + item->delay_busy;
			   if(item->delay_busy > 0){
				   digitalWrite(item->pin, PWM_PIN_BUSY);
			   }
			   item->state = PWM_BUSY;
			}
			break;
		 }
	  default:
		 item->state = PWM_IDLE;
		 break;
	}
	return item->state;
}
