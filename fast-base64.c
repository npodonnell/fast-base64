#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned char BYTE;

#include "lookup_tables.h"

/* 
    Buffer size is measured in chunks. Each chunk is either
    3 bytes of raw data, or 4 bytes of base64 characters.
*/
#define BUF_CHUNKS 100

const static size_t DATA_BUF_SIZE = BUF_CHUNKS * 3;
const static size_t B64_BUF_SIZE = BUF_CHUNKS * 4;


void encode_base64(
    const BYTE* data,
    const size_t len,
    char* out,
    size_t* nout)
{
    BYTE* dp = (BYTE*) data - 1;
    BYTE* dl = (BYTE*) dp + len;
    char* op = out - 1;

    /* main encoding loop */
    while (dp < dl) {
        *(++op) = C0_ENCODE_TABLE[*++dp];
        *(++op) = C1_ENCODE_TABLE[*((unsigned short*) dp )]; 
        *(++op) = C2_ENCODE_TABLE[*((unsigned short*) ++dp )];
        *(++op) = C3_ENCODE_TABLE[*++dp];
    }

    /* pad */
    switch (dp - dl) {
        case 1:
            /* 2/3 byte block - re-compute b64 character c2 with mask and add 1 padding character */
            *(op - 1) = C2_ENCODE_TABLE[*((unsigned short*)(dp - 1)) & 0x00FF];
            *(op) = '=';
            break;
        case 2:
            /* 1/3 byte block - re-compute b64 character c1 with mask and add 2 padding characters */
            *(op - 2) =  C1_ENCODE_TABLE[*((unsigned short*)(dp - 2)) & 0x00FF];
            *(op - 1) = '=';
            *(op) = '=';
            break;
    }
 
    *nout = (op - out) + 1;
}

void decode_base64(
    const char* b64,
    const size_t len,
    BYTE* out,
    size_t* nout)
{
    char* sp = (char*) b64 - 1;
    char* sl = (char*) sp + len;
    BYTE* op = out - 1;

    /* main decoding loop */
    while (sp < sl) {
        *(++op) = B0_DECODE_TABLE[*((unsigned short*)(++sp))];
        *(++op) = B1_DECODE_TABLE[*((unsigned short*)(++sp))];
        *(++op) = B2_DECODE_TABLE[*((unsigned short*)(++sp))];
        sp++;
    }

    /* up-pad - just reduce the # of bytes reported. No need to mess with the data. */
    if (*sp == '=') {
        if (*(sp - 1) == '=') {
            *nout = (op - out) - 1;
        } else {
            *nout = (op - out);
        }
    } else {
        *nout = (op - out) + 1;
    }
}

int main(
    int argc, 
    char** argv) 
{
    BYTE data_buf[DATA_BUF_SIZE];
    char b64_buf[B64_BUF_SIZE];
    size_t nread, nout;
    int encode = 0;

    if (encode) {
        while ((nread = fread(data_buf, 1, DATA_BUF_SIZE, stdin)) > 0) {
            encode_base64(data_buf, nread, b64_buf, &nout);
            fwrite(b64_buf, nout, 1, stdout);
        }
    } else {
        while ((nread = fread(b64_buf, 1, B64_BUF_SIZE, stdin)) > 0) {
            decode_base64(b64_buf, nread, data_buf, &nout);

            if (!write(1, data_buf, nout)) {
                fprintf(stderr, "oh");
            }
        }
    }
    puts("");

    return 0;
}
