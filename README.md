# MicroCipher

Extremely simple and fast streaming block cipher

## Modes Of Operation

* Counter mode

## Description Of The Cipher

### Key Structure

A MicroCipher key consists of 8 randomly chosen 64-bit unsigned integers called "jumps".

### Encryption

Plaintexts are measured in bytes, not bits.

Encryption may begin from any arbitrary 1-indexed block number. The encryption function takes 3 inputs: The plaintext, the key and the
starting block.

Before encryption begins, an internal state is initialized from the key. This internal state consists of 8 64-bit (8-byte) unsigned integers known as
`x[1..8]`. `x[1..8]` are initialized to `jump[1..8] * starting_block`.

Next, each block is read in. If the block is a full 8 bytes it's XORed with `x1 XOR x2 XOR x3 XOR x4 XOR x5 XOR x6 XOR x7 XOR x8`, then
`x[1..8]` each have `jump[1..8]` added to them, allowing overflow to occur.

When a block of length 0..7 is encountered, that is the last block and will be padded before the XORing. Padding works by filling each of the
remaining byte(s) in the block with the number of non-padding bytes, represented as an 8-bit integer. For example if 3 bytes were read and
they were: 
```
194 09 24
```

The block would have the remaining 5 bytes filled with `3` to make:
```
194 09 24 3 3 3 3 3

```

Once the padded block is encrypted the encryption is complete.

### Decryption

Decryption is the same as encryption with the exception that the padding operation is reversed. To identify the last
block an out-of-band signalling mechanism *is needed* since some valid unpadded blocks are also valid padded blocks so 
we can't determine the last block by examining its structure.

Typically a read-ahead mechanism will attempt to read block n, and if block n has data, it will decrypt block n-1 as a
non-terminal block. But if block n has no data, block n-1 is the last block so will be un-padded.

Once the last block is known the last byte is read as the `remainder`, and `remainder` bytes from that block are kept, thus
restoring the original length of the plaintext.

### Cryptanalysis

Not done yet