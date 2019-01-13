#include <iostream>
#include <cassert>
#include <sstream>
#include "microcipher.h"

using namespace std;

/**
 * Converts a string to an external form key. The conversion is essentially a type conversion and all
 * data from the string is preserved. This function is provided for convenience and the output will
 * almost always be immediately passed to microcipher_mcexkey_to_mckey.
 *
 * If `is_hex` is true the string is interpreted as a hex string.
 *
 * @param str - Input string
 * @param is_hex - True if str is a hex string
 * @return External form key
 */
MCEXKEY microcipher_string_to_mcexkey(const string& str, const bool is_hex) {
    if (is_hex) {
        long len = str.size();
        stringstream ss;
        int byte;

        if (len % 2 != 0) {
            throw std::invalid_argument("Invalid odd-length hex-encoded key");
        }

        for (BYTE c: str) {
            if (!(('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))) {
                throw std::invalid_argument("Invalid hex character: " + string(1, c));
            }
        }

        long nbytes = len / 2;
        MCEXKEY mckeyex(nbytes);

        for (long i = 0; i < nbytes; i++) {
            ss << hex << str.substr(i * 2, 2);
            ss >> byte;
            mckeyex[i] = byte;
        }
        return mckeyex;
    } else {
        return MCEXKEY(str.begin(), str.end());
    }
}

/**
 * Converts an external form key to an internal form key. An external form key is essentially an
 * array of zero or more bytes. To convert this to an internal form key the following steps are
 * performed:
 *
 * 1. The bytes are hashed into a 256-bit hash using a secure hashing algorithm
 *
 * 2. The 256 bits are split into 4 64-bit unsigned integers which are known as "jumps"
 *
 * @param mcexkey - External form key
 * @return Internal form key
 */
MCKEY microcipher_mcexkey_to_mckey(const MCEXKEY& mcexkey) {
    return MCKEY{
        //FIXME
        0x13dae87c0bc67012,
        0xba08cbb7108fac8e,
        0x901aa87bca109ab0,
        0x55980ccb8109aebb
    };
}

/**
 * Encrypt a stream of data using key `mckey`. Data is read from `is` and written to `os`. Encryption
 * is performed in batches of blocks, with each batch containing at most BLOCKS_PER_BATCH blocks.
 *
 * @param mckey - Key used for encryption
 * @param is - Input stream
 * @param os - Output stream
 * @param startblock - 1-indexed block index of the first block which will be read from `is`
 */
void microcipher_encrypt(const MCKEY& mckey, istream& is, ostream& os, const uint64_t startblock) {
    MCBLOCK batch[BLOCKS_PER_BATCH];
    bool is_last = false;

    assert(startblock > 0);

    INITIALIZE_JUMPS

    do {
        is.read(batch[0].chars, BYTES_PER_BATCH);
        long bytes_read = is.gcount();
        long nblocks = bytes_read / sizeof(MCBLOCK);

        if (nblocks < BLOCKS_PER_BATCH) {
            // pad remaining bytes
            long remainder = bytes_read % sizeof(MCBLOCK);
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
 *
 * Unlike encryption, decryption uses 2 batch buffers because there are cases where we can read in a full
 * batch but we don't know if there's more data to come. If there is no more data, we can assume the last block
 * as the padded block and un-pad it then exit, however if there's more data we just want to continue.
 *
 * Using 2 alternating buffers solves this. We read into buffer n then decrypt buffer n-1. If buffer n is empty
 * we know the padded block is in buffer n-1, so we un-pad it and we're finished, otherwise we continue looping.
 *
 * A `parity` variable is used to keep track of whether we're on an even or an odd batch.
 *
 * @param mckey - Key used for decryption
 * @param is - Input stream
 * @param os - Output stream
 * @param startblock - 1-indexed block index of the first block which will be read from `is`
 */
void microcipher_decrypt(const MCKEY& mckey, istream& is, ostream& os, const uint64_t startblock) {
    MCBLOCK batch[2][BLOCKS_PER_BATCH];
    long nblocks[2];
    long bytes_out;
    bool parity = startblock % 2;
    bool is_last;

    assert(startblock > 0);

    INITIALIZE_JUMPS

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
            long remainder = ((long) batch[parity][ilastblock].chars[7]) % 8L;
            bytes_out = (ilastblock * sizeof(MCBLOCK)) + remainder;
            is_last = true;
        } else {
            // more batches to come
            bytes_out = (pblocks * sizeof(MCBLOCK));
        }

        os.write(batch[parity][0].chars, bytes_out);
    } while (!is_last);
}
