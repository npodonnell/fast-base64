#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned char BYTE;

#include "lookup_tables.h"

#define BUFSIZE 1024 * 1024

static BYTE buf[BUFSIZE];
static char out[(BUFSIZE * 5) / 3];	// Technically 4/3 of input, but take some margin

void encode_base64(
    const BYTE* data,
    const size_t len,
    char* out,
    size_t* out_len)
{
    size_t slen = (len / 3) * 4;
    BYTE* dp = (BYTE*) data - 1;
    BYTE* dl = dp + len;
    char* op = out - 1;

    // for (size_t ic = 0; dp < dl;) {
    //     out[ic++] = C0_ENCODE_TABLE[*(++dp)];
    //     out[ic++] = C1_ENCODE_TABLE[*((unsigned short*)(dp))];
    //     out[ic++] = C2_ENCODE_TABLE[*((unsigned short*)(++dp))];
    //     out[ic++] = C3_ENCODE_TABLE[*(++dp)];
    // }

    for (size_t ic = 0; dp < dl;) {
        *(++op) = C0_ENCODE_TABLE[*(++dp)];
        *(++op) = C1_ENCODE_TABLE[*((unsigned short*)(dp))];
        *(++op) = C2_ENCODE_TABLE[*((unsigned short*)(++dp))];
        *(++op) = C3_ENCODE_TABLE[*(++dp)];
    }

    *out_len = slen;
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

int main(int argc, char** argv) 
{
    FILE* fp = stdin;
    size_t nread;
    size_t nout;

    while ((nread = fread(buf, 1, BUFSIZE, fp)) > 0) {
        encode_base64(buf, nread, out, &nout);
        fwrite(out, nout, 1, stdout);
    }

    // //const BYTE data[] = "The Magic Words are Squeamish Ossifrage";
    // const size_t NDATA = 300000000;
    // BYTE* data = (BYTE*) malloc(NDATA);
    // char* b64 = (char*)0;
    // BYTE* dec = (BYTE*)0;
    // size_t dec_len;

    // encode_base64(data, /*sizeof(data)*/ NDATA, &b64);
    // puts(b64);

    // //decode_base64(b64, &dec, &dec_len);

    // //for (size_t i = 0; i < dec_len; i++) {
    // //    printf("%c", dec[i]);
    // //}
    // //printf("\n");

    // free(b64);
    // //free(dec);
    // free(data);

    return 0;
}
