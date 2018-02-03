#ifndef _H_YM2413_
#define _H_YM2413_
#include <stdint.h>
/* select output bits size of output : 8 or 16 */
#define SAMPLE_BITS 16

#if (SAMPLE_BITS==16)
typedef int16_t SAMP;
#endif
#if (SAMPLE_BITS==8)
typedef int8_t SAMP;
#endif

int  YM2413Init(int num, int clock, int rate);
void YM2413Shutdown(void);
void YM2413ResetChip(int which);
void YM2413Write(int which, int a, int v);
unsigned char YM2413Read(int which, int a);
void YM2413UpdateOne(int which, int16_t **buffers, int length);

typedef void (*OPLL_UPDATEHANDLER)(int param,int min_interval_us);

void YM2413SetUpdateHandler(int which, OPLL_UPDATEHANDLER UpdateHandler, int param);

#endif /*_H_YM2413_*/
