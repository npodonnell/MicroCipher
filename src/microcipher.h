#pragma once

#include <iostream>

using namespace std;

const int BLOCKS_PER_BATCH = 1000;

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

typedef enum microcipher_op {
    MCOP_ENCRYPT,
    MCOP_DECRYPT
} microcipher_op, MCOP;

MCKEY& microcipher_check_make_key(const string&);
void microcipher_process(const MCKEY&, const MCOP&, istream&, ostream&);
