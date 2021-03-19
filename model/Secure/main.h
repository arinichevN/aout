#ifndef MODEL_SECURE_H
#define MODEL_SECURE_H

/*
 * this block will set output to certain value when there are
 * no set goal requests from master
 * 
 */

#include "../../lib/ton.h"
#include "../../lib/common.h"
#include "param.h"

typedef struct secure_st Secure;
struct secure_st {
	double out;
	yn_t enable;
	Ton tmr;
	void (*control) (Secure *, double *);
};

extern const char *secure_getStateStr(Secure *self);
extern void secure_begin (Secure *self);
extern void secure_start (Secure *self);
extern void secure_stop (Secure *self);
extern void secure_setEnable(Secure *self, int v);
extern void secure_setParam (Secure *self, const SecureParam *param);
extern void secure_touch (Secure *self);

#define SECURE_CONTROL(ITEM, V) (ITEM)->control(ITEM, (V))

#endif 
