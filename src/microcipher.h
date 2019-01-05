#pragma once

#include <iostream>

using namespace std;

const int MC_NJUMPS = 4;

typedef unsigned char BYTE;

typedef union microcipher_block {
    uint64_t uint64t;
    char chars[4];
} microcipher_block, MCBLOCK;

typedef struct microcipher_key {
    uint64_t jump1;
    uint64_t jump2;
    uint64_t jump3;
    uint64_t jump4;
} microcipher_key, MCKEY;

MCKEY& microcipher_check_make_key(const string& key_str);
void microcipher_process(const MCKEY&, istream&, ostream&);
