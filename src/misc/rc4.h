#ifndef _RC4_H
#define _RC4_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned char S[256];
    unsigned int i, j;
} RC4_KEY;

MG_LOCAL void RC4_set_key(RC4_KEY *key, int len, const unsigned char *data);
MG_LOCAL void RC4(RC4_KEY *key, unsigned long len, const unsigned char *indata, unsigned char *outdata);

#ifdef __cplusplus
}
#endif

#endif /* _RC4_H */
