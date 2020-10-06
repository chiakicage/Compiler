#include <cstdio>
#include <string>
#include <iostream>

enum test {testt, test1};

test a;

int main() {
    a = test1;
    std::cout << a;   
}
