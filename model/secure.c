#include "secure.h"

void secure_begin (Secure *item) {
	if(item->enable == YES){
		item->state = INIT;
    }else{
	    item->state = OFF;
    }
}

void secure_stop (Secure *item) {
	item->state = OFF;
}

void secure_setParam (Secure *item, unsigned long timeout, double out, int enable) {
	ton_setInterval(&item->tmr, timeout);
	item->out = out;
	item->enable = enable;
}

void secure_touch (Secure *item) {
	item->state = INIT;
}

const char *secure_getStateStr(Secure *item){
	switch(item->state){
		case INIT:return "INI";
		case DO:return "SEC";
		case OFF:return "OFF";
		case WAIT:return "WAIT";
	}	
	return " ? ";
}

void secure_setEnable(Secure *item, int v){
	if(v == YES){
		if(item->state == OFF){
			item->state = INIT;
		}
    }else{
	    item->state = OFF;
    }
}

void secure_control (Secure *item, double *v) {
	switch(item->state){
		case WAIT:
			//printd("sec WAIT ");
			if(ton(&item->tmr)){
				item->state = DO;
				*v = item->out;
			}
			break;
	    case OFF:
		   //printdln("sec OFF");
		    break;
		case DO:
			//printdln("sec DO");
			*v = item->out;
			break;
		case INIT:
			printdln("sec INIT");
			ton_reset(&item->tmr);
		    if(item->enable == YES){
				item->state = WAIT;
		    }else{
			    item->state = OFF;
		    }
			break;
		}
}
