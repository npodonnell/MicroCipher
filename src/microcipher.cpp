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
 * Encryption and decryption are the same process with the exception of the last block.
 */ 
void microcipher_process(const MCKEY& mckey, const MCOP& mcop, istream& input_stream, ostream& output_stream) {

}
