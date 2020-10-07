
#include<iostream>
#include<cstdio>
using namespace std;
int main() {
    int a = 1, b = 2;
    int c = a + ++ ++b;
    std::cout << a << " " << b << " " << c << std::endl;
}
