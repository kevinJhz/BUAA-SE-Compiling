#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>

using namespace std;

unordered_map<string, string> tokenMap;

void init() {
    tokenMap["if"] = "If";
    tokenMap["else"] = "Else";
    tokenMap["while"] = "While";
    tokenMap["break"] = "Break";
    tokenMap["continue"] = "Continue";
    tokenMap["return"] = "Return";
    tokenMap["="] = "Assign";
    tokenMap[";"] = "Semicolon";
    tokenMap["("] = "LPar";
    tokenMap[")"] = "RPar";
    tokenMap["{"] = "LBrace";
    tokenMap["}"] = "RBrace";
    tokenMap["+"] = "Plus";
    tokenMap["*"] = "Mult";
    tokenMap["/"] = "Div";
    tokenMap["<"] = "Lt";
    tokenMap[">"] = "Gt";
    tokenMap["=="] = "Eq";
}

string getTokenType(string s) {
    return tokenMap[s];
}

bool isKeyword(string s) {
    return tokenMap.find(s) != tokenMap.end();
}

bool isOperator(string s) {
    return tokenMap.find(s) != tokenMap.end();
}

bool isDelimiter(string s) {
    return tokenMap.find(s) != tokenMap.end();
}

bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

string getIdentOrNumber(string s) {
    if (isDigit(s[0])) {
        int num = 0;
        bool overflow = false;
        for (int i = 0; i < s.length(); i++) {
            num = num * 10 + s[i] - '0';
            if (num < 0) overflow = true;
        }
        if (overflow) {
            return "overflow";
        } else {
            return "Number(" + to_string(num) + ")";
        }
    } else {
        return "Ident(" + s + ")";
    }
}

bool isEq(string line, int i) {
    if (line[i] == '=') {
        if (line[i + 1] == '=') {
            return true;  // 判断为==运算符
        }
    }
    return false;
}

// 对输入的一行字符串进行词法分析
string analyzeLine(string line) {
    int i = 0;                                              // 当前字符索引
    while (i < line.length()) {
        int len = 0;                                        // 关键字长度
        for (int j = i; j < line.length(); j++, len++) {    // 计算关键字长度
            if (!isLetter(line[j])) break;
        }
        if (isKeyword(line.substr(i, len))) {               // 判断是否为关键字
            cout << getTokenType(line.substr(i, len)) << endl;
            i += len;                                       // 跳过已匹配的关键字
        } else if (isOperator(line.substr(i, 1))) {                   // 判断是否为运算符
            if (isEq(line, i)) {
                cout << getTokenType("==") << endl;
                i += 2;
            } else {
                cout << getTokenType(line.substr(i, 1)) << endl;
                i++;
            }
        } else if (isDelimiter(line.substr(i, 1))) {                  // 判断是否为界符
            cout << getTokenType(line.substr(i, 1)) << endl;
            i++;
        } else if (isLetter(line[i]) || isDigit(line[i])) { // 判断是否为标识符或整数
            int j = i;
            while (j < line.length() && (isLetter(line[j]) || isDigit(line[j]))) j++;
            string IdentOrNum = getIdentOrNumber(line.substr(i, j - i));
            if (IdentOrNum == "overflow") {
                cout << "Err" << endl;
                return "";
            }
            cout << IdentOrNum << endl;
            i = j;
        } else if (line[i] == ' ' || line[i] == '\n' || line[i] == '\r') {
            i++;
        } else {     // 未匹配任何规则,输出错误并终止
            cout << "Err" << endl;
            return "";
        }
    }
    return line;      // 返回剩余未分析的输入字符串
}

int main() {
    ifstream file("source.txt");
    init();
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            line = analyzeLine(line);
            if (line == "") break;
        }
    } else {
        cout << "Unable to open file";
    }
    return 0;
}