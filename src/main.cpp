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
bool check_get_is_encrypt(const variables_map& map) {
    auto encrypt = map["encrypt"].as<bool>();
    auto decrypt = map["decrypt"].as<bool>();
    
    if (!(encrypt ^ decrypt)) {
        throw error("Must specify either encrypt or decrypt operation, bot not both");
    }
    
    return encrypt;
}


/**
 * Checks if the map contains a microcipher key in a valid hex format. If so it's parsed
 * and converted to an MCKEY.
 */ 
MCKEY check_get_mckey(const variables_map& map) {
    auto key_str = map["key"].as<string>();
    return MCKEY{
        //FIXME
        1,2,3,4
    };
}


/**
 * Returns an input stream which may be a file stream if the filename is present in the optional,
 * or else std::cin. If there's a problem opening the file, an error will be thrown.
 * A std::pair is used to also return a boolean which indicates if it's an ifstream or not.
 */
pair<istream&, bool> get_input_stream(const optional<string>& in_filename) {
    if (in_filename == nullopt) {
        return pair<istream&, bool>(cin, false);
    }
    
    ifstream* infile = new ifstream(in_filename.value(), ios::in|ios::binary);
    if (!infile->good()) {
        throw error("Input file " + in_filename.value() + " is not good");
    }
    
    return pair<istream&, bool>(*infile, true);
}


/**
 * Returns an output stream which may be a file stream if the filename is present in the optional,
 * or else std::cout. If there's a problem opening the file, an error will be thrown.
 * A std::pair is used to also return a boolean which indicates if it's an ofstream or not.
 */
pair<ostream&, bool> get_output_stream(const optional<string>& out_filename) {
    if (out_filename == nullopt) {
        return pair<ostream&, bool>(cout, false);
    }
    
    ofstream* outfile = new ofstream(out_filename.value(), ios::out|ios::binary);
    if (!outfile->good()) {
        throw error("Output file " + out_filename.value() + " is not good");
    }
    
    return pair<ostream&, bool>(*outfile, true);
}


/**
 * Closes any input stream as returned from get_input_stream. There are 2 cases:
 * cin and file. Nothing needs to be done in the case of cin. If it's a file,
 * the file stream shall be closed and the associated ifstream destructed.
 */
void close_input_stream(const pair<istream&, bool>& input_pair) {
    istream& input_stream = input_pair.first;
    bool is_file = input_pair.second;
    
    if (is_file) {
        ((ifstream*)&input_stream)->close();
        delete &input_stream;
    }
}


/**
 * Closes any output stream as returned from get_output_stream. There are 2 cases:
 * cout and file. Nothing needs to be done in the case of cout. If it's a file,
 * the file stream shall be closed and the associated ofstream destructed.
 */
void close_output_stream(const pair<ostream&, bool>& output_pair) {
    ostream& output_stream = output_pair.first;
    bool is_file = output_pair.second;
    
    if (is_file) {
        ((ofstream*)&output_stream)->close();
        delete &output_stream;
    }
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
        
        auto is_encrypt = check_get_is_encrypt(map);
        MCKEY mckey = check_get_mckey(map);
        optional<string> in_filename = map.count("infile") ? optional<string>(map["infile"].as<string>()) : nullopt;
        optional<string> out_filename = map.count("outfile") ? optional<string>(map["outfile"].as<string>()): nullopt;
        
        if (in_filename != nullopt && out_filename != nullopt && in_filename.value() == out_filename.value()) {
            throw error("Input file cannot be same as output file");
        }
        
        pair<istream&, bool> input_pair = get_input_stream(in_filename);
        pair<ostream&, bool> output_pair = get_output_stream(out_filename);
        
        microcipher_process(mckey, input_pair.first, output_pair.first);
        
        close_input_stream(input_pair);
        close_output_stream(output_pair);
        
        return 0;
    } catch(const error& ex) {
        cerr << ex.what() << endl;
        cout << desc << endl;
        return 1;
    }
}
