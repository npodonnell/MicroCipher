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
    return MCKEY{
        //FIXME
        1,2,3,4
    };
}


/**
 * Gets input file or nullopt
 */
optional<string> check_get_in_filename(const variables_map& map) {
    if (!map.count("infile")) {
        return nullopt;
    }
    
    return optional<string>(map["infile"].as<string>());
}


/**
 * Output filename will be the one specified in the options map, otherwise it will be
 * the input filename appended with either .enc or .dec depending on whether the operation
 * is encryption or decryption. If there's no input filename or output filename, the output
 * filename will become a nullopt and output will instead go to stdout.
 */
optional<string> check_get_out_filename(const variables_map& map, const bool is_encrypt, 
                              const optional<string>& in_filename) {
    string out_filename;
    
    if (map.count("outfile")) {
        out_filename = map["outfile"].as<string>();
    } else if (in_filename != nullopt){
        out_filename = in_filename.value() + (is_encrypt ? ".enc" : ".dec");
    } else {
        return nullopt;
    }
    
    if (in_filename == out_filename) {
        throw error("Input file cannot be same as output file");
    }
    
    return optional<string>(out_filename);
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
    
    ifstream* infile = new ifstream(in_filename.value());
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
    
    ofstream* outfile = new ofstream(out_filename.value());
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
        optional<string> in_filename = check_get_in_filename(map);
        optional<string> out_filename = check_get_out_filename(map, is_encrypt, in_filename);
        
        pair<istream&, bool> input_pair = get_input_stream(in_filename);
        pair<ostream&, bool> output_pair = get_output_stream(out_filename);
        
        // TODO: Encryption/Decryption
        //microcipher_process()
        
        
        close_input_stream(input_pair);
        close_output_stream(output_pair);
        
        return 0;
    } catch(const error& ex) {
        cerr << ex.what() << endl;
        cout << desc << endl;
        return 1;
    }
}
