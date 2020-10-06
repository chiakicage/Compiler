#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <set>

enum token_type {
    FUNCTION,
    VARIABLE
};
enum char_type { 
    EMPTY,
    ALPHA,
    DIGIT,
    OTHER
};
enum stmt_type {
    PROGRAM,
    VARDEF,
    FUNCDEF,
    BLOCK,
    EXPR,
    IF,
    IF_ELSE,
    FOR,
    WHILE,
    RETURN
};

namespace Stream {
    char buffer;

    inline char nxtChar() {
        if (!buffer) {
            buffer = getchar();
        }
        return buffer;
    }

    inline char getChar() {
        char ret = nxtChar();
        buffer = 0;
        return ret;
    }

}

namespace Lexer {
    std::string buffer;

    inline int charType(char ch) {
        if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') return EMPTY;
        else if ('0' <= ch && ch <= '9') return DIGIT;
        else if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_') return ALPHA;
        else return OTHER;
    }

    inline std::string nxtLexeme() {
        if (buffer.empty()) {
            while (charType(Stream::nxtChar()) == EMPTY) Stream::getChar();
            if (charType(Stream::nxtChar()) == DIGIT) {
                while (charType(Stream::nxtChar()) == DIGIT) 
                    buffer += Stream::getChar();
            } else if (charType(Stream::nxtChar()) == ALPHA) {
                while (charType(Stream::nxtChar()) == DIGIT || charType(Stream::nxtChar()) == ALPHA)
                    buffer += Stream::getChar();
            } else {
                switch (Stream::nxtChar()) {
                    case '+': case '-': case '*': case '/': case '%':
                    case '^': case ',': case ';': case '(': case ')':
                    case '{': case '}': case '[': case ']': case '#':
                        buffer += Stream::getChar();
                        break;
                    case '<':
                        buffer += Stream::getChar();
                        if (Stream::nxtChar() == '=')
                            buffer += Stream::getChar();
                        break;
                    case '>':
                        buffer += Stream::getChar();
                        if (Stream::nxtChar() == '=')
                            buffer += Stream::getChar();
                        break;
                    case '!':
                        buffer += Stream::getChar();
                        if (Stream::nxtChar() == '=')
                            buffer += Stream::getChar();
                        break;
                    case '=':
                        buffer += Stream::getChar();
                        if (Stream::nxtChar() == '=')
                            buffer += Stream::getChar();
                        break;
                    case '&':
                        buffer += Stream::getChar();
                        if (Stream::nxtChar() == '&')
                            buffer += Stream::getChar();
                        break;
                    case '|':
                        buffer += Stream::getChar();
                        if (Stream::nxtChar() == '|')
                            buffer += Stream::getChar();
                        break;
                }
            }
        }
        return buffer;
    }

    inline std::string getLexeme() {
        std::string ret = nxtLexeme();
        buffer.clear();
        return ret;
    }
}

struct Token {
    token_type type;
    std::string name;
    int value;
    Token(token_type type = VARIABLE, const std::string& name = "", int v = 0):
        type(type), name(name), value(v) {}
};

struct Tree {
    stmt_type type;
    std::vector<Tree*> children;
    Tree(stmt_type type):type(type) {
        children.clear();
    }
};

Tree* Main;
Tree* Root;
std::map<std::string, Tree*> func_table;
std::map<std::string, int> var_table;
std::map<std::string, std::vector<int> > array_table;


namespace Parser {

    inline void match(const std::string& str) {
        if (Lexer::getLexeme() != str)
            throw "Match Failed!";
    }
    Tree* program() {
        Tree* ret = new Tree(PROGRAM);
        match("#"), match("include"), match("<"), match("iostream"), match(">");
        match("#"), match("include"), match("<"), match("cstdio"), match(">");
        match("using"), match("namespace"), match("std"), match(";");
        return ret;
    }
}

namespace Runner {

}

int main() {
#ifdef ARK
    freopen("test.in", "r", stdin);
#endif
    Root = Parser::program();
}
