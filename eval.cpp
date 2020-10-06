/*
    <exp> => <term> { <addop> <term> }
    <addop> => + | -
    <term> => <factor> { <mulop> <factor> }
    <mulop> => * | / | %
    <factor> => ( <exp> ) | Number
*/

#include <cstdio>
#include <cstdlib>
#include <cctype>

const int MAXN = 100010;

char token;
char text[MAXN];

int exp();
int term();
int factor();

int error() {
    fprintf(stderr, "ERROR\n");
    exit(1);
}
inline char getChar() {
    char ret = getchar();
    while (isblank(ret)) ret = getchar();
    return ret;
}
void match(char expectedToken) {
    if (token == expectedToken) token = getChar();
    else error();
}

int main() {    
#ifdef ARK
    freopen("test.in", "r", stdin);
#endif
    // for (int i = 1; i <= 6; ++i) fgets(text, MAXN, stdin);
    // for (int i = 1; i <= 12; ++i) getchar();
    token = getChar();
    int result = exp();
    // if (token == '\n') {
        printf("%d\n", result);
    // } else {
        // error();
    // }
}

int exp() {
    int ret = term();
    while (token == '+' || token == '-') {
        switch (token) {
            case '+': 
                match('+');
                ret += term();
                break;
            case '-':
                match('-');
                ret -= term();
                break;
        }
    }
    return ret;
}

int term() {
    int ret = factor();
    while (token == '*') {
        match('*');
        ret *= factor();
    }
    return ret;
}

int factor() {
    int ret;
    if (token == '(') {
        match('(');
        ret = exp();
        match(')');
    } else if (isdigit(token)) {
        ungetc(token, stdin);
        scanf("%d", &ret);
        token = getChar();
    } else error();
    return ret;
}





