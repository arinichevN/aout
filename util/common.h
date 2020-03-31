#ifndef COMMON_H
#define COMMON_H

#define STATE_STR_MAX_LENGTH 16
#define ACTIVE_STATE_STR "RUN"
#define UNACTIVE_STATE_STR "OFF"

enum StateE {
	INIT=0,
	OFF,
	FAILURE,
	DISABLE,
	RESET, 
	
	DONE,
	RUN,
	DO,
	IDLE,
	BUSY,
	OUT_MAX,
	OUT_MIN,
	WAIT,
	WAIT_OUT,
	WAIT_HIGH,
	WAIT_LOW,
	WAIT_GAP,
	BLOCK,
	LEVEL_HIGH,
    LEVEL_LOW,
	
	WAIT_IN,
	WAIT_DELAY,
	
	COMMAND,
	LISTEN,
	
	SLAVE_START,
	SLAVE_STOP,
	
	SKIP,
	ON,
	EDIT,
	MOVE, 
	DSTEP1,
	DSTEP2,
};

enum ErrorE {
	ERROR_FIRST = 1,
	
	WARNING_READ,
	WARNING_BAD_DATA,
	ERROR_SUBBLOCK,
	ERROR_BLOCK_STATUS,
	ERROR_GOAL,
	ERROR_SAVE_GOAL,
	ERROR_METHOD,
	ERROR_OUT,
	ERROR_PID_MODE,
	ERROR_PID_KP,
	ERROR_PID_KI,
	ERROR_PID_KD,
	ERROR_POS2_MODE,
	ERROR_POS2_HYS,
	ERROR_PMEM_READ,
	ERROR_READ,
	ERROR_CHANNELS,
	ERROR_SERVER_NODES,
	ERROR_SERIAL,
	ERROR_SERIAL_DEVICE,
	ERROR_SERIAL_RATE,
	ERROR_SERIAL_CONFIG,
	ERROR_SERIAL_KIND,
	ERROR_SERIAL_BEGIN,
	ERROR_NO_SERIAL,
	ERROR_SEND,
	ERROR_BAD_RESULT,
	ERROR_PATIENCE_LOST,
	ERROR_SLAVE_START,
	ERROR_SLAVE_STOP,
	ERROR_RETRY,
	
	ERROR_COUNT
};

#define BUTTON_DOWN 0
#define BUTTON_UP 1

#define ERROR_NO 0
typedef enum {
	NO=0,
	YES=1
} EnableE;

struct timespec{
	unsigned long tv_sec;
	unsigned long tv_nsec;
};

typedef struct {
	double value;
	struct timespec tm;
	int state;
} FTS;

#define ARRLEN(A) (sizeof(A) / sizeof((A)[0]))

#define PERCENT_MAX 100

#define MIN_IND 0UL

#define FLOAT_SURROGATE_FORMAT "%d.%.3d"
#define FLOAT_SURROGATE_DEF(W, F, V) int W = (int)V;	int F = (int)((V - (double)W)*1000);

extern void snprinttime(unsigned long v, char *buf, size_t len);

extern long int getSerialRate (uint8_t kind);

extern uint8_t getSerialRateKind (long int v);

extern long int getSerialConfig(uint8_t kind);

extern uint8_t getSerialConfigKind(long int v);

extern double tsToDouble(struct timespec v);

extern const char *getErrorStr(int v);

#endif 
