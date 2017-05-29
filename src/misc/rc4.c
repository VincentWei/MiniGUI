/* 
 * http://en.wikipedia.org/wiki/RC4
 */

#include "rc4.h"

static void swap(unsigned char *s, unsigned int i, unsigned int j) {
    unsigned char temp = s[i];
    s[i] = s[j];
    s[j] = temp;
}

/* KSA */
void RC4_set_key(RC4_KEY *key, int len, const unsigned char *data) {
    int i, j;

    for (i = 0; i < 256; i++)
        key->S[i] = i;

    for (i = j = 0; i < 256; i++) {
        j = (j + data[i % len] + key->S[i]) & 255;
        swap(key->S, i, j);
    }

    key->i = key->j = 0;
}

/* PRGA */
static unsigned char rc4_output(RC4_KEY *key) {
    key->i = (key->i + 1) & 255;
    key->j = (key->j + key->S[key->i]) & 255;

    swap(key->S, key->i, key->j);

    return key->S[(key->S[key->i] + key->S[key->j]) & 255];
}

void RC4(RC4_KEY *key, unsigned long len, const unsigned char *indata, unsigned char *outdata) {
    unsigned long n;

    for (n=0; n<len; ++n) {
        outdata[n] = indata[n] ^ rc4_output(key);
    }
}

#if 0
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "rc4.c"

void test(int key_len, const unsigned char *key, int data_len, const unsigned char *data) {
    RC4_KEY rc4_key;
    unsigned char out[1024];
    int i;

    assert(data_len < sizeof(out));
    RC4_set_key(&rc4_key, key_len, key);
    RC4(&rc4_key, data_len, data, out);

    for (i=0; i<data_len; ++i) {
        printf("%02X", out[i]);
    }
    printf("\n");
}

int main(int argc, const char *argv[]) {
    test(strlen(argv[1]), argv[1], strlen(argv[2]), argv[2]);
    return 0;
}
#endif
