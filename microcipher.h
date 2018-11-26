#pragma once

#include <iostream>

using namespace std;

const int MC_NJUMPS = 4;

typedef unsigned char BYTE;
typedef uint64_t MCJUMP;

typedef struct microcipher_block {
    BYTE bytes[sizeof(MCJUMP)];
} microcipher_block, MCBLOCK;

typedef struct microcipher_key {
    MCJUMP jumps[MC_NJUMPS];
} microcipher_key, MCKEY;

MCKEY& microcipher_check_make_key(const string& key_str);
void microcipher_process(const MCKEY&, istream&, ostream&);
