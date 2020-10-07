#include <cstdio>
#include <string>
#include <iostream>

enum test {testt, test1};

test a;

int main() {
    std::string s;
    switch (s) {
        case "123": std::cout << "1" << std::endl; break;
        case "213": std::cout << "2" << std::endl; break;
    }
    a = test1;
    std::cout << a;   
}
