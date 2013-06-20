#include <iostream>
#include <fstream>
#include "Delser.hpp"

int main(int argc, char *argv[]) { 
    Delser *cur_delser = new Delser(0); 

    if (true) { 
        long int space = 900000;
     
        std::vector<std::string> to_test; 
        for (int i=0; i<space; i++) { 
            to_test.push_back(cur_delser->make_key(i)); 
        } 
     
        std::ofstream file_obj;
        file_obj.open("out.txt", std::ios::out);
        int good = 0;
        for (int test=0; test<to_test.size(); test++) { 
            if (cur_delser->check_key(to_test[test])) { 
                good += 1;
                file_obj.write(std::string(to_test[test]).c_str(), std::string(to_test[test]).length());
                file_obj.write(std::string("\n").c_str(), 2);
                file_obj.flush();
            } else {
                std::cout << "For the key " << to_test[test] << " with length " << to_test[test].length() << std::endl; 
            } 
        }
        file_obj.close();
        std::cout << good << " good keys" << std::endl;

    } else {
        std::string input;
        while (true) {
            std::cout << "> ";
            std::cin >> input;
            bool error = false;
            try {
                cur_delser->check_key(input);
            } catch(std::runtime_error e) {
                std::cout << "Error; " << e.what() << std::endl;
                error = true;
            }
            if (!error) std::cout << "Good" << std::endl;
        }
            
    }
} 

