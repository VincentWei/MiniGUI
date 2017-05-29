/*
 ** $Id$
 **
 ** md5.h: the head file of md5 algorithm.
 ** 
 ** Copyright (C) 2004 ~ 2007 Feynman Software.
 **
 ** Create date: 2004/05/10
 */


#ifndef _MD5_H
    #define _MD5_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;

/* MD5 context. */
typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

void get_md5 (const unsigned char *string, unsigned char digest[16]);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MD5_H */

