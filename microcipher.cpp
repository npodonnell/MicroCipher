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
 * Encryption and decryption are the same operation, which we call "process"
 */ 
void microcipher_process(const MCKEY& mckey, istream& input_stream, ostream& output_stream) {
    MCBLOCK block;
    
    do {
        input_stream.read((char*)&block, sizeof(MCBLOCK));
        int bytes_read = input_stream.gcount();
        output_stream.write((char*)&block, bytes_read);
    } while(!input_stream.eof());
}
