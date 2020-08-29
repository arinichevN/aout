#ifndef MODEL_SECURE_H
#define MODEL_SECURE_H

/*
 * this block will set output to certain value when there are
 * no set goal requests from master
 * 
 */


#include "../util/ton.h"
#include "../util/common.h"
#include "../acp/command/main.h"

typedef struct secure_st{
    double out;
    int enable;

    Ton tmr;
    void (*control) (struct secure_st *, double *);
} Secure;

extern const char *secure_getStateStr(Secure *item);

extern void secure_begin (Secure *item);

extern void secure_stop (Secure *item) ;

extern void secure_setEnable(Secure *item, int v);

extern void secure_setParam (Secure *item, unsigned long timeout, double out, int enable);

extern void secure_touch (Secure *item);

#define secure_control(ITEM, V) (ITEM)->control(ITEM, (V))

#endif 
