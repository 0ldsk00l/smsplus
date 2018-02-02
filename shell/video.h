#ifndef _SMSPVIDEO_H
#define _SMSPVIDEO_H

#define VIDEO_WIDTH_SMS 256
#define VIDEO_HEIGHT_SMS 192
#define VIDEO_WIDTH_GG 160
#define VIDEO_HEIGHT_GG 144

void ogl_init();
void ogl_render();
void ogl_deinit();

void smsp_video_screenshot(const char* filename);

#endif
