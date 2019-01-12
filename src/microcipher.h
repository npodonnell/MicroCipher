#pragma once

#include <iostream>

using namespace std;

const int BLOCKS_PER_BATCH = 65536;

typedef union microcipher_block {
    uint64_t uint64t;
    char chars[8];
} microcipher_block, MCBLOCK;

const int BYTES_PER_BATCH = BLOCKS_PER_BATCH * sizeof(MCBLOCK);

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

/**
 * Bitmasks for performing efficient padding
 *
 * NOTE: These are little endian and will need to be flipped around if run on a big-endian system
 */
const uint64_t PADDING_ZERO [] = {
        0x0000000000000000,
        0x00000000000000ff,
        0x000000000000ffff,
        0x0000000000ffffff,
        0x00000000ffffffff,
        0x000000ffffffffff,
        0x0000ffffffffffff,
        0x00ffffffffffffff
};

const uint64_t PADDING_PADS [] = {
        0x0000000000000000,
        0x0101010101010100,
        0x0202020202020000,
        0x0303030303000000,
        0x0404040400000000,
        0x0505050000000000,
        0x0606000000000000,
        0x0700000000000000
};

/**
 * Macros common to both encryption and decryption
 */
#define INIT_JUMPS \
uint64_t x1 = key.jump1 * startblock; \
uint64_t x2 = key.jump2 * startblock; \
uint64_t x3 = key.jump3 * startblock; \
uint64_t x4 = key.jump4 * startblock;

#define PROCESS_BATCH(batch, nblocks) \
for (long i = 0; i < nblocks; i++) { \
    batch[i].uint64t ^= (x1 ^ x2 ^ x3 ^ x4); \
    x1 += key.jump1; \
    x2 += key.jump2; \
    x3 += key.jump3; \
    x4 += key.jump4; \
} \

/**
 * Function declarations
 */
MCKEY& microcipher_check_make_key(const string&);
void microcipher_encrypt(const MCKEY&, istream&, ostream&, const uint64_t);
void microcipher_decrypt(const MCKEY&, istream&, ostream&, const uint64_t);
