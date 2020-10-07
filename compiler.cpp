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

enum char_type { 
    EMPTY,
    ALPHA,
    DIGIT,
    OTHER
};
enum stmt_type {
    PROGRAM,
    FUNCDEF,
    VARDEF,
    STATEMENT,
    EXPR,
    IF,
    IF_ELSE,
    FOR,
    WHILE,
    RETURN
};
enum obj_type {
    VARIABLE,
    ARRAY,
    CIN,
    COUT,
    ENDL,
    VALUE
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
            if (Stream::nxtChar() == EOF) return buffer;
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
                        if (Stream::nxtChar() == '=' || Stream::nxtChar() == '<')
                            buffer += Stream::getChar();
                        break;
                    case '>':
                        buffer += Stream::getChar();
                        if (Stream::nxtChar() == '=' || Stream::nxtChar() == '>')
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

struct Object {
    obj_type type;
    std::string name;
    std::vector<int> dims;
    int value;
    Object(obj_type type):type(type) {}
    Object(obj_type type, int val):type(type), value(val) {}
    Object(obj_type type, const std::string& name):type(type), name(name) {}
};

struct Tree {
    stmt_type type;
    std::vector<Tree*> children;
    std::vector<Object> vars;
    std::vector<std::string> ops;
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

    Tree* Program();
    Tree* Funcdef(const std::string&);
    Tree* Vardef(const std::string&);
    Tree* Statement();
    Tree* Statement_in_for();
    Tree* Expression();
    Tree* Return();
    Tree* If();
    Tree* While();
    Tree* For();
    Tree* Exec(const std::string&);
    Tree* Unit0();
    Tree* Unit1();
    Tree* Unit2();
    Tree* Unit3();
    Tree* Unit4();
    Tree* Unit5();
    Tree* Unit6();
    Tree* Unit7();
    Tree* Unit8();
    Tree* Unit9();

    inline void match(const std::string& str) {
        if (Lexer::getLexeme() != str)
            throw "Match Failed!";
    }

    Tree* Program() {
        Tree* ret = new Tree(PROGRAM);
        match("#"), match("include"), match("<"), match("iostream"), match(">");
        match("#"), match("include"), match("<"), match("cstdio"), match(">");
        match("using"), match("namespace"), match("std"), match(";");
        while (!Lexer::nxtLexeme().empty()) {
            match("int");
            std::string name = Lexer::getLexeme();
            if (Lexer::nxtLexeme() == "(") {
                ret->children.push_back(Funcdef(name));
            } else {
                ret->children.push_back(Vardef(name));
            }
        }
        return ret;
    }

    Tree* Vardef(const std::string& name) {
        Tree* ret = new Tree(VARDEF);
        ret->vars.emplace_back(VARIABLE, name);
        while (Lexer::nxtLexeme() != ";") {
            if (Lexer::nxtLexeme() == "[") {
                ret->vars.back().type = ARRAY;
                while (Lexer::nxtLexeme() == "[") {
                    match("[");
                    ret->vars.back().dims.push_back(std::stoi(Lexer::getLexeme()));
                    match("]");
                }
            }
            else {
                match(",");
                ret->vars.emplace_back(VALUE, Lexer::getLexeme());
            }
        }
        match(";");
        return ret;
    }

    Tree* Funcdef(const std::string& name) {
        Tree* ret = new Tree(FUNCDEF);
        func_table[name] = ret;
        match("(");
        if (Lexer::nxtLexeme() != ")") {
            match("int");
            ret->vars.emplace_back(VARIABLE, Lexer::getLexeme());
            while (Lexer::nxtLexeme() != ")") {
                match(","), match("int");
                ret->vars.emplace_back(VARIABLE, Lexer::getLexeme());
            }
        }
        match(")");
        match("{");
        ret->children.push_back(Statement());
        match("}");
        return ret;
    }

    Tree* Statement() {
        Tree* ret = new Tree(STATEMENT);
        std::string s = Lexer::nxtLexeme();
        if (s == "int") {
            match("int");
            ret->children.push_back(Vardef(Lexer::getLexeme()));
        } else if (s == "if") {
            match("if");
            ret->children.push_back(If());
        } else if (s == "for") {
            match("for");
            ret->children.push_back(For());
        } else if (s == "while") {
            match("while");
            ret->children.push_back(While());
        } else if (s == "return") {
            match("return");
            ret->children.push_back(Return());
        } else if (s == "{") {
            match("{");
            ret->children.push_back(Statement());
            match("}");
        } else {
            ret->children.push_back(Expression());
        }
        return ret;
    
    }

    Tree* If() {

    }

    Tree* For() {

    }

    Tree* While() {

    }

    Tree* Return() {

    }

    Tree* Exec(const std::string&) {

    }

    Tree* Expression() {
        Tree* ret = new Tree(EXPR);
        ret->children.push_back(Unit9());
        while (Lexer::nxtLexeme() == "<<" || Lexer::nxtLexeme() == ">>") {
            ret->ops.push_back(Lexer::getLexeme());
            ret->children.push_back(Unit9());
        }
        return ret;
    }

    Tree* Unit0() {
        Tree* ret = new Tree(EXPR);
        std::string s = Lexer::getLexeme();
        if (s == "cin") {
            ret->vars.emplace_back(CIN);
        } else if (s == "cout") {
            ret->vars.emplace_back(COUT);
        } else if (s == "endl") {
            ret->vars.emplace_back(ENDL);
        } else if (s == "(") { // vars -> empty()
            match("(");
            ret->children.push_back(Expression());
            match(")");
        } else if (isdigit(s[0])) { // vars[0].type = VALUE
            ret->vars.emplace_back(VALUE, std::stoi(s));
        } else {
            if (Lexer::nxtLexeme() == "(") {
                ret->children.push_back(Exec(s)); // !children.empty() && vars
                match("(");
                if (Lexer::nxtLexeme() != ")") {
                    match("int");
                    ret->vars.emplace_back(VARIABLE, Lexer::getLexeme());
                    while (Lexer::nxtLexeme() != ")") {
                        match(","), match("int");
                        ret->vars.emplace_back(VARIABLE, Lexer::getLexeme());
                    }
                }
                match(")");
            } else {
                ret->vars.emplace_back(VARIABLE, s);
            }
        }
        return ret;
    }
}

namespace Runner {

}

int main() {
#ifdef ARK
    freopen("test.in", "r", stdin);
#endif
    // Root = Parser::program();
    // for (int i = 1; i <= 20; ++i)
        // std::cout << Lexer::getLexeme().empty() << std::endl;
}
