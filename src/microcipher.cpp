#include <iostream>
#include "microcipher.h"

using namespace std;

/**
 * Checks if the string is interpretable as a valid microcipher key, and if
 * so converts it into a MCKEY struct, otherwise throws an error.
 */
MCKEY& microcipher_check_make_key(const string& key_str) {
    //TODO
}


/**
 * Encryption and decryption are the same process with the exception of the last block.
 */ 
void microcipher_process(const MCKEY& mckey, const MCOP& mcop, istream& input_stream, ostream& output_stream) {
    MCBLOCK blocks[BLOCKS_PER_BATCH];
    const size_t batch_max_bytes = sizeof(MCBLOCK) * BLOCKS_PER_BATCH;
    size_t bytes_read;

    // initialize jumps
    uint64_t x1 = mckey.jump1;
    uint64_t x2 = mckey.jump2;
    uint64_t x3 = mckey.jump3;
    uint64_t x4 = mckey.jump4;

    do {
        input_stream.read((char*)blocks, batch_max_bytes);

        bytes_read = input_stream.gcount();
        size_t nblocks = bytes_read / sizeof(MCBLOCK);

        for (size_t i = 0; i < nblocks; i++) {
            blocks[i].uint64t ^= (x1 ^ x2 ^ x3 ^ x4);

            x1 += mckey.jump1;
            x2 += mckey.jump2;
            x3 += mckey.jump3;
            x4 += mckey.jump4;
        }

        if (bytes_read == batch_max_bytes) {
            output_stream.write((char*)blocks, bytes_read);
        }
    } while(!input_stream.eof());

    // TODO - handle padding
    if (mcop == MCOP_ENCRYPT) {

    }
}
