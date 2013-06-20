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
#include "utils.cpp"

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
    string = utils::upper(string);
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
    std::string result = utils::int_to_hex((left >> 8) + right);
    return utils::upper(result);
}


std::string Delser::make_key(int seed) {
    std::vector<std::string> key;

    // the key string begins with a hexadecimal string of the seed
    key.push_back(utils::int_to_hex(seed));

    // Fill keybytes with values derived from seed.
    // The parameters used here must be exactly the same
    // as the ones used in the check_key function.
    // A real key system should use more than four bytes.
    // then is followed by hexadecimal strings of each byte in the key
    for (std::vector<sequence>::iterator cur_seq = sequences.begin(); cur_seq != sequences.end(); ++cur_seq) {
        int cur_key_byte = _get_key_byte(seed, std::get<0>(*cur_seq), std::get<1>(*cur_seq), std::get<2>(*cur_seq)) / 2;
        std::string cur_key_byte_str = utils::int_to_hex(cur_key_byte);
        key.push_back(cur_key_byte_str);
    }
    std::string skey = format_result(key);

    // add checksum to key string
    std::string checksum = get_checksum(skey);
    skey += "-" + checksum;

    return skey;
}

bool Delser::check_key_checksum(std::string skey) {
    if (std::count(skey.begin(), skey.end(), '-') != (sequences.size() + 1)) {
        throw key_invalid("wrong number of sections");//; %d <> %d", 
//            std::count(skey.begin(), skey.end(), '-') + 1, skey);
    }

    // remove cosmetic hyphens and normalize case
    std::vector<std::string> key = utils::split(utils::upper(skey), '-');
    
    // last four characters are the checksum
    std::string grabbed_checksum = key[key.size() - 1];
    
    // grab checksum
    std::vector<std::string> key_minus_checksum(key.begin(), key.end() - 1);

    // join vector with '-'
    std::string s = utils::vector_to_string(key_minus_checksum, '-');
    
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
        if (std::find(blacklist.begin(), blacklist.end(), utils::upper(key)) != blacklist.end()) {
            throw key_blacklisted(key);
        }
    }

    check_key_checksum(key);

    // remove cosmetic hyphens and normalize case
    auto skey = utils::split(utils::upper(key), '-');

    // At this point, the key is either valid or forged,
    // because a forged key can have a valid checksum.
    // We now test the "bytes" of the key to determine if it is
    // actually valid.

    // extract the seed from the supplied key string
    int seed = utils::hex_to_int(skey[0]);

//    throw Exception()
    sequence selected_sequence = sequences[byte_to_check];

    std::string key_byte = utils::upper(skey[byte_to_check + 1]);
    std::string generated_byte = utils::int_to_hex(_get_key_byte(seed,
       std::get<0>(selected_sequence),
       std::get<1>(selected_sequence),
       std::get<2>(selected_sequence)) / 2);
    
    if (utils::rjust(key_byte, '0', generated_byte.length()) != utils::rjust(utils::upper(generated_byte), '0', 4)) {
        throw key_phony("Phony key");//skey);
    }

    // If we get this far, then it means the key is either good, or was made
    // with a keygen derived from "this" release.
    return true;
}

std::string Delser::format_result(std::vector<std::string>key) {
    std::string seed = utils::rjust(key[0], '0', 4);
    std::string key_bytes;

    for (int x=1; x<key.size(); x++) {
        key_bytes += '-' + utils::rjust(key[x], '0', 4);
    }
    
    return seed + key_bytes;
}

