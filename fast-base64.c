#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned char BYTE;

#include "lookup_tables.h"


void encode_base64(
    const BYTE* data,
    const size_t len,
    char** out) 
{
    size_t slen = (len / 3) * 4;
    char* buf = (char*) malloc(slen + 1);

    for (size_t ib = 0, ic = 0; ib < len; ib += 3, ic += 4) {
        buf[ic + 0] = C0_ENCODE_TABLE[*(data + ib)];
        buf[ic + 1] = C1_ENCODE_TABLE[*((unsigned short*)(data + ib))];
        buf[ic + 2] = C2_ENCODE_TABLE[*((unsigned short*)(data + ib + 1))];
        buf[ic + 3] = C3_ENCODE_TABLE[*(data + ib + 2)];
    }

    buf[slen] = '\0';
    *out = buf;
}

void decode_base64(
    const char* b64,
    BYTE** out,
    size_t* out_len)
{
    size_t slen = strlen(b64);
    size_t len = (slen / 4) * 3;
    BYTE* buf = (BYTE*) malloc(len);

    for (size_t ib = 0, ic = 0; ib < len; ib += 3, ic += 4) {
        buf[ib + 0] = B0_DECODE_TABLE[*((unsigned short*) (b64 + ic + 0))];
        buf[ib + 1] = B1_DECODE_TABLE[*((unsigned short*) (b64 + ic + 1))];
        buf[ib + 2] = B2_DECODE_TABLE[*((unsigned short*) (b64 + ic + 2))];
    }

    *out = buf;
    *out_len = len;
}

int main() 
{
    const BYTE data[] = "The Magic Words are Squeamish Ossifrage";
    char* b64 = (char*)0;
    BYTE* dec = (BYTE*)0;
    size_t dec_len;

    encode_base64(data, sizeof(data), &b64);
    puts(b64);

    decode_base64(b64, &dec, &dec_len);

    for (size_t i = 0; i < dec_len; i++) {
        printf("%c", dec[i]);
    }
    printf("\n");

    free(b64);
    free(dec);

    return 0;
}
