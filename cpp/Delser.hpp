/*
http://www.brandonstaggs.com/2007/07/26/implementing-a-partial-serial-number-verification-system-in-delphi/
*/

#ifndef DELSER_HPP
#define DELSER_HPP

#include <iostream>
#include <sstream>
#include <iomanip>

#include <memory>
#include <algorithm>

#include <vector>
#include <array>
#include <tuple>


class Delser {
public:
    bool check_key(std::string key);

    std::string make_key(int seed);

    /// fairly self explanatory
    class Exception: public std::exception
    {
    public:
        /** Constructor (C strings).
         *  @param message C-style string error message.
         *                 The string contents are copied upon construction.
         *                 Hence, responsibility for deleting the \c char* lies
         *                 with the caller. 
         */
        explicit Exception(const char* message):
          msg_(message)
          {
          }

        /** Constructor (C++ STL strings).
         *  @param message The error message.
         */
        explicit Exception(const std::string& message):
          msg_(message)
          {}

        /** Destructor.
         * Virtual to allow for subclassing.
         */
        virtual ~Exception() throw (){}

        /** Returns a pointer to the (constant) error description.
         *  @return A pointer to a \c const \c char*. The underlying memory
         *          is in posession of the \c Exception object. Callers \a must
         *          not attempt to free the memory.
         */
        virtual const char* what() const throw (){
           return msg_.c_str();
        }

    protected:
        /** Error message.
         */
        std::string msg_;
    };

    class key_gen_exception : public Exception {};

    // TODO: fix this shit
    #define key_invalid std::runtime_error
    //class key_invalid : public key_gen_exception {};

    #define key_blacklisted std::runtime_error
    //class key_blacklisted : public key_gen_exception {};

    #define key_phony std::runtime_error
    //class key_phony : public key_gen_exception {};

    #define key_bad_checksum std::runtime_error
    //class key_bad_checksum : public key_gen_exception {};

    Delser(int byte_to_check) : byte_to_check(byte_to_check) {
        // ensure a valid sequence has been selected to check against
        if (!(0 <= byte_to_check && byte_to_check <= sequences.size())) {
            throw Exception("Bad byte_to_check");
        }
    }

private:

    int byte_to_check;

    typedef std::tuple<int,int,int> sequence;
    std::vector<sequence> sequences = {
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
            throw Delser::Exception(str);
        }
    }

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
