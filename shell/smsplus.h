#ifndef _SMSPLUS_H
#define _SMSPLUS_H

typedef struct {
	int video_scale;
	int audio_rate;
	int audio_fm;
	int audio_fmtype;
	int misc_region;
} settings_t;

void smsp_state(int slot, int mode);

#endif
