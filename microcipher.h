#pragma once

#include <iostream>

using namespace std;

typedef unsigned char BYTE;
typedef uint64_t MCJUMP;
typedef uint64_t MCBLOCK;

typedef struct microcipher_key {
    MCJUMP jump1;
    MCJUMP jump2;
    MCJUMP jump3;
    MCJUMP jump4;
} microcipher_key;

typedef microcipher_key MCKEY;

void microcipher_process(const MCKEY&, const istream&, const ostream&);
