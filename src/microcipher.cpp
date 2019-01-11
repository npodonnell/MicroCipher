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
    MCBLOCK batches[2][BLOCKS_PER_BATCH];
    long bytes_read[2];
    const size_t bytes_in_full_batch = BLOCKS_PER_BATCH * sizeof(MCBLOCK);
    size_t iblock = 1;
    size_t parity;

    // initialize jumps
    uint64_t x1 = mckey.jump1 * iblock;
    uint64_t x2 = mckey.jump2 * iblock;
    uint64_t x3 = mckey.jump3 * iblock;
    uint64_t x4 = mckey.jump4 * iblock;

    parity = iblock % 2;
    input_stream.read((char*)batches[parity], bytes_in_full_batch);
    bytes_read[parity] = input_stream.gcount();

    while (true) {
        input_stream.read((char*)batches[~parity], bytes_in_full_batch);
        bytes_read[~parity] = input_stream.gcount();

        bool all_read = input_stream.eof();

        if (!all_read && bytes_read[parity] == bytes_in_full_batch) {
            // encrypt full batch
        } else {
            // previous batch was last
        }

        if (input_stream.eof()) {
            break;
        }
    }
}
