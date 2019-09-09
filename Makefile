CC=gcc # hint: try clang

all: lookup fast-base64 test

test: test.sh
	./test.sh

fast-base64: fast-base64.c lookup_tables.h
	$(CC) fast-base64.c -o fast-base64 -O3 -s

lookup: generate_lookup_tables.py
	./generate_lookup_tables.py > lookup_tables.h

clean:
	rm lookup_tables.h fast-base64
