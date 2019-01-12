#include <iostream>
#include <cassert>
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
 * Encrypt a stream of data using key `key`. Data is read from `is` and written to `os`. Encryption
 * is performed in batches of blocks, with each batch containing at most BLOCKS_PER_BATCH blocks.
 *
 * @param key - Key used for encryption
 * @param is - Input stream
 * @param os - Output stream
 * @param startblock - 1-indexed block index of the first block which will be read from `is`
 */
void microcipher_encrypt(const MCKEY& key, istream& is, ostream& os, const uint64_t startblock) {
    MCBLOCK batch[BLOCKS_PER_BATCH];
    bool is_last = false;

    assert(startblock > 0);

    INIT_JUMPS

    do {
        is.read(batch[0].chars, BYTES_PER_BATCH);
        long bytes_read = is.gcount();
        long nblocks = bytes_read / sizeof(MCBLOCK);

        if (nblocks < BLOCKS_PER_BATCH) {
            // pad remaining bytes
            int remainder = bytes_read % sizeof(MCBLOCK);
            batch[nblocks].uint64t &= PADDING_ZERO[remainder];
            batch[nblocks].uint64t |= PADDING_PADS[remainder];
            nblocks++;
            is_last = true;
        }

        PROCESS_BATCH(batch, nblocks)

        os.write(batch[0].chars, nblocks * sizeof(MCBLOCK));
    } while (!is_last);
}

/**
 * Decrypt a stream of data using key `key`. Data is read from `is` and written to `os`
 * An optional 1-indexed start_block may be specified.
 *
 * Unlike encryption, decryption uses 2 batch buffers because there are cases where we can read in a full
 * batch but we don't know if there's more data to come. If there is no more data, we can assume the last block
 * as the padded block and un-pad it then exit, however if there's more data we just want to continue.
 *
 * Having 2 buffers solves this. We read in buffer n, then decrypt buffer n-1, and if buffer n is full we can tell
 * if it's the last of the data by checking for a block in buffer n. If we find at least 1 block in buffer n, we
 * can just decrypt all data in butter n-1 and continue. If not, we un-pad the last block in buffer n-1 and exit.
 *
 * A `parity` variable is used to keep track of whether we're on an even or an odd batch.
 *
 * @param key - Key used for decryption
 * @param is - Input stream
 * @param os - Output stream
 * @param startblock - 1-indexed block index of the first block which will be read from `is`
 */
void microcipher_decrypt(const MCKEY& key, istream& is, ostream& os, const uint64_t startblock) {
    MCBLOCK batch[2][BLOCKS_PER_BATCH];
    long nblocks[2];
    long bytes_out;
    bool parity = startblock % 2;
    bool is_last;

    assert(startblock > 0);

    INIT_JUMPS

    is.read(batch[parity][0].chars, BYTES_PER_BATCH);
    nblocks[parity] = is.gcount() / sizeof(MCBLOCK);
    parity = !parity;

    do {
        is.read(batch[parity][0].chars, BYTES_PER_BATCH);
        is_last = ((nblocks[parity] = is.gcount() / sizeof(MCBLOCK)) == 0);
        parity = !parity;

        // pblocks = process(ed) blocks (# of blocks to process)
        long pblocks = nblocks[parity];

        PROCESS_BATCH(batch[parity], pblocks)

        if (is_last) {
            // un-pad last block of this last batch
            long ilastblock = pblocks - 1;
            bytes_out = (ilastblock * sizeof(MCBLOCK)) + (long) batch[parity][ilastblock].chars[7];
            is_last = true;
        } else {
            // more batches to come
            bytes_out = (pblocks * sizeof(MCBLOCK));
        }

        os.write(batch[parity][0].chars, bytes_out);
    } while (!is_last);
}
