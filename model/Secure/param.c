#include "param.h"

err_t secureParam_check(const SecureParam *self){
	if(!checkBlockStatus(self->enable)){
		return ERROR_BLOCK_STATUS;
	}
	return ERROR_NO;
}
