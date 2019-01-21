# MicroCipher

Extremely simple and fast streaming block cipher

## Modes Of Operation

* Counter mode

## Description Of The Cipher

### Key Structure

A MicroCipher key consists of 8 randomly chosen 64-bit unsigned integers called "jumps".

### Encryption

MicroCipher encrypts by XORing an N-byte plaintext with an N-byte pseudo-random keystream. 
Encryption may begin from any arbitrary 1-indexed block number so may be parallelized. 

The encryption function takes 3 inputs: The plaintext, the key and the
starting block.

Before encryption begins, an internal state is initialized from the key. This internal state consists of 8 64-bit (8-byte) unsigned integers known as
`x[1..8]`. `x[1..8]` are initialized to `jump[1..8] * starting_block`.

Next, each block is read in. If the block is a full 8 bytes it's encrypted immediately, if it's 0..7 bytes it's the last block and needs
to be padded before encryption.
 
Padding works by filling each of the remaining byte(s) in the block with the number of non-padding bytes, represented
as an 8-bit integer. For example if 3 bytes were read and they were: 
```
194 09 24
```

The block would have the remaining 5 bytes filled with `3` to make:
```
194 09 24 3 3 3 3 3

```
 
Next the block is encrypted. Encryption is is done by XORing the (possibly padded) block with:

```
x1 XOR x2 XOR x3 XOR x4 XOR x5 XOR x6 XOR x7 XOR x8
```

Next the jumps are adjusted by adding each of `jump[1..8]` to  `x[1..8]`, allowing overflow to occur.

### Decryption

Decryption is the same as encryption with the exception that the padding operation is reversed. 

To identify the last
block an out-of-band signalling mechanism *is needed* since every valid padded block is also a valid un-padded block
therefore we can not identify the last block by examining its structure.

Typically a read-ahead mechanism will attempt to read block n, and if block n has data, it will decrypt block n-1 as a
non-terminal block. But if block n has no data, then block n-1 must be the last block so will be un-padded after
decryption.

The last byte of the last block is known as the `remainder`, and the first `remainder mod 8` bytes from that block are kept, thus
restoring the original length of the plaintext. For example if the last block after decryption was:

```
84 82 85 77 80 13 13 13 13
```

The last byte is 13. 

13 mod 8 = 5

So we keep the first 5 bytes, giving:

```
84 82 85 77 80
```

## Cryptanalysis

Not done yet .....  DON'T USE THIS CIPHER FOR ANYTHING.

## Usage

Encrypt a file called `file` to a file called `file.enc`:

```
microcipher --encrypt --key abc123 --infile file --outfile file.enc
```

Decrypt a file:

```
./microcipher --decrypt --key abc123 --infile file.enc --outfile file.dec
```

## Recipes

### Microcipher/Netcat encrypted tunnel


Listener:
```
nc -l -p 12345 | ./microcipher -d -k abc123
```

Sender:
```
./microcipher -e -k abc123 | nc localhost 12345
```
