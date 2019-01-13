#include <iostream>
#include <fstream>
#include <string>
#include <typeinfo>
#include <utility>
#include <boost/program_options.hpp>
#include <experimental/optional>
#include "microcipher.h"

using namespace std;
using namespace std::experimental;
using namespace boost::program_options;

/**
 * Returns true if the encryption operation is specified only or false if the decryption
 * operation is specified only.
 */
microcipher_op check_get_operation(const variables_map& map) {
    auto encrypt_on = map["encrypt"].as<bool>();
    auto decrypt_on = map["decrypt"].as<bool>();
    
    if (!(encrypt_on ^ decrypt_on)) {
        throw error("Must specify either encrypt or decrypt operation, bot not both");
    }
    
    return encrypt_on ? MCOP_ENCRYPT : MCOP_DECRYPT;
}

/**
 * Checks if the map contains a valid microcipher key. A key
 */ 
MCKEY check_get_mckey(const variables_map& map) {
    auto str = map["key"].as<string>();
    auto is_hex = map["hexkey"].as<bool>();

    return microcipher_mcexkey_to_mckey(microcipher_string_to_mcexkey(str, is_hex));
}

/**
 * Returns an input stream which may be a file stream if the filename is present in the optional,
 * or else std::cin. If there's a problem opening the file, an error will be thrown.
 */
istream& get_input_stream(const optional<string>& in_filename) {
    if (in_filename == nullopt) {
        return cin;
    }
    
    ifstream* infile = new ifstream(in_filename.value(), ios::in|ios::binary);
    if (!infile->good()) {
        throw error("Input file " + in_filename.value() + " is not good");
    }
    
    return *infile;
}

/**
 * Returns an output stream which may be a file stream if the filename is present in the optional,
 * or else std::cout. If there's a problem opening the file, an error will be thrown.
 */
ostream& get_output_stream(const optional<string>& out_filename) {
    if (out_filename == nullopt) {
        return cout;
    }
    
    ofstream* outfile = new ofstream(out_filename.value(), ios::out|ios::binary);
    if (!outfile->good()) {
        throw error("Output file " + out_filename.value() + " is not good");
    }
    
    return *outfile;
}

/**
 * Close and delete `stream` if it's an ifstream, otherwise do nothing
 */
void try_close_ifstream(istream& stream) {
    try {
        dynamic_cast<ifstream&>(stream).close();
        delete& stream;
    } catch(std::bad_cast&) {}
}

/**
 * Close and delete `stream` if it's an ofstream, otherwise do nothing
 */
void try_close_ofstream(ostream& stream) {
    try {
        dynamic_cast<ofstream&>(stream).close();
        delete& stream;
    } catch(std::bad_cast&) {}
}

/**
 * Main function
 */
int main(int argc, char **argv) {
    // define some options
    options_description desc{"Options"};

    try {
        desc.add_options()
            ("help,h", "Help Screen")
            ("encrypt,e", bool_switch()->default_value(false), "Encrypt")
            ("decrypt,d", bool_switch()->default_value(false), "Decrypt")
            ("hexkey,x", bool_switch()->default_value(false), "Interpret key as hex")
            ("key,k", value<string>()->required(), "Key")
            ("infile,i", value<string>(), "Input File")
            ("outfile,o", value<string>(), "Output File");
        
        // read variables map
        variables_map map;
        store(parse_command_line(argc, argv, desc), map);
        
        // check if user needs help
        if (map.count("help")) {
            cout << desc << endl;
            return 0;
        }
        
        notify(map);
        
        MCKEY mckey = check_get_mckey(map);
        MCOP mcop = check_get_operation(map);

        optional<string> in_filename = map.count("infile") ? optional<string>(map["infile"].as<string>()) : nullopt;
        optional<string> out_filename = map.count("outfile") ? optional<string>(map["outfile"].as<string>()): nullopt;

        if (in_filename != nullopt && out_filename != nullopt && in_filename.value() == out_filename.value()) {
            throw error("Input file cannot be same as output file");
        }
        
        istream& is = get_input_stream(in_filename);
        ostream& os = get_output_stream(out_filename);

        switch(mcop) {
            case MCOP_ENCRYPT:
                microcipher_encrypt(mckey, is, os, 1);
                break;
            case MCOP_DECRYPT:
                microcipher_decrypt(mckey, is, os, 1);
                break;
        }

        try_close_ifstream(is);
        try_close_ofstream(os);

        return 0;
    } catch(const exception& ex) {
        cerr << ex.what() << endl;
        cout << desc << endl;
        return 1;
    }
}
