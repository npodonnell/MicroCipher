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

As discussed in the previous section, any form of key re-use is considered mis-use
of the cipher, and therefore will not be discussed. The remainder of this section
will concern attacks where the target has used the cipher in a way that is
considered safe.

That is:

1. They have used a cryptographically-secure randomly-generated key
2. They have not used this key before and will not use it again

Having said that, we'll begin with analyzing how MicroCipher *can* easily be broken
by using a pathologically-chosen weak key.

### Weak Key Attack

Although technically not an attack, we will begin by discussing weak keys and how they
can, if used, easily be exploited by an attacker if the conditions are right.

MicroCipher keys consist of 8 64-bit unsigned integers called "jumps". All such integers
are valid keys, thus The number of possible MicroCipher keys is:
 
> (2<sup>64</sup>)<sup>8</sup> = 2<sup>512</sup> = 13407807929942597099574024998205846127479365820592393377723561443721764030073546976801874298166903427690031858186486050853753882811946569946433649006084096

Although this is a massive number, there are certain keys within this keyspace which should
never be used. The worst possible key being all zeros, or (0,0,0,0,0,0,0,0). The reason
it is such a poor choice of key is not just because it's easily guessed but also because
it causes the ciphertext and plaintext to be the same. Consider the following 8-byte
string, which will be the plaintext in a 1-block encryption:

> ENCRYPTME

Interpreting this as an ASCII string and converting to hex, we get:

> 0x454e43525950544d45

Before the encryption process begins, MicroCipher initializes 8 64-bit unsigned integers 
called x1..x8 to the values of the 8 jump values j1..j8. Since j1..j8 are all zero, so
will x1..x8:


> x1 = 0x0000000000000000 x2 = 0x0000000000000000 x3 = 0x0000000000000000 x4 = 0x0000000000000000
> x5 = 0x0000000000000000 x6 = 0x0000000000000000 x7 = 0x0000000000000000 x8 = 0x0000000000000000
 
The ciphertext (C<sub>1</sub>) of the first block's plaintext is given by:

> C<sub>1</sub> = P<sub>1</sub> ⊕ (x<sub>1</sub> ⊕ x<sub>1</sub> ⊕ x<sub>1</sub> ⊕ x4 ⊕ x5 ⊕ x6 ⊕ x7 ⊕ x8)

Therefore, the ciphertext will be:

> C<sub>1</sub> = 0x454e43525950544d45 ⊕ (0x0000000000000000 ⊕ 0x0000000000000000 ⊕ 0x0000000000000000 ⊕ 0x0000000000000000 ⊕ 0x0000000000000000 ⊕ 0x0000000000000000 ⊕ 0x0000000000000000 ⊕ 0x0000000000000000)

> C<sub>1</sub> = 0x454e43525950544d45 ⊕ 0x0000000000000000

> C<sub>1</sub> = 0x454e43525950544d45

> C<sub>1</sub> = "ENCRYPTME"

