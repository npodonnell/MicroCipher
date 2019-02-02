# Cryptanalysis

MicroCipher encrypts by XOR'ing a pseudo-random keystream with a plaintext 
to produce a ciphertext and decrypts by XOR'ing that same keystream with
the ciphertext to recover the plaintext.

If we encrypt a plaintext that is all zeros, the ciphertext will match the
keystream. If we can find a way of predicting the keystream, or reverse
engineering the keystream to get the key, we can successfully mount an attack.

## Chosen {Plain|Cipher}text attacks

Many successful attacks mounted against symmetric encryption schemes are
either CPAs (Chosen Plaintext Attacks) or CCAs (Chosen Ciphertext Attacks). 

Because MicroCipher treats encryption and decryption are the same operation,
with the exception of how padding is handled, we will use the term "CTA" (Chosen
Text Attack), when referring to  such attacks.

## The Importance Of Choosing A Unique Key

Many symmetric encryption schemes, notably AES, have a concept of an initalization
vector (IV) which is a randomly-chosen bitstring, equal in size to the block
size, which is not considered part of the key.

The purpose of an IV is to ensure that if the same plaintext is encrypted twice
using the same key by two different parties, they will produce two different
ciphertexts. MicroCipher does not have such a concept, therefore choosing a unique
key for every encryption is a requirement and the onus is on the user to meet this
requirement. There are no safety mechanisms build into the cipher to protect
against the dangers of key re-use.

If the same key is used to encrypt two different plaintexts which have some common
data, it's possible to deduce this fact; for example, supposing we possess the
ability to encrypt arbitrary texts with an unknown key, and we encrypt the
following 6-block plaintext P1 into a ciphertext C1:

```
[  B1  ][  B2  ][  B3  ][  B4  ][  B5  ][  B6  ]
Once upon a time a little boy designed a cipher.
```

We can analyze unknown ciphertexts, known to have been encrypted with the same
key, and check for substrings of bits in these ciphertexts which match the bits
in the same position in C1. This allows to deduce certain facts about the
corresponding plaintexts, such as allowing us to identify any plaintext which
begins with the string "Once upon a time".

For this reason, any kind of key re-use is considered mis-use. All keys should 
be ephemeral and used only once.

## Attack Vectors

### Key Re-Use

As discussed in the last section, any form of key re-use is considered mis-use, similar
to choosing an extremely easy-to-guess key, and for that reason it won't be
discussed.

