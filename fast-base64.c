#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>

typedef unsigned char BYTE;

#include "lookup_tables.h"

/* 
    Buffer size is measured in chunks. Each chunk is either
    3 bytes of raw data, or 4 bytes of base64 characters.
*/
#define BUF_CHUNKS 10000

const static size_t DATA_BUF_SIZE = BUF_CHUNKS * 3;
const static size_t B64_BUF_SIZE = BUF_CHUNKS * 4;


void encode_base64(
    const BYTE* data,
    const size_t len,
    char* out,
    size_t* nout)
{
    BYTE* dp = (BYTE*) data;
    BYTE* dll = (BYTE*) (dp + len);
    BYTE* dl = dll - 3;
    char* op = out;

    /* main encoding loop */
    while (dp < dl) {
        *op++ = C0_ENCODE_TABLE[*dp];
        *op++ = C1_ENCODE_TABLE[*((unsigned short*) dp++ )]; 
        *op++ = C2_ENCODE_TABLE[*((unsigned short*) dp++ )];
        *op++ = C3_ENCODE_TABLE[*dp++];
    }

    *op++ = C0_ENCODE_TABLE[*dp];

    /* last block */
    switch (dll - dp) {
        case 1:
            /* last block contains 1 byte */
            *op++ = C1_ENCODE_TABLE[*((unsigned short*)dp) & 0x00FF];
            *op++ = '=';
            *op = '=';
            break;
        case 2:
            /* last block contains 2 bytes */
            *op++ = C1_ENCODE_TABLE[*((unsigned short*) dp++ )]; 
            *op++ = C2_ENCODE_TABLE[*((unsigned short*) dp) & 0x00FF];
            *op = '=';
            break;
        case 3:
            /* last block contains 3 bytes */
            *op++ = C1_ENCODE_TABLE[*((unsigned short*) dp++ )]; 
            *op++ = C2_ENCODE_TABLE[*((unsigned short*) dp++ )];
            *op = C3_ENCODE_TABLE[*dp];
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
    char* sp = (char*) b64;
    char* sl = (char*) sp + len;
    BYTE* op = out;

    /* main decoding loop */
    while (sp < sl) {
        *op++ = B0_DECODE_TABLE[*((unsigned short*) sp++ )];
        *op++ = B1_DECODE_TABLE[*((unsigned short*) sp++ )];
        *op++ = B2_DECODE_TABLE[*((unsigned short*) sp++ )];
        sp++;
    }

    /* un-pad - just reduce the # of bytes reported. No need to mess with the data. */
    if (*(sp - 1) == '=') {
        if (*(sp - 2) == '=') {
            *nout = (op - out) - 2;
        } else {
            *nout = (op - out) - 1;
        }
    } else {
        *nout = (op - out);
    }
}

int main(
    int argc, 
    char** argv) 
{
    BYTE data_buf[DATA_BUF_SIZE];
    char b64_buf[B64_BUF_SIZE];
    size_t nread, nout;
    int encode = 1;
    int opt;

    while ((opt = getopt(argc, argv, "d")) != -1)
        switch (opt) {
            case 'd':
                encode = 0;
                break;
            default:
                exit(-1);
        }

    if (encode) {
        while ((nread = read(0, data_buf, DATA_BUF_SIZE)) > 0) {
            encode_base64(data_buf, nread, b64_buf, &nout);
            if (!write(1, b64_buf, nout)) {
                fprintf(stderr, "error writing base64 data to stdout");
            }
        }

        puts("");

    } else {
        while ((nread = read(0, b64_buf, B64_BUF_SIZE)) > 0) {
            decode_base64(b64_buf, nread, data_buf, &nout);
            if (!write(1, data_buf, nout)) {
                fprintf(stderr, "error writing binary data to stdout");
            }
        }
    }
    

    return 0;
}
