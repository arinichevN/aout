#include "secure.h"

void secure_DO (Secure *item, double *v){
	*v = item->out;
}

void secure_WAIT (Secure *item, double *v){
	if(ton(&item->tmr)){
		item->control = secure_DO;
		*v = item->out;
	}
}

void secure_OFF (Secure *item, double *v){
	;
}

void secure_INIT (Secure *item, double *v){
	ton_reset(&item->tmr);
	item->control = secure_OFF;
    if(item->enable == YES){
		item->control = secure_WAIT;
    }
}

void secure_begin (Secure *item) {
	item->control = secure_OFF;
	if(item->enable == YES){
		item->control = secure_INIT;
    }
}

void secure_stop (Secure *item) {
	item->control = secure_OFF;
}

void secure_setParam (Secure *item, unsigned long timeout, double out, int enable) {
	ton_setInterval(&item->tmr, timeout);
	item->out = out;
	item->enable = enable;
}

void secure_touch (Secure *item) {
	item->control = secure_INIT;
}

const char *secure_getStateStr(Secure *item){
	if(item->control == secure_INIT)		return "INI";
	else if(item->control == secure_DO)		return "SEC";
	else if(item->control == secure_OFF)	return "OFF";
	else if(item->control == secure_WAIT)	return "WAIT";
	return " ? ";
}

void secure_setEnable(Secure *item, int v){
	if(v == YES){
		if(item->control == secure_OFF){
			item->control = secure_INIT;
		}
    }else{
	    item->control = secure_OFF;
    }
}

//void secure_control (Secure *item, double *v) {
	//switch(item->state){
		//case WAIT:
			////printd("sec WAIT ");
			//if(ton(&item->tmr)){
				//item->control = secure_DO;
				//*v = item->out;
			//}
			//break;
	    //case OFF:
		   ////printdln("sec OFF");
		    //break;
		//case DO:
			////printdln("sec DO");
			//*v = item->out;
			//break;
		//case INIT:
			//printdln("sec INIT");
			//ton_reset(&item->tmr);
		    //if(item->enable == YES){
				//item->control = secure_WAIT;
		    //}else{
			    //item->control = secure_OFF;
		    //}
			//break;
		//}
//}
