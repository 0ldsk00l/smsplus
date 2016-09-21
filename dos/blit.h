
#ifndef _BLIT_H_
#define _BLIT_H_

#define BLITTER_FUNC(x)     extern void x (BITMAP *src, BITMAP *dst);

/* Global data */
extern int blitter_index;
extern void (*blitter_proc)(BITMAP *src, BITMAP *dst);

/* Function prototypes */
void pick_blitter_proc(void);

BLITTER_FUNC( (*blit_table[])                  )
BLITTER_FUNC( blit_sms_twk                     )
BLITTER_FUNC( blit_gg_twk                      )
BLITTER_FUNC( blit_sms                         )
BLITTER_FUNC( blit_sms_scanlines               )
BLITTER_FUNC( blit_sms_scale                   )
BLITTER_FUNC( blit_sms_scale_scanlines         )
BLITTER_FUNC( blit_gg                          )
BLITTER_FUNC( blit_gg_scanlines                )
BLITTER_FUNC( blit_gg_scale                    )
BLITTER_FUNC( blit_gg_scale_scanlines          )
BLITTER_FUNC( blit_sms_expand_scanlines_mmx    )
BLITTER_FUNC( blit_sms_expand_scanlines        )
BLITTER_FUNC( blit_sms_expand_mmx_vs           )
BLITTER_FUNC( blit_sms_expand_mmx              )
BLITTER_FUNC( blit_sms_expand_vs               )
BLITTER_FUNC( blit_sms_expand                  )
BLITTER_FUNC( blit_gg_expand_scanlines_mmx     )
BLITTER_FUNC( blit_gg_expand_scanlines         )
BLITTER_FUNC( blit_gg_expand_mmx_vs            )
BLITTER_FUNC( blit_gg_expand_mmx               )
BLITTER_FUNC( blit_gg_expand_vs                )
BLITTER_FUNC( blit_gg_expand                   )
BLITTER_FUNC( blit_sms_expand_scanlines_mmx_16 )
BLITTER_FUNC( blit_sms_expand_scanlines_16     )
BLITTER_FUNC( blit_sms_expand_mmx_vs_16        )
BLITTER_FUNC( blit_sms_expand_mmx_16           )
BLITTER_FUNC( blit_sms_expand_vs_16            )
BLITTER_FUNC( blit_sms_expand_16               )
BLITTER_FUNC( blit_gg_expand_scanlines_mmx_16  )
BLITTER_FUNC( blit_gg_expand_scanlines_16      )
BLITTER_FUNC( blit_gg_expand_mmx_vs_16         )
BLITTER_FUNC( blit_gg_expand_mmx_16            )
BLITTER_FUNC( blit_gg_expand_vs_16             )
BLITTER_FUNC( blit_gg_expand_16                )

#endif /* _BLIT_H_ */






