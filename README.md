# MicroCipher

Extremely simple and fast streaming block cipher

## Modes Of Operation

* Counter mode

## Description Of The Cipher

### Key Structure

A MicroCipher key consists of 4 64-bit unsigned integers called "jumps". A jump is a random unsigned integer which is ideally:
* An odd prime
* Not equal to any of the other jumps

### Encryption

Plaintext is split into 64-bit (8 byte) blocks, 4 uint64s, which we call x1, x2, x3 and x4 are initalized to the values of the 4 jumps in the key.

Each block of plaintext is XORed with (x1 XOR x2 XOR x3 XOR x4) to produce the block's ciphertext. x1, x2, x3 and x4 then have the 4 jump values added to them again and the next block is processed.

If the data being encrypted is of fixed length, padding will be added to (or after) the last block.

### Decryption

Decryption is the same as encryption with the exception that the padding operation is reversed.