# fast-base64

*Noel P O'Donnell, Sep 2019*

**fast-base64** is a small, simple, fast and novel implementation of base64 encoding/decoding.

## Why write another base64 implementation?

I was working on another much larger C++ project, and I happened to need to encode/decode some base64. Since I was making heavy use of the excellent boost libraries, I assumed boost would have some nice base64 classes, just like it has nice classes for working with JSON, networking, and lots of other stuff, however I soon discovered that base64 encoding using boost is not for the faint-hearted and it does not contain a simple "base64Encode" and "base64Decode" function, as I had wished.

I decided that the only logical course of action was to write my own base64 implementation completely from scratch in C instead of C++, and later write a C++ wrapper to integrate it into the project.

## How does it work?

[Most](https://github.com/search?l=C&q=base64&type=Repositories) base64 implementations use bit-shifting and masking operations to chop up the input bytes to 6-bit numbers which get mapped to a base64 alphabet,  `ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/`.

fast-base64 also uses these operations but performs them *just once* at compile time, pre-computing their results into several large lookup tables which are used during run time. The lookup tables are generated by the `generate_lookup_tables.py` python script which outputs a c header file `lookup_tables.h`.

### Encoding

fast-base64 splits the input data into 24-bit blocks, each block being split into 3 bytes labelled B0..B2, and outputs 4 base64 characters per block, each character encoding 6 logical bits:

```
bits:      [a][b][c][d][e][f][g][h][i][j][k][l][m][n][o][p][q][r][s][t][u][v][w][x]
bytes:     [          B0          ][          B1          ][          B2          ]
b64 chars: [       C0       ][       C1       ][       C2       ][       C3       ]
```

From the diagram we can see that b64 character `C0` encodes bits a..f of the block, which are all contained within byte `B0`, therefore `C0` is a function of `B0`. `C1` overlaps bytes `B0` and `B1` so is a function of these 2 bytes, `C2` is a function of `B1` and `B2`, and finally `C3` is a function of `B2` only.

To calculate `C0`, a 256-entry lookup table is consulted, based on the value of byte `B0`, as is the case for `C3` which depends on the value of `B2`. Since `C1` and `C2` they both depend on two different bytes, they each use 65536-entry lookup tables.

The keys of these lookup tables (technically arrays) but the values of the bytes being encoded, and the values are the *ASCII representation* of the base64 substrings they generate. 

**Here's the clever bit**: fast-base64 skips the intermediate steps of generating the 6-bit numbers using bit-shifting and masking, and indexing into an alphabet table to find the corerct base64 char, but instead *directly maps* the input bytes to the ASCII of the base64 characters they generate. Basically it asks the lookup table "If I have this byte as my `B0`, what will be the ASCII of my `C0`?", and "If I have this `B0` and this `B1`, what is the ASCII of my `C1`? , etc.

This means that the critical encoding loop is very simple and contains just a few assignment, increment, and dereference operations:

```
        *op++ = C0_ENCODE_TABLE[*dp];
        *op++ = C1_ENCODE_TABLE[*((unsigned short*) dp++ )]; 
        *op++ = C2_ENCODE_TABLE[*((unsigned short*) dp++ )];
        *op++ = C3_ENCODE_TABLE[*dp++];
```

### Decoding

decoding is similar to encoding in that lookup tables are again used. This time we have 3 lookup tables, one for each of the bytes `B0`, `B1` and `B2`, however all these tables are 65536-entry, because each of `B0`, `B1` and `B2` depend on exactly 2 base64 characters.

The algorithm casts pairs of base64 ASCII characters which appear side-by-side into `unsigned short`s and uses these as the keys to the decode tables. Since most ASCII characters never appear in valid base64, the majority of entries in these tables are unused and set to zero; however the ones that are consulted directly give back the byte that would have been encoded into these two base64 ASCII characters. Again: no ASCII decoding, no masking, no bit-shifting, or "joining". Just a few assignments, increments and de-references:

```
        *op++ = B0_DECODE_TABLE[*((unsigned short*) sp++ )];
        *op++ = B1_DECODE_TABLE[*((unsigned short*) sp++ )];
        *op++ = B2_DECODE_TABLE[*((unsigned short*) sp++ )];
        sp++;
```

### Padding

When the end of the input data is reached, the base64 standard requires that some padding bytes are added if the length of the input data is not a multiple of 3. The `=` character is used as the padding character.

The last block will be handled differently in both encoding and decoding to ensure no reads are made to uninitalized memory and no writes are made to unallocated memory.

## Performance

Performance is comparable to the `base64` program from GNU coreutils which ships with most Linux distros, however this doesn't indicate much since the speed of piping in the data is much slower compared to the speed of processing.

I did test the performance using in-memory arrays of random data and compared to the fastest base64 implementation I could find, which is [this AVX2-accelerated implementation](https://github.com/lemire/fastbase64/blob/master/src/fastavxbase64.c).

fast-base64 took about 2X the time of this implementation, which is impressive since fast-base64 does not contain any SIMD optimizations.
