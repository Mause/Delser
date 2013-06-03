/*
http://www.brandonstaggs.com/2007/07/26/implementing-a-partial-serial-number-verification-system-in-delphi/
*/
#include <iostream>
#include <sstream>
#include <iomanip>

#include <memory>
#include <algorithm>

#include <vector>
#include <array>
#include <tuple>

#include "Delser.hpp"

int Delser::_get_key_byte(int seed, int a, int b, int c) {
    a = a % 25;
    b = b % 3;
    long int result;
    if (a % 2 == 0) {
        result = ((seed << a) & 0x000000FF) ^ ((seed << b) | c);
    } else {
        result = ((seed << a) & 0x000000FF) ^ ((seed << b) & c);
    }
    return result;
}

std::string Delser::get_checksum(std::string string) {
    string = string_toupper(string);
    int i = 0;
    int left = 0x0056;
    int right = 0x00AF;
    if (string.length() > 0) {
        for (int i=0; i<string.length(); i++) {
            right = right + int(string[i]);
            if (right > 0x00FF) {
                right -= 0x00FF;
            }
            left += right;
            if (left > 0x00FF) {
                left -= 0x00FF;
            }
        }
    }
    std::string result = int_to_hex((left >> 8) + right, false, 4);
    return string_toupper(result);
}


std::string Delser::make_key(int seed) {
    std::vector<std::string> key;

    // the key string begins with a hexadecimal string of the seed
    key.push_back(int_to_hex(seed, false, 0));

    // Fill keybytes with values derived from seed.
    // The parameters used here must be exactly the same
    // as the ones used in the check_key function.
    // A real key system should use more than four bytes.
    // then is followed by hexadecimal strings of each byte in the key
    for (int cur_seq=0; cur_seq<sequences.size(); cur_seq++) {
        sequence cur_seq_a = sequences[cur_seq];
        int cur_key_byte = _get_key_byte(seed, std::get<0>(cur_seq_a), std::get<1>(cur_seq_a), std::get<2>(cur_seq_a)) / 2;
        std::string cur_byte = int_to_hex(cur_key_byte, false, 0);
        key.push_back(cur_byte);
    }

    std::string skey = format_result(key);

    // add checksum to key string
    std::string checksum = get_checksum(skey);
    skey += "-" + checksum;

    //std::cout << "Key; " << skey << " Checksum; " << checksum << std::endl;
    return skey;
}

bool Delser::check_key_checksum(std::string skey) {

    if (std::count(skey.begin(), skey.end(), '-') != (sequences.size() + 1)) {
        throw key_invalid("wrong number of sections");//; %d <> %d", 
//            std::count(skey.begin(), skey.end(), '-') + 1, skey);
    }
    // remove cosmetic hyphens and normalize case
    std::vector<std::string> key = split(string_toupper(skey), '-');
    
    // last four characters are the checksum
    std::string grabbed_checksum = key[key.size() - 1];
    
    // grab checksum
    std::vector<std::string> key_minus_checksum(key.begin(), key.end() - 1);

    // join vector with '-'
    std::string s = vector_to_string(key_minus_checksum, '-');
    
    // compare the supplied checksum against the real checksum for
    // the key string.
    if (grabbed_checksum != get_checksum(s)) {
        throw key_bad_checksum("Incorrect checksum");//; %s is not equal to %s", 
            //grabbed_checksum, get_checksum(s));
    }

    return true;
}

bool Delser::check_key(std::string key) {
    // test against blacklist
    if (!blacklist.empty()) {
        if (std::find(blacklist.begin(), blacklist.end(), string_toupper(key)) != blacklist.end()) {
            throw key_blacklisted(key);
        }
    }

    check_key_checksum(key);

    // remove cosmetic hyphens and normalize case
    auto skey = split(string_toupper(key), '-');

    // At this point, the key is either valid or forged,
    // because a forged key can have a valid checksum.
    // We now test the "bytes" of the key to determine if it is
    // actually valid.

    // extract the seed from the supplied key string
    int seed = hex_str_to_int(skey[0]);

    sequence selected_sequence = sequences[byte_to_check];

    std::string key_byte = string_toupper(skey[byte_to_check + 1]);
    std::string generated_byte = int_to_hex(_get_key_byte(seed,
       std::get<0>(selected_sequence),
       std::get<1>(selected_sequence),
       std::get<2>(selected_sequence)) / 2, false, 0);
    
    if (pad_with(key_byte, '0', generated_byte.length()) != pad_with(string_toupper(generated_byte), '0', 4)) {
        throw key_phony("Phony key");//skey);
    }

    // If we get this far, then it means the key is either good, or was made
    // with a keygen derived from "this" release.
    return true;
}

std::string Delser::pad_with(std::string str, char with, int size) {
    std::stringstream stream;
    
    if (str.length() >= size) {
        return str;
    }
    stream << std::setfill(with) << std::setw(size) << str;
    return stream.str();
}

std::string Delser::format_result(std::vector<std::string>key) {
    std::string seed = pad_with(key[0].substr(2, key[0].length()), '0', 4);
    std::string key_bytes;

    for (int x=1; x<key.size(); x++) {
        key_bytes += '-' + pad_with(key[x].substr(2, key[x].length()), '0', 4);
    }
    
    return seed + key_bytes;
}

std::string Delser::vector_to_string(std::vector<std::string> v, char sep) {
    std::stringstream ss;
    for(size_t i=0; i < v.size(); ++i) {
        if(i!=0) {
            ss << sep;
        }
        ss << v[i];
    }
    return ss.str();
}

std::string Delser::string_toupper(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

std::vector<std::string> &Delser::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> Delser::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}
 
