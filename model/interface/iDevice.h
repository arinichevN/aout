#ifndef MODEL_IDEVICE_H
#define MODEL_IDEVICE_H

typedef struct {
	void *self;
	void (*begin)(void *);
	void (*control)(void *, double);
	int (*normalizeInput)(void *, double *);
	double (*getInitialGoal)(void *);
	void (*start)(void *);
	void (*stop)(void *);
	void (*free)(void *);
} iDevice;

#endif 
