#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>

using namespace std;

// 词法分析
void lex(ifstream &file);

// 错误处理
void error();

// 语法分析
int parse();

// PrimaryExp
string getPrimaryExp();

// UnaryExp
string getUnaryExp();

// MulExp
string getMulExp();

// AddExp
string getAddExp();

//  tokens
vector<string> tokens;

int parseDecimal(string s) {
    int num = 0;
    bool overflow = false;
    for (int i = 0; i < s.length(); i++) {
        num = num * 10 + s[i] - '0';
        if (num < 0) overflow = true;
    }
    if (overflow) return -1;
    return num;
}

int parseHex(string s) {
    int num = 0;
    bool overflow = false;
    for (int i = 0; i < s.length(); i++) {
        num = num * 16 + s[i] - '0';
        if (num < 0) overflow = true;
    }
    if (overflow) return -1;
    return num;
}

int parseOctal(string s) {
    int num = 0;
    bool overflow = false;
    for (int i = 0; i < s.length(); i++) {
        num = num * 8 + s[i] - '0';
        if (num < 0) overflow = true;
    }
    if (overflow) return -1;
    return num;
}

string getNumber(string s) {
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        int num = parseHex(s.substr(2));
        return num == -1 ? "overflow" : to_string(num);
    } else if (s[0] == '0') {
        int num = parseOctal(s.substr(1));
        return num == -1 ? "overflow" : to_string(num);
    } else {
        int num = parseDecimal(s);
        return num == -1 ? "overflow" : to_string(num);
    }
}

// 词法分析
void lex(ifstream &file) {
    string code;

    while (getline(file, code)) {
        int i = 0;
        while (i < code.size()) {
            if (code[i] == '/' && code[i + 1] == '/') {             // 遇到单行注释，则直接跳过，直到读取一个非空行
                while (getline(file, code) && (code[0] != '\n' && code[0] != '\r'));
            }
            if (code[i] == '/' && code[i + 1] == '*') {
                int comment_nest = 1;                               // 追踪注释的嵌套层数
                while (true) {
                    while (i < code.size() && code[i] != '\n') {    // 在当前行追踪
                        if (code[i] == '*')
                            if (code[i + 1] == '/')
                                comment_nest--;
                            else if (code[i] == '/' && code[i + 1] == '*')
                                comment_nest++;
                        i++;
                    }
                    if (comment_nest == 0) break;                   // 如果在当前行的嵌套注释结束，则结束多行注释的判断。否则读取下一行
                    getline(file, code);
                    if (file.eof()) break;
                    i = 0;
                }
                if (comment_nest != 0) {                            // 添加的逻辑
                    cout << "未正确多行注释!" << endl;
                    exit(1);
                }
            } else if (isalpha(code[i])) {                          // 标识符
                string id;
                while (i < code.size() && isalnum(code[i])) {
                    id += code[i];
                    i++;
                }
                tokens.push_back(id);
            } else if (isdigit(code[i]) || code[i] == 'x' || code[i] == 'X' || code[i] == '0') {   // 数字
                string num;
                while (i < code.size() &&
                       (isdigit(code[i]) || (code[i] >= 'a' && code[i] <= 'f') || (code[i] >= 'A' && code[i] <= 'F'))) {
                    num += code[i];
                    i++;
                }
                tokens.push_back(num);
            } else if (code[i] == ' ' || code[i] == '\n' || code[i] == '\r') {
                i++;
            } else {                                                // 运算符和分隔符
                string op(1, code[i]);
                tokens.push_back(op);
                i++;
            }
        }
    }
}

// 错误处理
void error() {
    cout << "Compilation error!" << endl;
    exit(1);
}

// 语法分析
int parse() {
    // CompUnit -> FuncDef
    if (tokens[0] != "int") error();
    tokens.erase(tokens.begin());

    if (tokens[0] != "main") error();
    tokens.erase(tokens.begin());

    // FuncDef -> FuncType Ident '(' ')' Block
    if (tokens[0] != "(") error();
    tokens.erase(tokens.begin());

    if (tokens[0] != ")") error();
    tokens.erase(tokens.begin());

    // Block -> '{' Stmt '}'
    if (tokens[0] != "{") error();
    tokens.erase(tokens.begin());

    // Stmt -> 'return' Exp ';'
    if (tokens[0] != "return") error();
    tokens.erase(tokens.begin());

    // Exp -> AddExp
    string exp_str = getAddExp();

    if (tokens[0] != ";") error();
    tokens.erase(tokens.begin());

    if (tokens[0] != "}") error();
    tokens.erase(tokens.begin());

    cout << "define dso_local i32 @main(){" << endl;
    cout << "    %1 = " << exp_str << endl;
    cout << "    ret i32 %1 " << endl;
    cout << "}" << endl;

    return 0;
}

// PrimaryExp -> '(' Exp ')' | Number
string getPrimaryExp() {
    if (tokens[0] == "(") {
        tokens.erase(tokens.begin());
        string exp_str = getAddExp();
        if (tokens[0] != ")") error();
        tokens.erase(tokens.begin());
        return exp_str;
    } else {
        string num_str = getNumber(tokens[0]);
        tokens.erase(tokens.begin());
        return num_str;
    }
}

// UnaryExp -> PrimaryExp | UnaryOp UnaryExp
string getUnaryExp() {
    if (tokens[0] == "+" || tokens[0] == "-") {
        string op = tokens[0];
        tokens.erase(tokens.begin());
        string exp_str = getUnaryExp();
        return op + " " + exp_str;
    } else {
        return getPrimaryExp();
    }
}

// MulExp -> UnaryExp | UnaryExp UnaryOp UnaryExp
string getMulExp() {
    string exp_str = getUnaryExp();
    while (tokens[0] == "*" || tokens[0] == "/") {
        string op = tokens[0];
        tokens.erase(tokens.begin());
        string right = getUnaryExp();
        exp_str = exp_str + " " + op + " " + right;
    }
    return exp_str;
}

// AddExp -> MulExp
string getAddExp() {
    string exp_str = getMulExp();
    while (tokens[0] == "+" || tokens[0] == "-") {
        string op = tokens[0];
        tokens.erase(tokens.begin());
        string right = getMulExp();
        exp_str = exp_str + " " + op + " " + right;
    }
    return exp_str;
}

int main() {
    ifstream file("source_lab1.txt");
    if (file.is_open()) {
        lex(file); // std::ifstream类型的对象持有文件资源,必须传引用,否则简单的拷贝构造或赋值不会复制这些资源。
    } else {
        cout << "Unable to open file";
    }
    parse();
    return 0;
}