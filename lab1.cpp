#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <stack>

using namespace std;

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

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

//  tokens
vector<string> tokens;

void lex(ifstream &file) {
    string code;
    stack<bool> comment_stack;

    while (getline(file, code)) {
        int i = 0;
        while (i < code.size()) {
            if (!comment_stack.empty()) {                       // 在多行注释中
                if (code[i] == '*' && i < code.size() - 1 && code[i + 1] == '/') {
                    comment_stack.pop();                        // 多行注释结束
                    i += 2;
                } else {
                    i++;
                }
            } else {
                if (code[i] == '/' && i < code.size() - 1) {
                    if (code[i + 1] == '/') {                   // 单行注释
                        break;
                    } else if (code[i + 1] == '*') {            // 多行注释
                        comment_stack.push(true);
                        i += 2;
                    } else {
                        tokens.push_back(string(1, code[i]));   // 运算符和分隔符
                        i++;
                    }
                } else if (isalpha(code[i])) {                  // 标识符
                    string id;
                    while (i < code.size() && isalnum(code[i])) {
                        id += code[i];
                        i++;
                    }
                    tokens.push_back(id);
                } else if (isdigit(code[i]) || code[i] == 'x' || code[i] == 'X' || code[i] == '0') {   // 数字
                    string num;
                    while (i < code.size() &&
                           (isdigit(code[i]) || (code[i] >= 'a' && code[i] <= 'f') ||
                            (code[i] >= 'A' && code[i] <= 'F'))) {
                        num += code[i];
                        i++;
                    }
                    tokens.push_back(num);
                } else if (code[i] == ' ' || code[i] == '\n' || code[i] == '\r') {
                    i++;
                } else {                                        // 运算符和分隔符
                    tokens.push_back(string(1, code[i]));
                    i++;
                }
            }
        }
    }

    if (!comment_stack.empty() && comment_stack.top()) {
        cout << "未正确多行注释!" << endl;
        exit(1);
    }
}

// 错误处理
void error() {
    cout << "Compilation error!" << endl;
    exit(1);
}

// 语法分析
int parse() {
    if (tokens[0] != "int") error();
    tokens.erase(tokens.begin());

    if (tokens[0] != "main") error();
    tokens.erase(tokens.begin());

    if (tokens[0] != "(") error();
    tokens.erase(tokens.begin());

    if (tokens[0] != ")") error();
    tokens.erase(tokens.begin());

    if (tokens[0] != "{") error();
    tokens.erase(tokens.begin());

    if (tokens[0] != "return") error();
    tokens.erase(tokens.begin());

    string num_str = getNumber(tokens[0]);
    if (num_str == "overflow")error();
    tokens.erase(tokens.begin());

    if (tokens[0] != ";") error();
    tokens.erase(tokens.begin());

    if (tokens[0] != "}") error();
    tokens.erase(tokens.begin());

    cout << "define dso_local i32 @main(){" << endl;
    cout << "    ret i32 " << num_str << endl;
    cout << "}" << endl;

    return 0;
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