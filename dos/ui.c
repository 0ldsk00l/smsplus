
#include "osd.h"

int fg                       = 0;
int bg                       = 0;
char msg[MSG_LEN];
volatile int msg_enable      = 0;

/* Check user interface keys */
void check_ui_keys(void)
{
    if(check_key(KEY_TILDE))
    {
        static const char *msg[] = {"EMU2413", "YM2413"};

        if(snd.fm_which == SND_EMU2413)
            snd.fm_which = SND_YM2413;
        else
            snd.fm_which = SND_EMU2413;

        add_msg("Using %s FM sound emulator.", msg[snd.fm_which]);
        sound_init();
    }

    /* Frame skip keys */
    if(check_key(KEY_F1)) { frame_skip = 1; add_msg("Frame skip disabled"); };
    if(check_key(KEY_F2)) { frame_skip = 2; add_msg("Frame skip set to 1"); };
    if(check_key(KEY_F3)) { frame_skip = 3; add_msg("Frame skip set to 2"); };
    if(check_key(KEY_F4)) { frame_skip = 4; add_msg("Frame skip set to 3"); };

    /* State save/load keys */
    if(check_key(KEY_F5)) { if(save_state()) add_msg("State saved to slot %d", state_slot); };
    if(check_key(KEY_F6)) { state_slot = (state_slot + 1) % 10; add_msg("Slot %d", state_slot); };
    if(check_key(KEY_F7)) { if(load_state()) add_msg("State loaded from slot %d", state_slot); };

    /* Screen snapshot */
    if(check_key(KEY_F8))
    {
        char name[PATH_MAX];
        PALETTE snap_pal;
        BITMAP *snap_bmp = create_bitmap_ex(option.video_depth, bitmap.viewport.w, bitmap.viewport.h);
        if(snap_bmp)
        {
            int i;

            /* Get current palette */
            get_palette(snap_pal);

            /* Remove unused palette entries */
            for(i = 0x20; i < 0xFE; i++)
                snap_pal[i].r = snap_pal[i].g = snap_pal[i].b = 0x00;
            
            /* Clip image */
            blit(sms_bmp, snap_bmp, bitmap.viewport.x, bitmap.viewport.y, 0, 0, bitmap.viewport.w, bitmap.viewport.h);

            /* Generate file name */
            sprintf(name, "snap%04d.pcx", snap_count);

            /* Remove unused bits */
            if(option.video_depth == 8)
            {
                int x, y;
                for(y = 0; y < bitmap.viewport.h; y++)
                for(x = 0; x < bitmap.viewport.w; x++)
                    putpixel(snap_bmp, x, y, getpixel(snap_bmp, bitmap.viewport.x + x, bitmap.viewport.y + y) & PIXEL_MASK);
            }

            /* Save snapshot in PCX format */
            save_pcx(name, snap_bmp, snap_pal);

            /* Deallocate temporary bitmap */
            destroy_bitmap(snap_bmp);

            /* Bump snapshot counter */
            snap_count = (snap_count + 1) & 0xFFFF;

            /* Display results */
            add_msg("Saved screen to %s", name);
        }
    }


    /* FPS meter toggle */
    if(check_key(KEY_F9))
    {
        option.fps ^= 1;
        add_msg("FPS meter %s", option.fps ? "on" : "off");
    }

    /* Speed throttling */
    if(check_key(KEY_F10))
    {
        if(!option.sound)
        {
            option.throttle ^= 1;
            add_msg("Speed throttling %s", option.throttle ? "on" : "off");
        }
    }

    /* Speed throttling */
    if(check_key(KEY_F11))
    {
        option.vsync ^= 1;
        add_msg("VSync %s", option.vsync ? "on" : "off");
    }

    /* Exit keys */
    if(key[KEY_ESC] || key[KEY_END]) running = 0;
}


void msg_print(int x, int y, char *fmt, ...)
{
    int i = bitmap.viewport.x;
    int j = bitmap.viewport.y;
    va_list ap;
    char token[MSG_LEN];
    char str[MSG_LEN];

    strcpy(str, "\0");
    va_start(ap, fmt);
    vsprintf(token, fmt, ap);
    strcat(str, token);
    va_end(ap);

    textprintf_ex(sms_bmp, font, i+x+1, j+y  , bg, -1, "%s", str);
    textprintf_ex(sms_bmp, font, i+x-1, j+y  , bg, -1, "%s", str);
    textprintf_ex(sms_bmp, font, i+x  , j+y+1, bg, -1, "%s", str);
    textprintf_ex(sms_bmp, font, i+x  , j+y-1, bg, -1, "%s", str);
    textprintf_ex(sms_bmp, font, i+x  , j+y  , fg, -1, "%s", str);
}


void add_msg(char *fmt, ...)
{
    va_list ap;
    char token[MSG_LEN];

    msg_enable = FRAMES_PER_SECOND;

    strcpy(msg, "\0");
    va_start(ap, fmt);
    vsprintf(token, fmt, ap);
    strcat(msg, token);
    va_end(ap);
}




