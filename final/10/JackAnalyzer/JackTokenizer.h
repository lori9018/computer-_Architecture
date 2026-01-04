#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H

#include <string>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <algorithm>

// 定義 Token 類型
enum TokenType {
    KEYWORD, SYMBOL, IDENTIFIER, INT_CONST, STRING_CONST, NONE
};

// 定義 Keyword 類型 (對應 Jack 的關鍵字)
enum KeyWord {
    CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT, BOOLEAN, CHAR, VOID,
    VAR, STATIC, FIELD, LET, DO, IF, ELSE, WHILE, RETURN,
    TRUE, FALSE, NULL_VAL, THIS
};

class JackTokenizer {
private:
    std::vector<std::string> tokens; // 儲存分割好的所有 Token 字串
    int currentTokenIdx;             // 當前指向第幾個 Token
    std::string currentToken;        // 當前 Token 的字串內容
    TokenType currentTokenType;      // 當前 Token 的類型

    // 關鍵字對照表 (字串 -> KeyWord Enum)
    const std::unordered_set<std::string> keywordsSet = {
        "class", "constructor", "function", "method", "field", "static", "var",
        "int", "char", "boolean", "void", "true", "false", "null", "this",
        "let", "do", "if", "else", "while", "return"
    };

    const std::unordered_set<char> symbolsSet = {
        '{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'
    };

    // 內部輔助函式
    void removeComments(std::string& source);
    void tokenize(std::string source);
    bool isSymbol(char c);

public:
    JackTokenizer(std::string filename);
    bool hasMoreTokens();
    void advance();
    TokenType tokenType();
    
    // 取得值的函式
    KeyWord keyWord();
    char symbol();
    std::string identifier();
    int intVal();
    std::string stringVal();
    
    // 額外輔助：直接取得當前 Token 字串 (給 CompilationEngine 用方便)
    std::string getCurrentTokenString() { return currentToken; }
};

#endif