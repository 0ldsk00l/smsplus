/*
    main.c --
    OSD code for DOS platform.
*/

#include "osd.h"


/* Display timing data */
volatile int frame_skip      = 1;
volatile int frame_count     = 0;
volatile int frames_rendered = 0;
volatile int frame_rate      = 0;
volatile int tick_count      = 0;
volatile int old_tick_count  = 0;
volatile int skip            = 0;
int running                  = 1;
int state_slot               = 0;
int snap_count               = 0;
int use_mouse                = 0;

/* Blitter data */
BITMAP *sms_bmp = NULL;
PALETTE sms_pal;

/* Game file name */
char game_name[PATH_MAX];



int main (int argc, char **argv)
{
    /* Show version information if no arguments are specified */
    if(argc < 2)
    {
        printf("\n%s\n", APP_NAME);
        printf("Copyright (C) Charles MacDonald 1998-2007\n");
        printf("Version %s, build date: %s, %s\n", APP_VERSION, __DATE__, __TIME__);
        printf("Usage: sp <filename.ext> [-options]\n");
        printf("Type 'sp -help' for a summary of the available options\n");
        exit(1);
    }

    /* Show option list */
    if(stricmp(argv[1], "-info") == 0)
    {
        int version = AGetVersion();
        printf("Using library versions:\n");
        printf("* Allegro: `%s'\n", allegro_id);
        printf("* zlib:    %s\n", zlibVersion());
        printf("* SEAL:    %d.%d\n", (version >> 8) & 0xFF, version & 0xFF);
        exit(1);
    }

    /* Show option list */
    if(stricmp(argv[1], "-help") == 0)
    {
        printf("Options:\n");
        printf(" -vdriver <s> \t specify video driver.\n");
        printf(" -res <x> <y> \t set the display resolution.\n");
        printf(" -scale       \t scale display to full resolution. (slow)\n");
        printf(" -expand      \t force 512x384 or 400x300 zoomed display.\n");
        printf(" -nommx       \t disable use of MMX instructions.\n");
        printf(" -novga       \t disable use of VGA vertical scaling with '-expand'.\n");
        printf(" -depth <n>   \t specify color depth. (8, 16)\n");
        printf(" -blur        \t blur display. (16-bit color only)\n");
        printf(" -scanlines   \t use scanlines effect.\n");
        printf(" -tweak       \t force 256x192 or 160x144 tweaked display.\n");
        printf(" -vsync       \t wait for vertical sync before blitting.\n");
        printf(" -throttle    \t limit updates to 60 frames per second.\n");
        printf(" -sound       \t enable sound. (auto-throttle)\n");
        printf(" -sndrate <n> \t specify sound rate. (8000, 11025, 22050, 44100)\n");
        printf(" -sndcard <n> \t specify sound card. (0-7)\n");
        printf(" -joy <s>     \t specify joystick type.\n");
        printf(" -jp          \t use Japanese console type.\n");
        printf(" -fm          \t required to enable YM2413 sound.\n");
        printf(" -info        \t show library versions.\n");
        printf(" -codies      \t force Codemasters mapper.\n");
        exit(1);
    }

    /* Do configuration */
    do_config("sp.cfg");

    /* Make copy of game filename */
    strcpy(game_name, argv[1]);

    /* Attempt to load game off commandline */
    if(load_rom(game_name) == 0)
    {
        printf("Error loading `%s'.\n", game_name);
        exit(1);
    }

    /* Force Codemasters mapper */
    if(option.codies)
    {
        cart.mapper = MAPPER_CODIES;
        sms.territory = TERRITORY_EXPORT;
    }

    /* Set up video, audio, input, etc. */
    osd_init();

    snd.fm_which = SND_EMU2413;
    snd.fps = (sms.display == DISPLAY_NTSC) ? FPS_NTSC : FPS_PAL;
    snd.fm_clock = (sms.display == DISPLAY_NTSC) ? CLOCK_NTSC : CLOCK_PAL;
    snd.psg_clock = (sms.display == DISPLAY_NTSC) ? CLOCK_NTSC : CLOCK_PAL;
    snd.sample_rate = option.sound ? option.sndrate : 0;
    snd.mixer_callback = NULL;

    /* Initialize the virtual console emulation */
    system_init();
    sms.territory = option.country;
    sms.use_fm = option.fm_enable;

    system_poweron();

    pick_blitter_proc();

    if(blitter_proc == NULL)
    {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        printf("Illegal combination of parameters - not all options can be combined.\n");
        exit(1);
    }

    /* Main emulation loop */
    while(running)
    {
        /* Bump frame count */
        frame_count++;
        frames_rendered++;
        skip = (frame_count % frame_skip == 0) ? 0 : 1;

        /* Get current input */
        osd_update_inputs();
        check_ui_keys();

        /* Run the system emulation for a frame */
        system_frame(skip);

        /* Update the display */
        if(skip == 0)
        {
            if(option.sound)
            {
                osd_play_streamed_sample_16(0, snd.output[0], snd.buffer_size, option.sndrate, 60, -100);
                osd_play_streamed_sample_16(1, snd.output[1], snd.buffer_size, option.sndrate, 60,  100);
            }

            osd_update_video();
        }

        /* Speed throttling */
        if(option.throttle)
        {
            while(tick_count == old_tick_count);
            old_tick_count = tick_count;
        }
    }

    system_poweroff();
    system_shutdown();
    osd_shutdown();
    return 0;
}


/****************************************************************************/

/* Timer handler, runs at 60Hz */
void tick_handler(void)
{
    tick_count++;
    if(msg_enable) --msg_enable;
    if(tick_count % FRAMES_PER_SECOND == 0)
    {
        frame_rate = frames_rendered;
        frames_rendered = 0;
    }
}
END_OF_FUNCTION(tick_handler);


/* Initialize machine dependant stuff */
void osd_init(void)
{
    /* Set up sound hardware */
    if(option.sound)
    {
        /* Initialize sound and update sample rate */
        int ret = msdos_init_sound(&option.sndrate, option.sndcard);

        /* Error - disable sound */
        if(ret != 0) option.sound = 0;
    }

    /* Set up the Allegro library */
    allegro_init();

    /* Install mouse handler */
    if(install_mouse() != -1)
        use_mouse = 1;

    /* Install input devices */
    install_keyboard();
    install_joystick(option.joy_driver);

    /* Install tick handler */
    install_timer();
    LOCK_FUNCTION(tick_handler);
    LOCK_VARIABLE(tick_count);
    LOCK_VARIABLE(frame_rate);
    LOCK_VARIABLE(frames_rendered);
    LOCK_VARIABLE(msg_enable);
    install_int_ex(tick_handler, BPS_TO_TIMER(FRAMES_PER_SECOND));


    dos_video_init();

    /* Allocate memory bitmap for blitting */
    sms_bmp = create_bitmap_ex(option.video_depth, 256, 256);
    clear(sms_bmp);

    /* Set up bitmap structure */
    memset(&bitmap, 0, sizeof(bitmap_t));
    bitmap.width  = sms_bmp->w;
    bitmap.height = sms_bmp->h;
    bitmap.depth  = option.video_depth;
    bitmap.granularity = (bitmap.depth >> 3);
    bitmap.pitch  = bitmap.width * bitmap.granularity;
    bitmap.data   = (uint8 *)&sms_bmp->line[0][0];
    bitmap.viewport.x = 0;
    bitmap.viewport.y = 0;
    bitmap.viewport.w = 256;
    bitmap.viewport.h = 192;

    fg = (option.video_depth == 8) ? 0xFE : makecol(0xFF, 0xFF, 0xFF);
    bg = (option.video_depth == 8) ? 0xFF : makecol(0x00, 0x00, 0x00);
    
}


/* Free system resources */
void osd_shutdown(void)
{
    if(option.sound) msdos_shutdown_sound();
    dos_video_shutdown();
}

/*--------------------------------------------------------------------------*/

/* Load system state */
int load_state(void)
{
    char name[PATH_MAX];
    FILE *fd = NULL;
    strcpy(name, game_name);
    sprintf(strrchr(name, '.'), ".st%d", state_slot);
    fd = fopen(name, "rb");
    if(!fd) return 0;
    system_load_state(fd);
    fclose(fd);
    return 1;
}

/* Save system state */
int save_state(void)
{
    char name[PATH_MAX];
    FILE *fd = NULL;
    strcpy(name, game_name);
    sprintf(strrchr(name, '.'), ".st%d", state_slot);
    fd = fopen(name, "wb");
    if(!fd) return 0;
    system_save_state(fd);
    fclose(fd);
    return 1;
}

/* Save or load SRAM */
void system_manage_sram(uint8 *sram, int slot, int mode)
{
    char name[PATH_MAX];
    FILE *fd;
    strcpy(name, game_name);
    strcpy(strrchr(name, '.'), ".sav");

    switch(mode)
    {
        case SRAM_SAVE:
            if(sms.save)
            {
                fd = fopen(name, "wb");
                if(fd)
                {
                    fwrite(sram, 0x8000, 1, fd);
                    fclose(fd);
                }
            }
            break;

        case SRAM_LOAD:
            fd = fopen(name, "rb");
            if(fd)
            {
                sms.save = 1;
                fread(sram, 0x8000, 1, fd);
                fclose(fd);
            }
            else
            {
                /* No SRAM file, so initialize memory */
                memset(sram, 0x00, 0x8000);
            }
            break;
    }
}

/* Dump RAM to disk */
void dump_wram(void)
{
    static int count = 0;
    char path[PATH_MAX];
    FILE *fd;
    sprintf(path, "wram.%03d", count);
    fd = fopen(path, "wb");
    if(!fd)
        add_msg("Error saving WRAM to `%s'", path);
    else
    {
        add_msg("WRAM saved to `%s'", path);
        fwrite(sms.wram, sizeof(sms.wram), 1, fd);
        fclose(fd);
        ++count;
    }
}

/* Dump VRAM to disk */
void dump_vram(void)
{
    static int count = 0;
    char path[PATH_MAX];
    FILE *fd;
    sprintf(path, "vram.%03d", count);
    fd = fopen(path, "wb");
    if(!fd)
        add_msg("Error saving VRAM to `%s'", path);
    else
    {
        add_msg("VRAM saved to `%s'", path);
        fwrite(vdp.vram, sizeof(vdp.vram), 1, fd);
        fclose(fd);
        ++count;
    }
}





