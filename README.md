# fast-base64

Noel O'Donnell, Sep 2019


## What is this?

This is a small, simple, fast implementation of base64 encoding/decoding.

## How does it work?

[Most](https://github.com/search?l=C&q=base64&type=Repositories) base64 implementations use bit-shifting and masking operations to chop up the input bytes to 6-bit numbers which get mapped to a base64 alphabet,  `ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/`.
