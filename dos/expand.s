
;----------------------------------------------------------------------------
; expand (vscale, usemmx, depth, dlwid)
;----------------------------------------------------------------------------
%macro                  expand  4
                        %define VSCALE          %1
                        %define USEMMX          %2
                        %define DEPTH           %3
                        %define DLWID           %4
%if(USEMMX == 1)
                        movq    mm0, [ds:esi]
                        movq    mm1, mm0
    %if(DEPTH == 16)
                        punpcklwd mm0, mm0
                        punpckhwd mm1, mm1
    %else
                        punpcklbw mm0, mm0
                        punpckhbw mm1, mm1
    %endif
    %if(VSCALE == 1)
                        movq    [es:edi], mm0
                        movq    [es:edi + DLWID], mm0
                        movq    [es:edi + 8], mm1
                        movq    [es:edi + 8 + DLWID], mm1
    %else
                        movq    [es:edi], mm0
                        movq    [es:edi + 8], mm1
    %endif
                        add     esi, byte 8
                        add     edi, byte 16
%else
                        mov     eax, [ds:esi]
                        mov     ebx, eax        
    %if(DEPTH == 16)
                        rol     eax, 16         
                        xchg    ax, bx          
    %else
                        bswap   eax                 
                        xchg    ax, bx              
                        rol     eax, 8              
                        ror     ebx, 8
    %endif
    %if(VSCALE == 1)
                        mov     [es:edi], eax
                        mov     [es:edi + DLWID], eax
                        mov     [es:edi + 4], ebx
                        mov     [es:edi + 4 + DLWID], ebx
    %else
                        mov     [es:edi], eax
                        mov     [es:edi + 4], ebx
    %endif
                        add     esi, byte 4
                        add     edi, byte 8
%endif
                        %endm

;-----------------------------------------------------------------------------
; blit (label, width, height, srcofs, dstofs, slwid, dlwid, usemmx, vscale, scanlines, depth)
;-----------------------------------------------------------------------------
%macro                  blit    11
                        %define LABEL           %1
                        %define WIDTH           %2
                        %define HEIGHT          %3
                        %define SRCOFS          %4
                        %define DSTOFS          %5
                        %define SLWID           %6
                        %define DLWID           %7
                        %define USEMMX          %8
                        %define VSCALE          %9
                        %define SCANLINES       %10
                        %define DEPTH           %11

                        align   4
                        global  LABEL
                        LABEL:

                        pushad
                        push    ds
                        push    es

                        mov     edx, [esp + 44]
                        mov     ax, word [edx + 60]
                        mov     ds, ax
                        mov     esi, [edx + 64]

                        mov     edx, [esp + 48]
                        mov     ax, word [edx + 60]
                        mov     es, ax
                        mov     edi, [edx + 64]
%if(SRCOFS != 0)
                        lea     esi, [esi + SRCOFS]
%endif

%if(DSTOFS != 0)
                        lea     edi, [edi + DSTOFS]
%endif
                        mov     edx, HEIGHT
        .row

    %if(USEMMX == 1)
                        mov     ecx, (WIDTH / 8)
    %else
                        mov     ecx, (WIDTH / 4)
    %endif

        .column
                        expand          VSCALE, USEMMX, DEPTH, DLWID

                        dec     ecx
                        jnz     .column
%if((SLWID != 0) && ((SLWID - WIDTH) != 0))
                        lea     esi, [esi + (SLWID - WIDTH)]
%endif

%if(SCANLINES == 1 || VSCALE == 1)
    %if(DLWID != 0)
                        lea     edi, [edi + (DLWID - (WIDTH * 2)) + DLWID]
    %endif
%else
    %if(DLWID != 0)
                        lea     edi, [edi + (DLWID - (WIDTH * 2))]
    %endif
%endif
                        dec     edx
                        jnz     .row

                        pop     es
                        pop     ds
                        popad
%if (USEMMX == 1)
                        emms
%endif
                        ret
                        %endm

;-----------------------------------------------------------------------------

                        section .text

blit _blit_sms_expand_scanlines_mmx,    256, 192, 0,           0,          256, 512, 1, 0, 1, 8
blit _blit_sms_expand_scanlines,        256, 192, 0,           0,          256, 512, 0, 0, 1, 8
blit _blit_sms_expand_mmx_vs,           256, 192, 0,           0,          256, 512, 1, 1, 0, 8
blit _blit_sms_expand_mmx,              256, 192, 0,           0,          256, 512, 1, 0, 0, 8
blit _blit_sms_expand_vs,               256, 192, 0,           0,          256, 512, 0, 1, 0, 8
blit _blit_sms_expand,                  256, 192, 0,           0,          256, 512, 0, 0, 0, 8

blit _blit_gg_expand_scanlines_mmx,     160, 144, 48+(24*256), 40+(6*512), 256, 512, 1, 0, 1, 8
blit _blit_gg_expand_scanlines,         160, 144, 48+(24*256), 40+(6*512), 256, 512, 0, 0, 1, 8
blit _blit_gg_expand_mmx_vs,            160, 144, 48+(24*256), 40+(6*256), 256, 512, 1, 1, 0, 8
blit _blit_gg_expand_mmx,               160, 144, 48+(24*256), 40+(6*256), 256, 512, 1, 0, 0, 8
blit _blit_gg_expand_vs,                160, 144, 48+(24*256), 40+(6*256), 256, 512, 0, 1, 0, 8
blit _blit_gg_expand,                   160, 144, 48+(24*256), 40+(6*256), 256, 512, 0, 0, 0, 8

blit _blit_sms_expand_scanlines_mmx_16, 512, 192, 0,           0,          512, 1024, 1, 0, 1, 16
blit _blit_sms_expand_scanlines_16,     512, 192, 0,           0,          512, 1024, 0, 0, 1, 16
blit _blit_sms_expand_mmx_vs_16,        512, 192, 0,           0,          512, 1024, 1, 1, 0, 16
blit _blit_sms_expand_mmx_16,           512, 192, 0,           0,          512, 1024, 1, 0, 0, 16
blit _blit_sms_expand_vs_16,            512, 192, 0,           0,          512, 1024, 0, 1, 0, 16
blit _blit_sms_expand_16,               512, 192, 0,           0,          512, 1024, 0, 0, 0, 16
                                                          
blit _blit_gg_expand_scanlines_mmx_16,  320, 144, 96+(24*512), 80+(6*1024), 512, 1024, 1, 0, 1, 16
blit _blit_gg_expand_scanlines_16,      320, 144, 96+(24*512), 80+(6*1024), 512, 1024, 0, 0, 1, 16
blit _blit_gg_expand_mmx_vs_16,         320, 144, 96+(24*512), 80+(6*512 ), 512, 1024, 1, 1, 0, 16
blit _blit_gg_expand_mmx_16,            320, 144, 96+(24*512), 80+(6*512 ), 512, 1024, 1, 0, 0, 16
blit _blit_gg_expand_vs_16,             320, 144, 96+(24*512), 80+(6*512 ), 512, 1024, 0, 1, 0, 16
blit _blit_gg_expand_16,                320, 144, 96+(24*512), 80+(6*512 ), 512, 1024, 0, 0, 0, 16
