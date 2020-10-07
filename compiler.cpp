#define NDEBUG

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>

namespace Reader {
    std::vector<int> numbers;
    int pos;
    inline int read() {
        return numbers[pos++];
    }
}
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
    STATEMENTS,
    EXPR,
    IF,
    IF_ELSE,
    FOR,
    WHILE,
    RETURN,
    UNIT0,
    UNIT1,
    UNIT2,
    UNIT3,
    UNIT4,
    UNIT5,
    UNIT6,
    UNIT7,
    UNIT8,
    UNIT9,
    NOSTMT
};
enum obj_type {
    VARIABLE,
    ARRAY,
    CIN,
    COUT,
    ENDL,
    VALUE,
    FUNCTION
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
    std::vector<int> address;
    Object(obj_type type):type(type), value(0) {
        // std::cerr << "done1\n";
        address.clear();
        name.clear();
        dims.clear();
    }
    Object(obj_type type, int val):type(type), value(val) {
        // std::cerr << "done2\n";
        address.clear();
        name.clear();
        dims.clear();
    }
    Object(obj_type type, const std::string& name):type(type), name(name), value(0) {
        // std::cerr << "done3\n";
        address.clear();
        dims.clear();
    }


    void setArray(const std::vector<int>& def) {
        assert(type == ARRAY);
        int size = 1;
        for (auto it = def.rbegin(); it != def.rend(); ++it) {
            dims.push_back(size);
            size *= *it;
        }
        std::reverse(dims.begin(), dims.end());
        address.resize(size);
    }

    int& getArray(const std::vector<int> ind) {
        assert(type == ARRAY);
        int ret = 0;
        for (size_t i = 0; i < ind.size(); i++) {
            ret = ret + dims[i] * ind[i];
        }
        return address[ret];
    }
};

struct Tree {
    stmt_type type;
    std::string name;
    std::vector<Tree*> children;
    std::vector<Object> vars;
    std::vector<std::string> ops;
    Tree(stmt_type type):type(type) {
        children.clear();
    }
};

std::unordered_map<std::string, Tree*> func_table;
Tree* Root;


namespace Parser {

    Tree* Program();
    Tree* Funcdef(const std::string&);
    Tree* Vardef(const std::string&);
    Tree* Statement();
    Tree* Statements();
    Tree* Expression();
    Tree* Return();
    Tree* If();
    Tree* While();
    Tree* For();
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
        // std::cerr << "Match " + str << std::endl;
        std::string tmp;
        if ((tmp = Lexer::getLexeme()) != str)
            throw "Match " + str + " Failed! " + tmp;
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
                match(";");
            }
        }
        func_table["putchar"] = nullptr;
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
            } else {
                match(",");
                ret->vars.emplace_back(VARIABLE, Lexer::getLexeme());
            }
        }
        return ret;
    }

    Tree* Funcdef(const std::string& name) {
        Tree* ret = new Tree(FUNCDEF);
        ret->name = name;
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
        ret->children.push_back(Statements());
        return ret;
    }

    Tree* Statements() {
        Tree* ret = new Tree(STATEMENTS);
        match("{");
        while (Lexer::nxtLexeme() != "}") {
            std::string s = Lexer::nxtLexeme();
            if (s == "int") {
                match("int");
                ret->children.push_back(Vardef(Lexer::getLexeme()));
                match(";");
            }
            else if (s == "if") {
                match("if");
                ret->children.push_back(If());
            }
            else if (s == "for") {
                match("for");
                ret->children.push_back(For());
            }
            else if (s == "while") {
                match("while");
                ret->children.push_back(While());
            }
            else if (s == "return") {
                match("return");
                ret->children.push_back(Return());
                match(";");
            }
            else if (s == "{") {
                ret->children.push_back(Statements());
            }
            else {
                ret->children.push_back(Expression());
                match(";");
            }
        }
        match("}");
        return ret;
    }

    Tree* Statement() {
        Tree* ret = new Tree(STATEMENT);
        std::string s = Lexer::nxtLexeme();
        if (s == "int") {
            match("int");
            ret->children.push_back(Vardef(Lexer::getLexeme()));
            match(";");
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
            match(";");
        } else if (s == "{") {
            ret->children.push_back(Statements());
        } else {
            ret->children.push_back(Expression());
            match(";");
        }
        return ret;
    }

    Tree* If() {
        Tree* ret = new Tree(IF);
        match("(");
        ret->children.push_back(Expression());
        match(")");
        ret->children.push_back(Statement());
        if (Lexer::nxtLexeme() == "else") {
            match("else");
            ret->type = IF_ELSE;
            ret->children.push_back(Statement());
        }
        return ret;
    }

    Tree* For() {
        Tree* ret = new Tree(FOR);
        match("(");
        if (Lexer::nxtLexeme() != ";") {
            if (Lexer::nxtLexeme() == "int") {
                match("int");
                ret->children.push_back(Vardef(Lexer::getLexeme()));
            } else {
                ret->children.push_back(Expression());
            }
        } else ret->children.push_back(nullptr);
        match(";");
        if (Lexer::nxtLexeme() != ";") {
            ret->children.push_back(Expression());
        } else ret->children.push_back(nullptr);
        match(";");
        if (Lexer::nxtLexeme() != ")") {
            ret->children.push_back(Expression());
        } else ret->children.push_back(nullptr);
        match(")");
        ret->children.push_back(Statement());
        return ret;
    }

    Tree* While() {
        Tree* ret = new Tree(WHILE);
        match("(");
        ret->children.push_back(Expression());
        match(")");
        ret->children.push_back(Statement());
        return ret;
    }

    Tree* Return() {
        Tree* ret = new Tree(RETURN);
        ret->children.push_back(Expression());
        return ret;
    }

    Tree* Expression() {
        Tree* ret = new Tree(EXPR);
        ret->children.push_back(Unit9());
        std::string s;
        while ((s = Lexer::nxtLexeme()) == "<<" || s == ">>") {
            if (s == "<<") match("<<");
            else match(">>");
            ret->ops.push_back(s);
            ret->children.push_back(Unit9());
        }
        return ret;
    }

    Tree* Unit0() {
        Tree* ret = new Tree(UNIT0);
        std::string s = Lexer::nxtLexeme();
        if (s == "cin") {
            match("cin");
            ret->vars.emplace_back(CIN);
        } else if (s == "cout") {
            match("cout");
            ret->vars.emplace_back(COUT);
        } else if (s == "endl") {
            match("endl");
            ret->vars.emplace_back(ENDL);
        } else if (s == "(") {
            ret->vars.emplace_back(VALUE);
            match("(");
            ret->children.push_back(Expression());
            match(")");
        } else if (isdigit(s[0])) { // vars[0].type = VALUE
            ret->vars.emplace_back(VALUE, std::stoi(Lexer::getLexeme()));
        } else {
            s = Lexer::getLexeme();
            if (Lexer::nxtLexeme() == "(") {
                ret->vars.emplace_back(FUNCTION, s); // !children.empty() && vars
                match("(");
                if (Lexer::nxtLexeme() != ")") {
                    ret->children.push_back(Expression());

                    // std::cerr << Lexer::nxtLexeme() << "\n";
                    while (Lexer::nxtLexeme() != ")") {
                        match(",");
                        ret->children.push_back(Expression());
                    }
                }
                match(")");
            } else {
                ret->vars.emplace_back(VARIABLE, s);
            }
        }
        if (Lexer::nxtLexeme() == "[") {
            ret->vars.back().type = ARRAY;
            while (Lexer::nxtLexeme() == "[") {
                match("[");
                ret->children.push_back(Expression());
                match("]");
            } 
        }
        return ret;
    }

    Tree* Unit1() {
        Tree* ret = new Tree(UNIT1);
        std::string s;
        while ((s = Lexer::nxtLexeme()) == "+" || s == "-" || s == "!") {
            if (s == "+") match("+");
            else if (s == "-") match("-");
            else match("!");
            ret->ops.push_back(s);
        }
        ret->children.push_back(Unit0());
        return ret;
    }

    Tree* Unit2() {
        Tree* ret = new Tree(UNIT2);
        ret->children.push_back(Unit1());
        std::string s;
        while ((s = Lexer::nxtLexeme()) == "*" || s == "/" || s == "%") {
            if (s == "*") match("*");
            else if (s == "/") match("/");
            else match("%");
            ret->ops.push_back(s);
            ret->children.push_back(Unit1());
        }
        return ret;
    }

    Tree* Unit3() {
        Tree* ret = new Tree(UNIT3);
        ret->children.push_back(Unit2());
        std::string s;
        while ((s = Lexer::nxtLexeme()) == "+" || s == "-") {
            if (s == "+") match("+");
            else match("-");
            ret->ops.push_back(s);
            ret->children.push_back(Unit2());
        }
        return ret;
    }

    Tree* Unit4() {
        Tree* ret = new Tree(UNIT4);
        ret->children.push_back(Unit3());
        std::string s;
        while ((s = Lexer::nxtLexeme()) == "<" || s == "<=" || s == ">" || s == ">=") {
            if (s == "<") match("<");
            else if (s == ">") match(">");
            else if (s == "<=") match("<=");
            else match(">=");
            ret->ops.push_back(s);
            ret->children.push_back(Unit3());
        }
        return ret;
    }

    Tree* Unit5() {
        Tree* ret = new Tree(UNIT5);
        ret->children.push_back(Unit4());
        std::string s;
        while ((s = Lexer::nxtLexeme()) == "==" || s == "!=") {
            if (s == "==") match("==");
            else match("!=");
            ret->ops.push_back(s);
            ret->children.push_back(Unit4());
        }
        return ret;
    }

    Tree* Unit6() {
        Tree* ret = new Tree(UNIT6);
        ret->children.push_back(Unit5());
        while (Lexer::nxtLexeme() == "^") {
            match("^");
            ret->ops.push_back("^");
            ret->children.push_back(Unit5());
        }
        return ret;
    }

    Tree* Unit7() {
        Tree* ret = new Tree(UNIT7);
        ret->children.push_back(Unit6());
        while (Lexer::nxtLexeme() == "&&") {
            match("&&");
            ret->ops.push_back("&&");
            ret->children.push_back(Unit6());
        }
        return ret;
    }

    Tree* Unit8() {
        Tree* ret = new Tree(UNIT8);
        ret->children.push_back(Unit7());
        while (Lexer::nxtLexeme() == "||") {
            match("||");
            ret->ops.push_back("||");
            ret->children.push_back(Unit7());
        }
        return ret;
    }
    Tree* Unit9() {
        Tree* ret = new Tree(UNIT9);
        ret->children.push_back(Unit8());
        while (Lexer::nxtLexeme() == "=") {
            match("=");
            ret->ops.push_back("=");
            ret->children.push_back(Unit8());
        }
        return ret;
    }
}

namespace Runner {
    int Program(Tree*);
    int Statement(Tree*);
    int Statements(Tree*);
    int Return(Tree*);
    int If(Tree*);
    int While(Tree*);
    int For(Tree*);
    int Function(Tree*, const std::vector<int>&);
    int Expression(Tree*);
    Object Unit0(Tree*);
    Object Unit1(Tree*);
    Object Unit2(Tree*);
    Object Unit3(Tree*);
    Object Unit4(Tree*);
    Object Unit5(Tree*);
    Object Unit6(Tree*);
    Object Unit7(Tree*);
    Object Unit8(Tree*);
    Object Unit9(Tree*);

    std::unordered_map<std::string, std::vector<Object> > var_table;

    bool return_tag;
    std::string func_tag;

    int Function(Tree* cur, const std::vector<int>& params) {
        // std::cerr << "in func " + cur->name << "\n";
        // for (auto x : params) 
            // std::cerr << x << " ";
        // std::cerr << "\n";
        if (cur == nullptr) {
            putchar(char(params.front()));
            return 0;
        }
        func_tag = cur->name;
        assert(params.size() == cur->vars.size());
        for (size_t i = 0; i < params.size(); i++) {
            var_table[cur->vars[i].name].emplace_back(VARIABLE, params[i]);
        }
        int ret = 0;
        if (!cur->children.empty()) {
            ret = Statements(cur->children.front());
        }
        for (size_t i = 0; i < params.size(); i++) {
            var_table[cur->vars[i].name].pop_back();
        }
        return_tag = false;
        return ret;
    }

    

    int Statements(Tree* cur) {
        // std::cerr << "in stmts\n";
        std::vector<std::string> new_vars;
        int ret = 0;
        for (auto chd : cur->children) {
            int tmp = 0;
            switch (chd->type) {
                case VARDEF:
                    for (auto obj : chd->vars) {
                        new_vars.push_back(obj.name);
                        // auto &v = var_table[obj.name];
                        // std::cerr << v.size() << " 234\n";
                        // v.push_back(Object(VALUE, 1));
                        // std::cerr << v.size() << " 345\n";
                        // v.pop_back();
                        var_table[obj.name].emplace_back(obj.type);
                        // std::cerr << var_table[obj.name].size() << "\n";
                        if (obj.type == ARRAY) {
                            auto &u = var_table[obj.name].back();
                            u.setArray(obj.dims);
                        }
                    }
                    break;
                case IF:
                case IF_ELSE:
                    tmp = If(chd);
                    break;
                case FOR:
                    tmp = For(chd);
                    break;
                case WHILE:
                    tmp = While(chd);
                    break;
                case RETURN:
                    tmp = Return(chd);
                    return_tag = true;
                    break;
                case STATEMENTS:
                    tmp = Statements(chd);
                    break;
                case EXPR:
                    Expression(chd);
                    break;
                default:
                    // std::cerr << chd->type << "\n";
                    assert(0);
            }
            if (return_tag) {
                ret = tmp;
                break;
            }
        }
        for (auto name : new_vars) {
            var_table[name].pop_back();
        }
        return ret;
    }

    int Statement(Tree* cur) {
        // std::cerr << "in stmt\n";
        std::vector<std::string> new_vars;
        int ret = 0;
        for (auto chd : cur->children) {
            int tmp = 0;
            switch (chd->type) {
                case VARDEF:
                    for (auto obj : chd->vars) {
                        new_vars.push_back(obj.name);
                        var_table[obj.name].emplace_back(obj.type);
                        if (obj.type == ARRAY) {
                            auto& u = var_table[obj.name].back();
                            u.setArray(obj.dims);
                        }
                    }
                    break;
                case IF:
                    tmp = If(chd);
                    break;
                case FOR:
                    tmp = For(chd);
                    break;
                case WHILE:
                    tmp = While(chd);
                    break;
                case RETURN:
                    tmp = Return(chd);
                    return_tag = true;
                    break;
                case STATEMENTS:
                    tmp = Statements(chd);
                    break;
                case EXPR:
                    Expression(chd);
                    break;
                default:
                    assert(0);
            }
            if (return_tag) {
                ret = tmp;
                break;
            }
        }
        for (auto name : new_vars) {
            var_table[name].pop_back();
        }
        return ret;
    }

    int If(Tree* cur) {
        // std::cerr << "in if\n";
        int ret = 0;
        if (Expression(cur->children[0])) {
            ret = Statement(cur->children[1]);
        } else {
            if (cur->type == IF_ELSE)
                ret = Statement(cur->children[2]);
        }
        return ret;
    }

    int For(Tree* cur) {
        // std::cerr << "in for\n";
        int ret = 0;
        std::vector<std::string> new_vars;
        if (cur->children[0] != nullptr) {
            if (cur->children[0]->type == VARDEF) {
                for (auto obj : cur->children[0]->vars) {
                    new_vars.push_back(obj.name);
                    var_table[obj.name].emplace_back(obj.type);
                    if (obj.type == ARRAY) {
                        auto& u = var_table[obj.name].back();
                        u.setArray(obj.dims);
                    }
                }
            } else {
                Expression(cur->children[0]);
            }
        }
        while (cur->children[1] == nullptr || Expression(cur->children[1])) {
            int tmp = Statement(cur->children[3]);
            if (return_tag) {
                ret = tmp;
                break;
            }
            if (cur->children[2] != nullptr) {
                Expression(cur->children[2]);
            }
        }
        for (auto name : new_vars) {
            var_table[name].pop_back();
        }
        return ret;
    }

    int While(Tree* cur) {
        // std::cerr << "in while\n";
        int ret = 0;
        while (Expression(cur->children[0])) {
            int tmp = Statement(cur->children[1]);
            if (return_tag) {
                ret = tmp;
                break;
            }
        }
        return ret;
    }

    int Return(Tree* cur) {
        // std::cerr << "in return\n";
        int ret = Expression(cur->children[0]);
        assert(return_tag == false);
        return_tag = true;
        return ret;
    }


    inline int getVal(const Object& obj) {
        assert(obj.type == VARIABLE || obj.type == ARRAY || obj.type == VALUE);
        if (obj.type == VALUE) return obj.value;
        else if (obj.type == VARIABLE) return var_table[obj.name].back().value;
        else return var_table[obj.name].back().getArray(obj.dims);
    }

    inline int& getVar(const Object& obj) {
        assert(obj.type == VARIABLE || obj.type == ARRAY);
        if (obj.type == VARIABLE) return var_table[obj.name].back().value;
        else return var_table[obj.name].back().getArray(obj.dims);
    }


    int Expression(Tree* cur) {
        // std::cerr << "in Expr\n";
        Object obj = Unit9(cur->children[0]);
        // std::cerr << "expr done\n";
        if (obj.type == CIN) {
            for (size_t i = 1; i < cur->children.size(); i++) {
                getVar(Unit9(cur->children[i])) = Reader::read();
            }
            return 0;
        } else if (obj.type == COUT) {
            for (size_t i = 1; i < cur->children.size(); i++) {
                Object u = Unit9(cur->children[i]);
                if (u.type == ENDL) {
                    std::cout << std::endl;
                } else {
                    std::cout << getVal(u);
                }
            }
            return 0;
        } else {
            int ret = getVal(obj);
            for (size_t i = 0; i < cur->ops.size(); i++) {
                int tmp = getVal(Unit9(cur->children[i + 1]));
                if (cur->ops[i] == "<<") ret = ret << tmp;
                else ret = ret >> tmp; 
            }
            return ret;
        }
    }

    Object Unit0(Tree* cur) {
        // std::cerr << "in 0\n";
        Object ret = cur->vars.front();
        if (ret.type == CIN || ret.type == COUT || ret.type == ENDL) {
            return ret;
        } else if (ret.type == VALUE) {
            if (!cur->children.empty())
                ret.value = Expression(cur->children[0]);
            return ret;
        } else if (ret.type == FUNCTION) {
            std::vector<int> params;
            for (auto chd : cur->children)
                params.push_back(Expression(chd));
            ret.type = VALUE;
            ret.value = Function(func_table[ret.name], params);
            return ret;
        } else if (ret.type == VARIABLE) {
            return ret;
        } else if (ret.type == ARRAY) {
            for (auto chd : cur->children) 
                ret.dims.push_back(Expression(chd));
            return ret;
        } else {
            assert(0);
            return ret;
        }
    }

    Object Unit1(Tree* cur) {
        // std::cerr << "in 1\n";
        Object ret = Unit0(cur->children[0]);
        if (!cur->ops.empty()) {
            ret = Object(VALUE, getVal(ret));
            for (auto it = cur->ops.rbegin(); it != cur->ops.rend(); ++it) {
                if (*it == "-") ret.value = -ret.value;
                else if (*it == "!") ret.value = !ret.value;
            }
        }
        return ret;
    }

    Object Unit2(Tree* cur) {
        // std::cerr << "in 2\n";
        Object ret = Unit1(cur->children[0]);
        if (!cur->ops.empty()) {
            ret = Object(VALUE, getVal(ret));
            for (size_t i = 0; i < cur->ops.size(); i++) {
                if (cur->ops[i] == "*") {
                    ret.value = ret.value * getVal(Unit1(cur->children[i + 1]));
                } else if (cur->ops[i] == "/") {
                    ret.value = ret.value / getVal(Unit1(cur->children[i + 1]));
                } else if (cur->ops[i] == "%") {
                    ret.value = ret.value % getVal(Unit1(cur->children[i + 1]));
                }
            }
        }
        return ret;
    }

    Object Unit3(Tree* cur) {
        // std::cerr << "in 3\n";
        Object ret = Unit2(cur->children[0]);
        if (!cur->ops.empty()) {
            ret = Object(VALUE, getVal(ret));
            for (size_t i = 0; i < cur->ops.size(); i++) {
                if (cur->ops[i] == "+") {
                    ret.value = ret.value + getVal(Unit2(cur->children[i + 1]));
                } else if (cur->ops[i] == "-") {
                    ret.value = ret.value - getVal(Unit2(cur->children[i + 1]));
                }
            }
        }
        return ret;
    }

    Object Unit4(Tree* cur) {
        // std::cerr << "in 4\n";
        Object ret = Unit3(cur->children[0]);
        if (!cur->ops.empty()) {
            ret = Object(VALUE, getVal(ret));
            for (size_t i = 0; i < cur->ops.size(); i++) {
                if (cur->ops[i] == "<") {
                    ret.value = (ret.value < getVal(Unit3(cur->children[i + 1])));
                } else if (cur->ops[i] == "<=") {
                    ret.value = (ret.value <= getVal(Unit3(cur->children[i + 1])));
                } else if (cur->ops[i] == ">") {
                    ret.value = (ret.value > getVal(Unit3(cur->children[i + 1])));
                } else if (cur->ops[i] == ">=") {
                    ret.value = (ret.value >= getVal(Unit3(cur->children[i + 1])));
                }
            }
        }
        return ret;
    }

    Object Unit5(Tree* cur) {
        // std::cerr << "in 5\n";
        Object ret = Unit4(cur->children[0]);
        if (!cur->ops.empty()) {
            ret = Object(VALUE, getVal(ret));
            for (size_t i = 0; i < cur->ops.size(); i++) {
                if (cur->ops[i] == "==") {
                    ret.value = (ret.value == getVal(Unit4(cur->children[i + 1])));
                } else if (cur->ops[i] == "!=") {
                    ret.value = (ret.value != getVal(Unit4(cur->children[i + 1])));
                } 
            }
        }
        return ret;
    }

    Object Unit6(Tree* cur) {
        // std::cerr << "in 6\n";
        Object ret = Unit5(cur->children[0]);
        if (!cur->ops.empty()) {
            ret = Object(VALUE, getVal(ret));
            for (size_t i = 0; i < cur->ops.size(); i++) {
                ret.value = (ret.value ^ getVal(Unit5(cur->children[i + 1])));
            }
        }
        return ret;
    }

    Object Unit7(Tree* cur) {
        // std::cerr << "in 7\n";
        Object ret = Unit6(cur->children[0]);
        if (!cur->ops.empty()) {
            ret = Object(VALUE, getVal(ret));
            for (size_t i = 0; i < cur->ops.size(); i++) {
                ret.value = (ret.value && getVal(Unit6(cur->children[i + 1])));
            }
        }
        return ret;
    }

    Object Unit8(Tree* cur) {
        // std::cerr << "in 8\n";
        Object ret = Unit7(cur->children[0]);
        if (!cur->ops.empty()) {
            ret = Object(VALUE, getVal(ret));
            for (size_t i = 0; i < cur->ops.size(); i++) {
                ret.value = (ret.value || getVal(Unit7(cur->children[i + 1])));
            }
        }
        return ret;
    }

    Object Unit9(Tree* cur) {
        // std::cerr << "in 9\n";
        std::vector<Object> rets;
        for (size_t i = 0; i < cur->children.size(); i++) {
            rets.push_back(Unit8(cur->children[i]));
        }
        if (!cur->ops.empty()) {
            for (size_t i = 0; i + 1 < cur->children.size(); i++) {
                getVar(rets[i]) = getVal(rets.back());
            }
        }
        // std::cerr << "done\n";
        // std::cerr << rets[0].name << "\n";
        return rets[0];
    }

    void Main() {
        for (auto chd : Root->children) {
            if (chd->type == VARDEF) {
                for (auto obj : chd->vars) {
                    var_table[obj.name].emplace_back(obj.type);
                    if (obj.type == ARRAY) {
                        auto& u = var_table[obj.name].back();
                        u.setArray(obj.dims);
                    }
                }
            }
        }
        Runner::Function(func_table["main"], std::vector<int>());
    }
}

int main() {
#ifdef ARK
    freopen("test.in", "r", stdin);
    freopen("error.out", "w", stderr);
#endif
    int n;
    std::cin >> n;
    for (int i = 1, j; i <= n; i++) {
        std::cin >> j;
        Reader::numbers.push_back(j);
    }
    try {
        Root = Parser::Program();
    } catch(std::string s) {
        std::cerr << s << std::endl;
    }
    // std::cerr << "parser done.\n";
    Runner::Main();
    // std::cerr << "runner done.\n";
    // for (int i = 1; i <= 20; ++i)
        // std::cout << Lexer::getLexeme().empty() << std::endl;
}
