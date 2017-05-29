#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int max_cc = 0;
inline void insert(unsigned short *table, int index, int value) {
    table[index] = value;
}

static int calc_index(int i)
{
    int code = 0;
    int ch1 = (i & 0xff00)>>8;
    int ch2 = i & 0xff;
    int adjust = ch2 < 159;
    int rowOffset = ch1 < 160 ? 112 : 176;
    int cellOffset = adjust ? (ch2 > 127 ? 32 : 31) : 126;

    ch1 = ((ch1 - rowOffset) << 1) - adjust;
    ch2 -= cellOffset;

    ch1 += 128;
    ch2 += 128;

    code = ((ch1 - 0xA1) * 94 + ch2 - 0xA1);
    return code;
}

static void print(const unsigned short *table, int cc) {
    int i;
    for (i=0; i<cc/3; i +=1) {
        printf("\t0x%04x, 0x%04x, 0x%04x,\n", table[i*3], table[i*3+1], table[i*3+2]);
    }
    if (cc%3 == 2) {
        printf("\t0x%04x, 0x%04x, \0x%04x\n", table[cc-2], table[cc-1]);
    }
    else if (cc%3 == 1) {
        printf("\t0x%04x\n", table[cc-1]);
    }
}

static void parse(unsigned short *table, const char *line) {
    int index, value;
    int cc;
    sscanf(line, "%x %x", &index, &value);
    if (index > 0xffff || value > 0xffff) {
        printf("%x, %x\n", index, value);
        assert(0);
    }
    cc = calc_index(index);
    if (cc > max_cc) max_cc = cc;
    if (cc >= 0)
        insert(table, cc, value);
}

int main(int argc, const char *argv[]) {
    char buf[1024];
    int len = 256 * 256;
    unsigned short *table;

    table = (unsigned short *)malloc(sizeof(unsigned short) * len);
    memset(table, 0, sizeof(unsigned short) * len);

    while (fgets(buf, sizeof(buf), stdin)) {
        parse(table, buf);
    }
    print(table, max_cc);
    free(table);
    return 0;
}
