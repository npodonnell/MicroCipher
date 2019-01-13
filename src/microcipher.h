#pragma once

#include <iostream>
#include <vector>

using namespace std;

const int BLOCKS_PER_BATCH = 131072;

typedef unsigned char BYTE;

typedef enum microcipher_op {
    MCOP_ENCRYPT,
    MCOP_DECRYPT
} microcipher_op, MCOP;

typedef union microcipher_block {
    uint64_t uint64t;
    char chars[8];
    BYTE bytes[8];
} microcipher_block, MCBLOCK;

typedef vector<BYTE> microcipher_ex_key, MCEXKEY;

typedef struct microcipher_key {
    uint64_t jump1;
    uint64_t jump2;
    uint64_t jump3;
    uint64_t jump4;
    uint64_t jump5;
    uint64_t jump6;
    uint64_t jump7;
    uint64_t jump8;
} microcipher_key, MCKEY;

const int BYTES_PER_BATCH = BLOCKS_PER_BATCH * sizeof(MCBLOCK);

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
#define INITIALIZE_JUMPS \
uint64_t x1 = mckey.jump1 * startblock; \
uint64_t x2 = mckey.jump2 * startblock; \
uint64_t x3 = mckey.jump3 * startblock; \
uint64_t x4 = mckey.jump4 * startblock; \
uint64_t x5 = mckey.jump5 * startblock; \
uint64_t x6 = mckey.jump6 * startblock; \
uint64_t x7 = mckey.jump7 * startblock; \
uint64_t x8 = mckey.jump8 * startblock;

#define PROCESS_BATCH(batch, nblocks) \
for (long i = 0; i < nblocks; i++) { \
    batch[i].uint64t ^= (x1 ^ x2 ^ x3 ^ x4 ^ x5 ^ x6 ^ x7 ^ x8); \
    x1 += mckey.jump1; \
    x2 += mckey.jump2; \
    x3 += mckey.jump3; \
    x4 += mckey.jump4; \
    x5 += mckey.jump5; \
    x6 += mckey.jump6; \
    x7 += mckey.jump7; \
    x8 += mckey.jump8; \
} \

/**
 * Function declarations
 */
MCEXKEY microcipher_string_to_mcexkey(const string&, const bool);
MCKEY microcipher_mcexkey_to_mckey(const MCEXKEY&);
void microcipher_encrypt(const MCKEY&, istream&, ostream&, const uint64_t);
void microcipher_decrypt(const MCKEY&, istream&, ostream&, const uint64_t);
