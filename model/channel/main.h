#ifndef MODEL_CHANNEL_H
#define MODEL_CHANNEL_H

#if OUTPUT_MODE == OUT_PWM
#include "../pwm.h"
#else
#if OUTPUT_MODE == OUT_SERVO
#include "../servo.h"
#endif
#endif

#include "../../util/dstructure.h"
#include "../../util/common.h"
#include "../secure.h"

#include "../../app/serial.h"


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
	int device_kind;
	int error_id;
	int enable;
	void (*control) (struct channel_st *);
	struct channel_st *next;
};
typedef struct channel_st Channel;

DEC_LLIST(Channel)

#define FOREACH_CHANNEL(LIST) FOREACH_LLIST(channel, LIST, Channel)
#define CHANNEL_SAVE_FIELD(F) ChannelParam pchannel; if(pmem_getPChannel(&pchannel, item->ind)){pchannel.F = item->F; pmem_savePChannel(&pchannel, item->ind);}
#define CHANNEL_FUN_GET(param) channel_get_ ## param

extern const char *channel_getErrorStr(Channel *item);
extern const char *channel_getStateStr(Channel *item);
extern void channel_begin(Channel *item, size_t ind, int default_btn);
extern void channels_buildFromArray(ChannelLList *channels, Channel arr[]);
extern void channel_free(Channel *item);
extern int channel_start(Channel *item);
extern int channel_stop(Channel *item);
extern int channel_disconnect(Channel *item);
extern int channel_reset(Channel *item);
extern int channels_activeExists(ChannelLList *channels);
extern void channel_setGoal(Channel *item, double v);

extern int CHANNEL_FUN_GET(enable)(Channel *item);
extern int CHANNEL_FUN_GET(device_kind)(Channel *item);
extern int CHANNEL_FUN_GET(pin)(Channel *item);
extern int channels_getIdFirst(ChannelLList *channels, int *out);
extern const char *channel_getSecureStateStr(Channel *item);
extern double CHANNEL_FUN_GET(goal)(Channel *item);

#if OUTPUT_MODE == OUT_PWM
extern unsigned long CHANNEL_FUN_GET(pwm_period)(Channel *item);
extern unsigned long CHANNEL_FUN_GET(pwm_resolution)(Channel *item);
extern unsigned long CHANNEL_FUN_GET(pwm_duty_cycle_min)(Channel *item);
extern unsigned long CHANNEL_FUN_GET(pwm_duty_cycle_max)(Channel *item);
#else
#if OUTPUT_MODE == OUT_SERVO
extern unsigned long CHANNEL_FUN_GET(servo_pw_min)(Channel *item);
extern unsigned long CHANNEL_FUN_GET(servo_pw_max)(Channel *item);
extern double CHANNEL_FUN_GET(servo_in_min)(Channel *item);
extern double CHANNEL_FUN_GET(servo_in_max)(Channel *item);
#endif
#endif

extern double CHANNEL_FUN_GET(secure_out)(Channel *item);
extern unsigned long CHANNEL_FUN_GET(secure_timeout)(Channel *item);
extern int CHANNEL_FUN_GET(secure_enable)(Channel *item);

#endif 
