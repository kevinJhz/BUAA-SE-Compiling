#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <stack>

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

string getIdentOrNumber(string s) {
    if (isDigit(s[0])) {
        string num_str = getNumber(s);
        return num_str == "overflow" ? num_str
                                     : "Number(" + num_str + ")";
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

//
/**
 * @brief 对输入的一行字符串进行词法分析
 * @param[in] line 一行文本
 * @param[out] string 若返回空串""，则说明发生错误
 */
string analyzeLine(string line) {
    int i = 0;                                                  // 当前字符索引
    while (i < line.length()) {
        int len = 0;                                            // 关键字长度
        for (int j = i; j < line.length(); j++, len++) {        // 计算关键字长度
            if (!isLetter(line[j])) break;
        }
        if (isKeyword(line.substr(i, len))) {                // 判断是否为关键字
            cout << getTokenType(line.substr(i, len)) << endl;
            i += len;                                           // 跳过已匹配的关键字
        } else if (isOperator(line.substr(i, 1))) {       // 判断是否为运算符
            if (isEq(line, i)) {
                cout << getTokenType("==") << endl;
                i += 2;
            } else {
                cout << getTokenType(line.substr(i, 1)) << endl;
                i++;
            }
        } else if (isDelimiter(line.substr(i, 1))) {      // 判断是否为界符
            cout << getTokenType(line.substr(i, 1)) << endl;
            i++;
        } else if (isLetter(line[i]) || isDigit(line[i])) {     // 判断是否为标识符或整数
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




//// 词法分析
//void lex(ifstream &file) {
//    string code;
//
//    while (getline(file, code)) {
//        int i = 0;
//        while (i < code.size()) {
//            if (code[i] == '/' && code[i + 1] == '/') {             // 遇到单行注释，则直接跳过，直到读取一个非空行
//                do {
//                    getline(file, code);
//                } while (code[0] == '\n' || code[0] == '\r');
//                i = 0;
//            }
//            if (code[i] == '/' && code[i + 1] == '*') {
//                int comment_nest = 1;
//                int j = i;
//
//                while (comment_nest > 0) {
//                    while (j < code.size() && code[j] != '\n') {
//                        if (code[j] == '*') {
//                            if (code[j + 1] == '/') {
//                                comment_nest--;
//                            } else if (code[j + 1] == '*')
//                                comment_nest++;
//                        }
//                        j++;
//                    }
//                    if (file.eof()) {
//                        cout << "多行注释未关闭!" << endl;
//                        exit(1);
//                    }
//                    getline(file, code);
//                    j = 0;
//                }
//                i = j;
//            }
//            if (isalpha(code[i])) {                          // 标识符
//                string id;
//                while (i < code.size() && isalnum(code[i])) {
//                    id += code[i];
//                    i++;
//                }
//                tokens.push_back(id);
//
//            } else if (isdigit(code[i]) || code[i] == 'x' || code[i] == 'X' || code[i] == '0') {   // 数字
//                string num;
//                while (i < code.size() &&
//                       (isdigit(code[i]) || (code[i] >= 'a' && code[i] <= 'f') || (code[i] >= 'A' && code[i] <= 'F'))) {
//                    num += code[i];
//                    i++;
//                }
//                tokens.push_back(num);
//            } else if (code[i] == ' ' || code[i] == '\n' || code[i] == '\r') {
//                i++;
//            } else {                                                // 运算符和分隔符
//                string op(1, code[i]);
//                tokens.push_back(op);
//                i++;
//            }
//        }
//    }
//}

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