
#include "osd.h"


/* Options structure */
t_option option;


/* Joystick driver names and numbers */
struct {
    char *name;
    int id;
} joy_driver_table[] = {
    { "auto",             JOY_TYPE_AUTODETECT },
    { "none",             JOY_TYPE_NONE },
    { "standard",         JOY_TYPE_STANDARD },
    { "2pads",            JOY_TYPE_2PADS },
    { "4button",          JOY_TYPE_4BUTTON },
    { "6button",          JOY_TYPE_6BUTTON },
    { "8button",          JOY_TYPE_8BUTTON },
    { "fspro",            JOY_TYPE_FSPRO },
    { "wingex",           JOY_TYPE_WINGEX },
    { "sidewinder",       JOY_TYPE_SIDEWINDER },
    { "gamepdpro",        JOY_TYPE_GAMEPAD_PRO },
    { "grip",             JOY_TYPE_GRIP },
    { "grip4",            JOY_TYPE_GRIP4 },
    { "sneslpt1",         JOY_TYPE_SNESPAD_LPT1 },
    { "sneslpt2",         JOY_TYPE_SNESPAD_LPT2 },
    { "sneslpt3",         JOY_TYPE_SNESPAD_LPT3 },
    { "psxlpt1",          JOY_TYPE_PSXPAD_LPT1 },
    { "psxlpt2",          JOY_TYPE_PSXPAD_LPT2 },
    { "psxlpt3",          JOY_TYPE_PSXPAD_LPT3 },
    { "n64lpt1",          JOY_TYPE_N64PAD_LPT1 },
    { "n64lpt2",          JOY_TYPE_N64PAD_LPT2 },
    { "n64lpt3",          JOY_TYPE_N64PAD_LPT3 },
    { "db9lpt1",          JOY_TYPE_DB9_LPT1 },
    { "db9lpt2",          JOY_TYPE_DB9_LPT2 },
    { "db9lpt3",          JOY_TYPE_DB9_LPT3 },
    { "tglpt1",           JOY_TYPE_TURBOGRAFX_LPT1 },
    { "tglpt2",           JOY_TYPE_TURBOGRAFX_LPT2 },
    { "tglpt3",           JOY_TYPE_TURBOGRAFX_LPT3 },
    { "wingwar",          JOY_TYPE_WINGWARRIOR },
    { "segaisa",          JOY_TYPE_IFSEGA_ISA},
    { "segapci",          JOY_TYPE_IFSEGA_PCI},
    { "segapci2",         JOY_TYPE_IFSEGA_PCI_FAST},
    { 0, 0 }
};


/* Video driver names and numbers */
struct {
    char *name;
    int id;
} video_driver_table[] = {
    { "auto",             GFX_AUTODETECT },
    { "safe",             GFX_SAFE },
    { "vga",              GFX_VGA },
    { "modex",            GFX_MODEX },
    { "vesa2l",           GFX_VESA2L },
    { "vesa3",            GFX_VESA3 },
    { "vbeaf",            GFX_VBEAF },
    { 0, 0 }
};



void do_config(char *file)
{
    /* The commandline */
    extern int __crt0_argc;
    extern char **__crt0_argv;

    /* Our token list */
    int i, argc;
    char *argv[0x100];

    set_option_defaults();

    for(i = 0; i < 0x100; i++) argv[i] = NULL;

    /* Check configuration file */
    if(file) parse_file(file, &argc, argv);

    /* Check extracted tokens */
    parse_args(argc, argv);

    /* Free token list */
    for(i = 0; i < argc; i++)
    {
        if(argv[argc]) free (argv[argc]);
    }

    /* Check command line */
    parse_args(__crt0_argc, __crt0_argv);
}


/* Parse configuration file */
int parse_file(const char *filename, int *argc, char **argv)
{
    char token[0x100];
    FILE *handle = NULL;

    *argc = 0;

    handle = fopen(filename, "r");
    if(!handle) return (0);

    fscanf(handle, "%s", &token[0]);
    while(!(feof(handle)))
    {
        int size = strlen(token) + 1;
        argv[*argc] = malloc(size);
        if(!argv[*argc]) return (0);
        strcpy(argv[*argc], token);
        *argc += 1;
        fscanf(handle, "%s", &token[0]);
    }

    if(handle) fclose(handle);
    return (1);
}


void set_option_defaults(void)
{
    option.joy_driver   =   JOY_TYPE_NONE;
    option.video_driver =   GFX_AUTODETECT;
    option.video_depth  =   8;
    option.video_width  =   320;
    option.video_height =   200;
    option.no_vga       =   0;
    option.no_mmx       =   0;
    option.expand       =   0;
    option.blur         =   0;
    option.scale        =   0;
    option.scanlines    =   0;
    option.tweak        =   0;
    option.vsync        =   0;
    option.throttle     =   0;
    option.fps          =   0;
    option.sound        =   0;
    option.sndcard      =   -1;
    option.sndrate      =   44100;
    option.country      =   TERRITORY_EXPORT;
    option.fm_enable    =   0;
    option.codies       =   0;
}

/* Parse argument list */
void parse_args(int argc, char **argv)
{
    int i;

    for(i = 0; i < argc; i++)
    {
        int left = argc - i - 1;

        if(stricmp(argv[i], "-novga") == 0)
        {
            option.no_vga = 1;
        }

        if(stricmp(argv[i], "-nommx") == 0)
        {
            option.no_mmx = 1;
        }

        if(stricmp(argv[i], "-scanlines") == 0)
        {
            option.scanlines = 1;
        }

        if(stricmp(argv[i], "-scale") == 0)
        {
            option.scale = 1;
        }

        if(stricmp(argv[i], "-expand") == 0)
        {
            option.expand = 1;
        }

        if(stricmp(argv[i], "-vsync") == 0)
        {
            option.vsync = 1;
        }

        if(stricmp(argv[i], "-fps") == 0)
        {
            option.fps = 1;
        }

        if(stricmp(argv[i], "-sound") == 0)
        {
            option.sound = 1;
        }

        if(stricmp(argv[i], "-sndcard") == 0 && left)
        {
            option.sndcard = atoi(argv[i+1]);
        }

        if(stricmp(argv[i], "-sndrate") == 0 && left)
        {
            option.sndrate = atoi(argv[i+1]);
        }

        if(stricmp(argv[i], "-res") == 0 && left > 1)
        {
            option.video_width  = atoi(argv[i+1]);
            option.video_height = atoi(argv[i+2]);
        }

        if(stricmp(argv[i], "-depth") == 0 && left)
        {
            option.video_depth = atoi(argv[i+1]);
        }

        if(stricmp(argv[i], "-blur") == 0)
        {
            option.blur = 1;
            option.video_depth = 16;
        }

        if(stricmp(argv[i], "-tweak") == 0)
        {
            option.tweak = 1;
        }

        if(stricmp(argv[i], "-jp") == 0)
        {
            option.country = TERRITORY_DOMESTIC;
        }

        if(stricmp(argv[i], "-fm") == 0)
        {
            option.fm_enable = 1;
        }

        if(stricmp(argv[i], "-vdriver") == 0 && left)
        {
            int j;
            for(j = 0; video_driver_table[j].name != 0; j++)
            {
                if(stricmp(argv[i + 1], video_driver_table[j].name) == 0)
                {
                    option.video_driver = video_driver_table[j].id;                                        
                }
            }
        }

        if(stricmp(argv[i], "-joy") == 0 && left)
        {
            int j;
            for(j = 0; joy_driver_table[j].name != 0; j++)
            {
                if(stricmp(argv[i + 1], joy_driver_table[j].name) == 0)
                {
                    option.joy_driver = joy_driver_table[j].id;                                        
                }
            }
        }

        if(stricmp(argv[i], "-throttle") == 0)
        {
            option.throttle = 1;
        }

        if(stricmp(argv[i], "-codies") == 0)
        {
            option.codies = 1;
        }
    }
}


