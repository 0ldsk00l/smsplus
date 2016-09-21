/*
    video.c --
    DOS video support.
*/
#include "osd.h"

/* CRTC parameters for 256x384 and 160x576 displays */
static const uint16 twktbl[2][0x10] =
{
    { 0x4F00, 0x3F01, 0x4002, 0x9203, 0x4204, 0x1005, 0xAF06, 0x1F07,
      0x4109, 0x8C10, 0x7E11, 0x7F12, 0x8615, 0xA916, 0x0063, 0x0101 },
    { 0x2D00, 0x2701, 0x2802, 0x9003, 0x2B04, 0x8005, 0x5306, 0xF007,
      0x6309, 0x4010, 0xAC11, 0x3F12, 0x4015, 0x4A16, 0x0067, 0x0901 },
};

void dos_video_init(void)
{
    /* Change video settings if screen expansion is enabled */
    if(option.expand)
    {
        option.video_driver = GFX_AUTODETECT;
        option.video_width  = (IS_GG) ? 400 : 512;
        option.video_height = (IS_GG) ? 300 : 384;
    }

    /* Double requested height if scanlines are being used
       and screen expansion is disabled */
    if((option.scanlines == 1) && (option.expand == 0))
    {
        option.video_height *= 2;            
    }

    /* Change video settings if tweaked display is enabled */
    if(option.tweak)
    {
        option.video_driver = GFX_VGA;
        option.video_depth  = 8;
        option.video_width  = 320;
        option.video_height = 200;
    }

    /* Attempt to set graphics mode */
    set_color_depth(option.video_depth);
    if(set_gfx_mode(option.video_driver, option.video_width, option.video_height, 0, 0) != 0)
    {
        printf("Error: %s\n", allegro_error);
        exit(1);
    }

    /* Clear palette and display */
    memcpy(sms_pal, black_palette, sizeof(PALETTE));
    sms_pal[0xFE].r = sms_pal[0xFE].g = sms_pal[0xFE].b = 0xFF;
    set_palette(sms_pal);
    clear(screen);

    /* Use vertical stretching if screen expansion is enabled,
       VGA expansion is disabled, and scanlines are disabled */
    if((option.expand == 1) && (option.no_vga == 0) && (option.scanlines == 0))
    {
        outp(0x3D4, 0x09);
        outp(0x3D5, (inp(0x3D5) & 0xE0) | 0x03);
    }

    /* Force overscan color to be black */
    if(option.video_depth == 8)
    {
        inp(0x3DA);
        outp(0x3C0, 0x31);
        outp(0x3C0, 0xFF);
        clear_to_color(screen, 0xFF);
    }

    /* Tweak display accordingly */
    if(option.tweak)
    {
        int i, j = IS_GG ? 1 : 0;

        /* Disable CRTC write protect */
        outp(0x3D4, 0x11);
        outp(0x3D5, inp(0x3D5) & 0x7F);

        /* Load CRTC parameters */
        for(i = 0; i <= 0x0D; i++)
            outpw(0x3D4, twktbl[j][i]);

        /* Enable CRTC write protect */
        outp(0x3D4, 0x11);
        outp(0x3D5, inp(0x3D5) | 0x80);

        /* Set clocking mode register */
        outp(0x3C2, twktbl[j][0x0E]);

        /* Set miscellaneous output register */
        outpw(0x3C4, twktbl[j][0x0F]);
    }
}

void dos_video_shutdown(void)
{
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
}

/* Update video */
void osd_update_video(void)
{
    /* Wait for VSync */
    if(option.vsync) vsync();

    /* Update the palette */
    if(option.video_depth == 8) update_palette();

    /* Use the blur effect */
    if(option.blur)
    {
        blur((uint16 *)&sms_bmp->line[bitmap.viewport.y][bitmap.viewport.x * bitmap.granularity],
                bitmap.viewport.w, bitmap.viewport.h, 
                (bitmap.width-bitmap.viewport.w) * bitmap.granularity
                );
    }

    if(option.fps) msg_print(2, 2, "%d", frame_rate);
    if(msg_enable)  msg_print(4, bitmap.viewport.h - 12, "%s", msg);

    blitter_proc(sms_bmp, screen);
}


/* Update VGA palette hardware */
void update_palette(void)
{
    RGB color;
    int index;

    if(bitmap.pal.update == 0) return;

    for(index = 0; index < PALETTE_SIZE; index++)
    {
        if(bitmap.pal.dirty[index])
        {
            color.r = (bitmap.pal.color[index][0] >> 2);
            color.g = (bitmap.pal.color[index][1] >> 2);
            color.b = (bitmap.pal.color[index][2] >> 2);
            set_color(0x00 | index, &color);
            set_color(0x20 | index, &color);
            set_color(0x40 | index, &color);
            set_color(0x60 | index, &color);
        }
    }

    bitmap.pal.update = 0;
    memset(bitmap.pal.dirty, 0, PALETTE_SIZE);
}



