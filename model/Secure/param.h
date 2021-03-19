#ifndef MODEL_SECURE_PARAM_H
#define MODEL_SECURE_PARAM_H

#include "../../lib/common.h"

typedef struct {
	double out;
	unsigned long timeout;
	yn_t enable;
} SecureParam;

extern err_t secureParam_check(const SecureParam *self);

#endif 
