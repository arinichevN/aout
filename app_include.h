#ifndef APP_INCLUDE_H
#define APP_INCLUDE_H

#include "util/debug.h"
#include "util/common.c"
#include "util/crc.c"
#include "util/check.c"
#include "util/ton.c"
#include "util/serial.c"
#include "pmem/main.c"

#if OUTPUT_MODE == OUT_PWM
#include "model/pwm.c"
#else
#if OUTPUT_MODE == OUT_SERVO
#include "model/servo.c"
#endif
#endif
#include "model/secure.c"
#include "model/channel/default.c"
#include "model/channel/main.c"

#include "acp/main.c"
#include "acp/loop/main.c"

#include "app/error_indicator.c"
#include "app/serial_config.c"
#include "app/config.c"
#include "app/serial.c"
#include "app/main.c"

#ifdef SERIAL_SERVER
#include "app/server_config.c"
#include "acp/loop/server.c"
#endif


#endif 
