#include <iostream>
#include "Delser.hpp"

int main(int argc, char *argv[]) { 
    Delser *cur_delser = new Delser(); 
 
    int space = 5000; 
 
    std::vector<std::string> to_test; 
    for (int i=0; i<space; i++) { 
        to_test.push_back(cur_delser->make_key(i)); 
    } 
 
    for (int test=0; test<to_test.size(); test++) { 
        if (!cur_delser->check_key(to_test[test])) { 
            std::cout << "For the key " << to_test[test] << " with length " << to_test[test].length() << std::endl; 
        } 
    } 
} 

