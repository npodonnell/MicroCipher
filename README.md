# MicroCipher


Extremely simple and fast streaming block cipher

---

<span style="color:red">This cryptosystem is very simple and has no mechanism for message authentication or key reuse protection,  in addition the cipher itself has not had any rigourous mathematical analysis. DO NOT use it for any purpose other than acedemic !!</span>

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
`x[1..8]`. `x[1..8]` are initialized to `jump[1..8] * starting_block`. The starting block is 1-indexed.

Encryption proceeds in batches of several 8-byte blocks. The number of batches per block is tuneable. Each block is 
XOR'ed with a pseudo-random bitmask which is generated from `x[1..8]`.

With each block, `x[1..8]` have the corresponding jump values `jump[1..8]` added to them allowing overflow to happen.

When the last block of plaintext is reached, a padding block *must* be appended to the end of the ciphertext.

Padding works by filling each of the remaining byte(s) in the last block with the number of non-padding bytes, represented
as an 8-bit integer. For example if 3 bytes were read and they were: 
```
194 09 24
```

The block would have the remaining 5 bytes filled with `3` to make:
```
194 09 24 3 3 3 3 3

```

If the last block is exactly 8 bytes, a new block is created consisting entirely of padding bytes.
 
### Decryption

Decryption is the same as encryption with the exception that the padding operation is reversed. 

A valid padded block is any block where the last `n` bytes are each equal to `8 - n` and n is in the range [1,8].

However this is not sufficient to identify the last block since every valid padded block is also a valid unpadded block.

To identify the end of the stream, block n will be read and if block n has data, block n-1 will be decrypted as a 
non-terminal block. But if block n has no data, then block n-1 must be the last block so will be un-padded after
decryption.

The un-padding works as follows:

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

See [Cryptanalysis](./CRYPTANALYSIS.md)

## Usage

Encrypt a file:

```
microcipher --encrypt --key abc123 --infile file --outfile file.enc
```

Decrypt a file:

```
microcipher --decrypt --key abc123 --infile file.enc --outfile file.dec
```

## Recipes

### Microcipher/Netcat encrypted tunnel


Listener:
```
nc -l -p 12345 | microcipher -d -k abc123
```

Sender:
```
microcipher -e -k abc123 | nc localhost 12345
```
