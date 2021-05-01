#include "main.h"

/*
 * -user_config:
 * put default parameters for each channel in this array
 * ChannelParam structure members:
 * id,
 * pin,
 * device_kind,
 * {spwm_resolution,
 * spwm_period,
 * spwm_duty_cycle_min, 
 * spwm_duty_cycle_max},
 * {hpwm_pw_min,
 * hpwm_pw_max,
 * hpwm_in_min,
 * hpwm_in_max},
 * {secure_out,
 * secure_timeout,
 * secure_enable}
 */
const ChannelParam CHANNEL_DEFAULT_PARAMS[] = {
	{11,  3, DEVICE_KIND_SPWM, {544UL, 2400UL, 0.0, 1000.0}, {1000UL, 5000UL, 0UL, 5000UL}, {0.0, 3000, YES}},
	{12,  4, DEVICE_KIND_SPWM, {544UL, 2400UL, 0.0, 1000.0}, {1000UL, 5000UL, 0UL, 5000UL}, {0.0, 3000, YES}},
	{13,  5, DEVICE_KIND_SPWM, {544UL, 2400UL, 0.0, 1000.0}, {1000UL, 5000UL, 0UL, 5000UL}, {0.0, 3000, YES}},

	
	//{21,  3, DEVICE_KIND_SPWM, {544UL, 2400UL, 0.0, 1000.0}, {1000UL, 5000UL, 0UL, 5000UL}, {0.0, 3000, YES}},
	//{22,  4, DEVICE_KIND_SPWM, {544UL, 2400UL, 0.0, 1000.0}, {1000UL, 5000UL, 0UL, 5000UL}, {0.0, 3000, YES}},
	//{23,  5, DEVICE_KIND_SPWM, {544UL, 2400UL, 0.0, 1000.0}, {1000UL, 5000UL, 0UL, 5000UL}, {0.0, 3000, YES}},

};

#define CHANNEL_COUNT (sizeof CHANNEL_DEFAULT_PARAMS / sizeof CHANNEL_DEFAULT_PARAMS[0])

Channel channel_buf[CHANNEL_COUNT];

ChannelLList channels = LLIST_INITIALIZER;

int channels_activeExists(){
	return channelLList_activeExists(&channels);
}

int channels_getIdFirst(int *out){
	return channelLList_getIdFirst(&channels, out);
}

int channels_begin(int default_btn){
	LLIST_BUILD_FROM_ARRAY_N(channels, channel_buf, CHANNEL_COUNT)
	size_t ind = 0;
	FOREACH_CHANNEL {
		if(default_btn == BUTTON_DOWN){
			pmem_saveChannelParam(&CHANNEL_DEFAULT_PARAMS[ind], ind);
			printd("default param has been saved for channel ind \n"); printdln(ind);
		}
		int r = channel_begin(channel, ind); ind++;
		if(!r){
			return 0;
		}
	}
	return 1;
}

#ifdef USE_AOIDS
void channels_buildAoids(Aoid *next_oid, Aoid *parent_oid, size_t *id){
	return channelLList_buildAoids(&channels, next_oid, parent_oid, id);
}
#endif

#ifdef USE_NOIDS
Noid *channels_buildNoids(Noid *prev){
	return channelLList_buildNoids(&channels, prev);
}
#endif
