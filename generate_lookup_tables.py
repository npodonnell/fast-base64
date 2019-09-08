#!/usr/bin/env python3

b64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"

print("/* Auto-generated file. Generated with generate_lookup_tables.py */")

def swap_endianness(x):
    return (x << 8) | (x >> 8)


#
# 4 Encode Tables - one for each 6-bit base64 character
#

# c0 is a function of b0 only
print("const char C0_ENCODE_TABLE[] = {")
for i in range(256):
  c = b64_chars[i >> 2]
  print("'" + c + ("', " if i < 255 else "'"), end = "\n" if i % 16 == 15 else "")
print("};")
print()

# c1 is a function of b0 and b1
print("const char C1_ENCODE_TABLE[] = {")
for i in range(65536):
  c = b64_chars[(swap_endianness(i) >> 4) & 63]
  print("'" + c + ("', " if i < 65535 else "'"), end = "\n" if i % 16 == 15 else "")
print("};")
print()

# c2 is a function of b1 and b2
print("const char C2_ENCODE_TABLE[] = {")
for i in range(65536):
  c = b64_chars[(swap_endianness(i) >> 6) & 63]
  print("'" + c + ("', " if i < 65535 else "'"), end = "\n" if i % 16 == 15 else "")
print("};")
print()

# c3 is a function of b2 only
print("const char C3_ENCODE_TABLE[] = {")
for i in range(256):
  c = b64_chars[i & 63]
  print("'" + c + ("', " if i < 255 else "'"), end = "\n" if i % 16 == 15 else "")
print("};")
print()

#
# 3 Decode Tables - one for each 8-bit byte
#

for ibyte in range(3):
    ui16_map = {}
    for i in range(64):
        for j in range(64):
            # Make a 12-bit unsigned integer from i and j, which will
            # later be reduced down to 8 bits, in different ways
            # depending if we're decoding byte 0, 1 or 2 (see below)
            ui12 = (i * 64) + j

            # Now we reduce the 12-bit uint down to 8 bits
            if ibyte == 0:
                b = ui12 >> 4
            elif ibyte == 1:
                b = (ui12 >> 2) & 0xff
            else: # 3
                b = ui12 & 0xff

            # Interpret the ASCII representation of the 
            # first two base64 characters side-by-side as a uint16
            ui16 = (ord(b64_chars[j]) * 256) + ord(b64_chars[i])
            
            # put the b1 byte into the map, keyed by the uint16
            ui16_map[ui16] = b

    # iterate over all 65536 possible combinations of 2 bytes
    print("const BYTE B" + str(ibyte) + "_DECODE_TABLE[] = {")
    for i in range(65536):
        if i in ui16_map:
            print(hex(ui16_map[i]), end = "")
        else:
            # shouldn't ever look up this value unless there are invalid
            # b64 characters in the string. Just output a 0 in that case.
            print("0x00", end = "")
        print(", " if i < 65535 else "", end = "\n" if i % 16 == 15 else "")
    print("};")
    print()