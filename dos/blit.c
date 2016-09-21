
#include "osd.h"

/* Index of blitter proc from table */
int blitter_index;

/* Points to currently used blitter proc */
void (*blitter_proc)(BITMAP *src, BITMAP *dst) = NULL;


/* Pick an appropriate blitter proc based on settings */
void pick_blitter_proc(void)
{
    /* Color depth */
    int f_d = (option.video_depth == 16) ? 1 : 0;

    /* 1= Don't use VGA vertical scaling (write odd lines manually) */
    int f_v = (option.no_vga) ? 1 : 0;

    /* 1= Use MMX */
    int f_m = ((cpu_capabilities & CPU_MMX) && (option.no_mmx == 0)) ? 1 : 0;

    /* 1= Do screen expansion */
    int f_e = (option.expand) ? 1 : 0;

    /* 1= Do screen scaling */
    int f_z = (option.scale) ? 1 : 0;

    /* 1= Use scanlines effect */
    int f_s = (option.scanlines) ? 1 : 0;

    /* 1= GG, 0= SMS */
    int f_g = (IS_GG) ? 1 : 0;

    /* Disable zooming if expansion is on */
    if(f_e && f_z) f_z = 0;

    /* Ignore VGA and MMX disable switches if not using expansion */
    if(!f_e) { f_v = f_m = 0; };

    /* If expansion and scanlines are on, ignore VGA disable */
    if(f_e && f_s) f_v = 0;

    /* Form index */
    blitter_index = (f_d << 6 | f_v << 5 | f_m << 4 | f_e << 3 | f_z << 2 | f_s << 1 | f_g) & 0x7F;

    /* Assign blitter proc */
    blitter_proc = blit_table[blitter_index];

    /* Assign custom blitters */
    if(option.tweak) blitter_proc = IS_GG ? blit_gg_twk : blit_sms_twk;
}

/* "C" blitter functions */

void blit_sms(BITMAP *src, BITMAP *dst) {
    blit(src, dst, 0, 0, (SCREEN_W-bitmap.viewport.w)/2, (SCREEN_H-bitmap.viewport.h)/2, bitmap.viewport.w, bitmap.viewport.h);
}

void blit_sms_scanlines(BITMAP *src, BITMAP *dst) {
    int i;
    for(i=0;i<bitmap.viewport.h;i+=1)
    blit(src, dst, 0, i, (SCREEN_W-bitmap.viewport.w)/2, (((SCREEN_H/2)-bitmap.viewport.h))+(i << 1), bitmap.viewport.w, 1);
}

void blit_gg(BITMAP *src, BITMAP *dst) {
    blit(src, dst, bitmap.viewport.x, bitmap.viewport.y, (SCREEN_W-bitmap.viewport.w)/2, (SCREEN_H-bitmap.viewport.h)/2, bitmap.viewport.w, bitmap.viewport.h);
}

void blit_gg_scanlines(BITMAP *src, BITMAP *dst) {
    int i;
    for(i=0;i<bitmap.viewport.h;i+=1)
    blit(src, dst, bitmap.viewport.x, bitmap.viewport.y+i, (SCREEN_W-bitmap.viewport.w)/2, (((SCREEN_H/2)-bitmap.viewport.h))+(i << 1), bitmap.viewport.w, 1);
}

void blit_sms_scale(BITMAP *src, BITMAP *dst) {
    stretch_blit(src, dst, 0, 0, bitmap.viewport.w, bitmap.viewport.h, 0, 0, SCREEN_W, SCREEN_H);
}

void blit_sms_scale_scanlines(BITMAP *src, BITMAP *dst) {
    int i;
    for(i=0;i<bitmap.viewport.h;i+=1)
    stretch_blit(src, dst, 0, i, bitmap.viewport.w, 1, 0, (((SCREEN_H/2)-bitmap.viewport.h))+(i << 1), SCREEN_W, 1);
}

void blit_gg_scale(BITMAP *src, BITMAP *dst) {
    stretch_blit(src, dst, bitmap.viewport.x, bitmap.viewport.y, bitmap.viewport.w, bitmap.viewport.h, 0, 0, SCREEN_W, SCREEN_H);
}

void blit_gg_scale_scanlines(BITMAP *src, BITMAP *dst) {
    int i;
    for(i=0;i<bitmap.viewport.h;i+=1)
    stretch_blit(src, dst, bitmap.viewport.x, bitmap.viewport.y+i, bitmap.viewport.w, 1, 0, (((SCREEN_H/2)-bitmap.viewport.h))+(i << 1), SCREEN_W, 1);
}

void blit_sms_twk(BITMAP *src, BITMAP *dst) {
    blit(src, dst, 0, 0, 0, 0, bitmap.viewport.w, bitmap.viewport.h);
}

void blit_gg_twk(BITMAP *src, BITMAP *dst) {
    blit(src, dst, bitmap.viewport.x, bitmap.viewport.y, 0, 0, bitmap.viewport.w, bitmap.viewport.h);
}


/* List of blitter functions */

void (*blit_table[])(BITMAP *src, BITMAP *dst) =
{
    blit_sms                         ,
    blit_gg                          ,
    blit_sms_scanlines               ,
    blit_gg_scanlines                ,
    blit_sms_scale                   ,
    blit_gg_scale                    ,
    blit_sms_scale_scanlines         ,
    blit_gg_scale_scanlines          ,
    blit_sms_expand                  ,
    blit_gg_expand                   ,
    blit_sms_expand_scanlines        ,
    blit_gg_expand_scanlines         ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    blit_sms_expand_mmx              ,
    blit_gg_expand_mmx               ,
    blit_sms_expand_scanlines_mmx    ,
    blit_gg_expand_scanlines_mmx     ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    blit_sms_expand_vs               ,
    blit_gg_expand_vs                ,
    NULL                             ,            
    NULL                             ,            
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             , 
    NULL                             , 
    NULL                             , 
    NULL                             , 
    NULL                             , 
    NULL                             , 
    NULL                             , 
    NULL                             , 
    blit_sms_expand_mmx_vs           ,
    blit_gg_expand_mmx_vs            ,
    NULL                             ,         
    NULL                             ,         
    NULL                             , 
    NULL                             , 
    NULL                             , 
    NULL                             ,
    blit_sms                         ,
    blit_gg                          ,
    blit_sms_scanlines               ,
    blit_gg_scanlines                ,
    blit_sms_scale                   ,
    blit_gg_scale                    ,
    blit_sms_scale_scanlines         ,
    blit_gg_scale_scanlines          ,
    blit_sms_expand_16               ,
    blit_gg_expand_16                ,
    blit_sms_expand_scanlines_16     ,
    blit_gg_expand_scanlines_16      ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    blit_sms_expand_mmx_16           ,
    blit_gg_expand_mmx_16            ,
    blit_sms_expand_scanlines_mmx_16 ,
    blit_gg_expand_scanlines_mmx_16  ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    blit_sms_expand_vs_16            ,
    blit_gg_expand_vs_16             ,
    NULL                             ,            
    NULL                             ,            
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             ,
    NULL                             , 
    NULL                             , 
    NULL                             , 
    NULL                             , 
    NULL                             , 
    NULL                             , 
    NULL                             , 
    NULL                             , 
    blit_sms_expand_mmx_vs_16        ,
    blit_gg_expand_mmx_vs_16         ,
    NULL                             ,         
    NULL                             ,         
    NULL                             , 
    NULL                             , 
    NULL                             , 
    NULL                             , 
};
