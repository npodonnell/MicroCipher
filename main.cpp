#include <iostream>
#include <fstream>
#include <string>
#include <boost/program_options.hpp>
#include <experimental/optional>
#include "microcipher.h"

using namespace std;
using namespace std::experimental;
using namespace boost::program_options;

bool file_exists(const string& filename) {
    ifstream infile(filename.c_str());
    return infile.good();
}

bool check_get_is_encrypt(const variables_map& map) {
    auto encrypt = map["encrypt"].as<bool>();
    auto decrypt = map["decrypt"].as<bool>();
    
    if (!(encrypt ^ decrypt)) {
        throw error("Must specify either encrypt or decrypt operation, bot not both");
    }
    
    return encrypt;
}

optional<string> check_get_in_filename(const variables_map& map) {
    if (!map.count("infile")) {
        return nullopt;
    }
    
    auto in_filename = map["infile"].as<string>();
    
    if (!file_exists(in_filename)) {
        throw error("Cannot read the file:" + in_filename);
    }
    
    return optional<string>(in_filename);
}

/**
 * Output filename will be the one specified in the options map, otherwise it will be
 * the input filename appended with either .enc or .dec depending on whether the operation
 * is encryption or decryption. If there's no input filename, the output filename will also
 * be made null and output will instead go to stdout.
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

int main(int argc, char **argv) {
    // define some options
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help Screen")
        ("encrypt,e", bool_switch()->default_value(false), "Encrypt")
        ("decrypt,d", bool_switch()->default_value(false), "Decrypt")
        ("infile,i", value<string>(), "Input File")
        ("outfile,o", value<string>(), "Output File");
    
    try {
        // read variables map
        variables_map map;
        store(parse_command_line(argc, argv, desc), map);
        
        
        // check if user needs help
        if (map.count("help")) {
            COUT << desc << ENDL;
            return 0;
        }
        
        notify(map);
        
        auto is_encrypt = check_get_is_encrypt(map);
        optional<string> in_filename = check_get_in_filename(map);
        optional<string> out_filename = check_get_out_filename(map, is_encrypt, in_filename);
        
        if (in_filename == nullopt) {
            COUT << "infile is nullopt" << ENDL;
        } else {
            COUT << "infile is " << in_filename.value() << ENDL;
        }

        if (out_filename == nullopt) {
            COUT << "outfile is nullopt" << ENDL;
        } else {
            COUT << "outfile is " << out_filename.value() << ENDL;
        }
        
        return 0;
    } catch(const error& ex) {
        CERR << ex.what() << ENDL;
        COUT << desc << ENDL;
        return 1;
    }
}
