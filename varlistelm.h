#ifndef VARLISTELM_H
#define VARLISTELM_H

#define varlistelm_maxchnls 16
#include <stdint.h>

struct varlistelm_t
    {
    unsigned char show; //0-bin 1-dec 2-signeddec 3-hex 4-float
    unsigned char size; //8 16 32 64
    double cfcnt;       //0-not allowed
    unsigned short chnl;
    char name[100];
    unsigned long addr;
    union {
    /*S - signet  US - unsignet
    US64   u;
    S8    s8;
    S16  s16;
    S32  s32;
    S64  s64;*/
    uint64_t u;
    int8_t s8;
    int16_t s16;
    int32_t s32;
    int64_t s64;
    }val[varlistelm_maxchnls];
    };

#endif // VARLISTELM_H

