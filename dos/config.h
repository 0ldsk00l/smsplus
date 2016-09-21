
#ifndef _CONFIG_H_
#define _CONFIG_H_

typedef struct
{
    int joy_driver;

    int video_driver;
    int video_depth;
    int video_width;
    int video_height;

    int no_vga;
    int no_mmx;
    int expand;
    int blur;
    int scale;
    int scanlines;
    int tweak;

    int vsync;
    int throttle;
    int fps;

    int sound;
    int sndcard;
    int sndrate;

    int country;
    int fm_enable;
    int codies;
}t_option;

/* Global data */
t_option option;

/* Function prototypes*/
void do_config(char *file);
void parse_args(int argc, char **argv);
int parse_file(const char *filename, int *argc, char **argv);
void set_option_defaults(void);

#endif /* _CONFIG_H_ */
