/*
http://www.brandonstaggs.com/2007/07/26/implementing-a-partial-serial-number-verification-system-in-delphi/
*/

#ifndef DELSER_HPP
#define DELSER_HPP

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

#include <memory>
#include <algorithm>

#include <vector>
#include <array>
#include <tuple>

#include "exceptions.cpp"

class Delser {
public:
    typedef std::tuple<int,int,int> sequence;
    
    bool check_key(std::string key);
    std::string make_key(int seed);

    // two constructors, to make it so that the supplying the sequences is optional
    Delser(int byte_to_check, std::vector<sequence> supplied_sequences) : byte_to_check(byte_to_check) {
        if (supplied_sequences.empty()) {
            sequences = default_sequences;
        } else {
            sequences = supplied_sequences;
        }

        validate_byte_to_check(byte_to_check);
    }

    Delser(int byte_to_check) : byte_to_check(byte_to_check) {
        sequences = default_sequences;

        validate_byte_to_check(byte_to_check);
    }

private:
    void validate_byte_to_check(int byte_to_check) {
        // ensure a valid sequence has been selected to check against
        if (!(0 <= byte_to_check && byte_to_check <= sequences.size())) {
            throw exceptions::bad_byte_to_check_error(std::to_string(byte_to_check));
        }
    }

    int byte_to_check;

    std::vector<sequence> sequences;
    std::vector<sequence> default_sequences = {
        std::make_tuple(24, 3, 200),
        std::make_tuple(10, 0, 56),
        std::make_tuple(15, 2, 91),
        std::make_tuple(25, 3, 200),
        std::make_tuple(25, 3, 56)
    };


    std::vector<std::string> blacklist;

    std::string get_checksum(std::string string);
    bool check_key_checksum(std::string skey);
    int _get_key_byte(int seed, int a, int b, int c);
    std::string format_result(std::vector<std::string>key);

};

namespace utils {
    template<typename T>
    void assert(bool condition, T str) {
        if (!condition) {
            throw std::runtime_error(str);
        }
    }

    #define SSTR( x ) dynamic_cast< std::ostringstream & >( ( std::ostringstream() << x ) ).str()
    #define CSTR( x ) SSTR( x ).c_str()

    template<typename T>
    std::string int_to_hex(T i) {
        std::stringstream stream;
        stream << std::hex << i;
        return stream.str();
    }
    template<typename T>
    int hex_to_int(T str) {
        int x;
        std::stringstream stream;
        stream << std::hex << str;
        stream >> x;
        return static_cast<int>(x);
    }
    std::string vector_to_string(std::vector<std::string> v, char sep);
    std::string upper(std::string);
    std::string rjust(std::string str, char with, int size);
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems); 
    std::vector<std::string> split(const std::string &s, char delim);
};

#endif
