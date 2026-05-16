/*
 * File:   adpcm.h
 *
 * Adaptive Differential Pulse Code Modulation Library
 *
 * Cheng Xing, Rene Zhang
 * cx34, rz99
 * December 2015
 */

#ifndef __ADPCM_H__
#define __ADPCM_H__

/*
 * Compresses a 16 bit sample into a 4 bit character
 * prevsample and previndex are used as the previous encoding state
 */
char ADPCMEncoder(signed long sample, signed long *prevsample, int *previndex);

/*
 * Decompresses a 4 bit encoded character into a 16 bit audio sample
 * prevsample and previndex are used as the previous decoding state
 */
signed long ADPCMDecoder(char code, signed long *prevsample, int *previndex);

#endif /* __ADPCM_H__ */
