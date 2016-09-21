
#ifndef _UI_H_
#define _UI_H_

#define MSG_LEN     1024

/* Global data */
extern int fg;
extern int bg;
extern char msg[MSG_LEN];
extern volatile int msg_enable;

/* Function prototypes */
void check_ui_keys(void);
void msg_print(int x, int y, char *fmt, ...);
void add_msg(char *fmt, ...);

#endif /* _UI_H_ */
