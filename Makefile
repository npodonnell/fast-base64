all: lookup fast-base64

fast-base64: fast-base64.c lookup_tables.h
	gcc fast-base64.c -o fast-base64 -O3 -s

lookup: generate_lookup_tables.py
	./generate_lookup_tables.py > lookup_tables.h
