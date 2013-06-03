/*
http://www.brandonstaggs.com/2007/07/26/implementing-a-partial-serial-number-verification-system-in-delphi/
*/
#include <iostream>
#include <sstream>
#include <iomanip>

#include <memory>
#include <algorithm>
#include <assert.h>

#include <vector>
#include <array>
#include <tuple>


class Delser {
public:
    bool check_key(std::string key);

    std::string make_key(int seed);

    /// fairly self explanatory
    class key_gen_exception : public std::exception {
    public:
        explicit key_gen_exception(const char* message) : msg_(message) {}
        explicit key_gen_exception(const std::string& message) : msg_(message) {}
        virtual ~key_gen_exception() throw(){}
        virtual const char* what() const throw(){
            return msg_.c_str();
        }
    protected:
        std::string msg_;
    };

    class key_invalid : public key_gen_exception {};

    class key_blacklisted : public key_gen_exception {};

    class key_phony : public key_gen_exception {};

    class key_bad_checksum : public key_gen_exception {};

    Delser() : byte_to_check(byte_to_check), sequences(sequences), blacklist(blacklist) {
        if (sequences.empty()) {
            #define PBT(x,y,z) sequences.push_back(std::make_tuple(x,y,z))
            PBT(24, 3, 200);
            PBT(10, 0, 56);
            PBT(15, 2, 91);
            PBT(25, 3, 200);
            PBT(25, 3, 56);
        }

        // ensure a valid sequence has been selected to check against
        assert (0 <= byte_to_check && byte_to_check <= sequences.size());

    }

private:

    int byte_to_check;

    typedef std::tuple<int,int,int> sequence;
    std::vector<sequence> sequences;

    std::vector<std::string> blacklist;

    std::string get_checksum(std::string string);
    bool check_key_checksum(std::string skey);
    int _get_key_byte(int seed, int a, int b, int c);
    std::string format_result(std::vector<std::string>key);

    std::string string_toupper(std::string);
    std::string pad_with(std::string str, char with, int size);
    std::string vector_to_string(std::vector<std::string> v, char sep);
    template<typename T>
    std::string int_to_hex(T i, bool prepend, int pad) {
        std::stringstream stream;
        if (prepend) stream << "0x";
        stream << std::setfill('0') << std::setw(sizeof(T) * 2)
               << std::hex << i;
        return stream.str();
    }
    template<typename T>
    int hex_str_to_int(T str) {
        int x;
        std::stringstream stream;
        stream << std::hex << str;
        stream >> x;
        return static_cast<int>(x);
    }
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems); 
    std::vector<std::string> split(const std::string &s, char delim);
};
