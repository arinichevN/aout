#ifndef MODEL_CHANNEL_H
#define MODEL_CHANNEL_H

#if OUTPUT_MODE == OUT_PWM
#include "pwm.h"
#else
#if OUTPUT_MODE == OUT_SERVO
#include "servo.h"
#endif
#endif

#include "../util/dstructure.h"
#include "../util/common.h"
#include "secure.h"

#include "../app/serial.h"

/*
 * -user_config:
 * set number of channels you want to use:
 */
#define CHANNEL_COUNT 3

/*
 * -user_config:
 * here you can set some default parameters for
 * channels
 */
#define DEFAULT_CHANNEL_FIRST_ID		21
#define DEFAULT_PWM_RESOLUTION			1000UL
#define DEFAULT_PWM_PERIOD_MS			5000UL
#define DEFAULT_PWM_DUTY_CYCLE_MIN_MS	0UL
#define DEFAULT_PWM_DUTY_CYCLE_MAX_MS	5000UL
#define DEFAULT_SERVO_PW_MIN			544
#define DEFAULT_SERVO_PW_MAX			2400
#define DEFAULT_SERVO_IN_MIN			0.0
#define DEFAULT_SERVO_IN_MAX			1000.0
#define DEFAULT_SECURE_TIMEOUT_MS		3000UL
#define DEFAULT_SECURE_OUT				0UL
#define DEFAULT_SECURE_ENABLE			NO
#define DEFAULT_CHANNEL_ENABLE			YES
#define DEFAULT_CHANNEL_GOAL			0UL


	
enum ChannelErrorE {
	CHANNEL_ERROR_BLOCK_STATUS = 1,
	CHANNEL_ERROR_SECURE_BLOCK_STATUS,
	CHANNEL_ERROR_SUBBLOCK,
	CHANNEL_ERROR_PMEM_READ,
	CHANNEL_ERROR_PWM_DC_MINMAX,
	CHANNEL_ERROR_PWM_DCMAX_PERIOD,
	CHANNEL_ERROR_SERVO_PW_MINMAX,
	CHANNEL_ERROR_SERVO_IN_MINMAX,
	CHANNEK_ERROR_COUNT
};

struct channel_st {
#if OUTPUT_MODE == OUT_PWM
	PWM pwm;
#else
#if OUTPUT_MODE == OUT_SERVO
	Servow servo;
#endif
#endif
	Secure secure;
	int id;//for external control
	size_t ind;//index in array
	double goal;
	int error_id;
	int enable;
	int state;
	struct channel_st *next;
};
typedef struct channel_st Channel;

DEC_LLIST(Channel)

#define FOREACH_CHANNEL(LIST) FOREACH_LLIST(channel, LIST, Channel){
	

#endif 
