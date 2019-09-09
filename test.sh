#!/usr/bin/env bash

# test against the system's `base64` program

error=0

# test encoding a few simple strings
for str in "" "a" "ab" "abc" "abcd" "abcde" "abcdef" "abcdefg" "abcdefgh" "abcdefghi"; do
    output_sys=$(echo -n $str | base64)
    output_fb64=$(echo -n $str | ./fast-base64)
    if [[ $output_sys != $output_fb64 ]]; then
        echo "Failed to encode $str - System output is $output_sys but our output is $output_fb64"
        error=1
    fi
done

# test decoding a few simple strings
for str in "" "YQ==" "YWI=" "YWJj" "YWJjZA==" "YWJjZGU=" "YWJjZGVm" "YWJjZGVmZw==" "YWJjZGVmZ2g=" "YWJjZGVmZ2hp"; do
    output_sys=$(echo -n $str | base64 -d)
    output_fb64=$(echo -n $str | ./fast-base64 -d)
    if [[ $output_sys != $output_fb64 ]]; then
        echo "Failed to decode $str - System output is $output_sys but our output is $output_fb64"
        error=1
    fi
done

exit $error
