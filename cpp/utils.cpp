#include "Delser.hpp"

std::string utils::vector_to_string(std::vector<std::string> v, char sep) { 
    std::stringstream ss; 
    for(size_t i=0; i < v.size(); ++i) { 
        if(i!=0) { ss << sep; } 
        ss << v[i]; 
    } 
    return ss.str(); 
} 
 
std::string utils::upper(std::string str) { 
    std::transform(str.begin(), str.end(), str.begin(), ::toupper); 
    return str; 
} 
 
std::vector<std::string> &utils::split(const std::string &s, char delim, std::vector<std::string> &elems) { 
    std::stringstream ss(s); 
    std::string item; 
    while (std::getline(ss, item, delim)) { 
        elems.push_back(item); 
    } 
    return elems; 
} 
 
std::vector<std::string> utils::split(const std::string &s, char delim) { 
    std::vector<std::string> elems; 
    split(s, delim, elems); 
    return elems; 
}

std::string utils::rjust(std::string str, char with, int size) { 
    std::stringstream stream; 
     
    if (str.length() >= size) { 
        return str; 
    } 
    stream << std::setfill(with) << std::setw(size) << str; 
    return stream.str(); 
}
