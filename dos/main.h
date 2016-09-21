
#ifndef _MAIN_H_
#define _MAIN_H_

extern volatile int frame_skip;
extern volatile int frame_count;
extern volatile int frames_rendered;
extern volatile int frame_rate;
extern volatile int tick_count;
extern volatile int old_tick_count;
extern volatile int skip;
extern int running;
extern int state_slot;
extern int snap_count;
extern int use_mouse;
extern BITMAP *sms_bmp;
extern PALETTE sms_pal;
extern char game_name[PATH_MAX];



/* Function prototypes */
void tick_handler(void);
int check_key(int code);
void check_ui_keys(void);
void update_audio(void);
void update_video(void);
void update_palette(void);
void system_load_sram(void);
void load_sram(void);
void save_sram(void);
int load_state(void);
int save_state(void);
void msg_print(int x, int y, char *fmt, ...);
void add_msg(char *fmt, ...);

void osd_init(void);
void osd_shutdown(void);
void osd_update_inputs(void);
void osd_update_audio(void);
void osd_update_video(void);

void dump_wram(void);
void dump_vram(void);

#endif /* _MAIN_H_ */
