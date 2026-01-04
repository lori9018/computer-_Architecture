#include "JackTokenizer.h"
#include <iostream>
#include <sstream>

JackTokenizer::JackTokenizer(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }

    // 1. 讀取整個檔案內容到字串
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    // 2. 移除註解
    removeComments(content);

    // 3. 切割 Token
    tokenize(content);

    // 初始化指標
    currentTokenIdx = -1;
    currentTokenType = NONE;
}

// 移除註解邏輯 (處理 // 和 /* */)
void JackTokenizer::removeComments(std::string& source) {
    std::string clean = "";
    int n = source.length();
    bool inString = false; // 判斷是否在字串內 (字串內的 // 不算註解)

    for (int i = 0; i < n; i++) {
        // 處理字串狀態
        if (source[i] == '"') {
            inString = !inString;
            clean += source[i];
            continue;
        }

        if (!inString) {
            // 檢查單行註解 //
            if (i + 1 < n && source[i] == '/' && source[i+1] == '/') {
                while (i < n && source[i] != '\n') i++; // 跳過直到換行
                clean += '\n'; // 保留換行符以防黏合
                continue;
            }
            // 檢查多行註解 /* */
            if (i + 1 < n && source[i] == '/' && source[i+1] == '*') {
                i += 2;
                while (i + 1 < n && !(source[i] == '*' && source[i+1] == '/')) i++;
                i++; // 跳過 '/'
                clean += ' '; // 補個空白以防黏合
                continue;
            }
        }
        clean += source[i];
    }
    source = clean;
}

bool JackTokenizer::isSymbol(char c) {
    return symbolsSet.find(c) != symbolsSet.end();
}

// 核心切割邏輯
void JackTokenizer::tokenize(std::string source) {
    int n = source.length();
    std::string buffer = "";

    for (int i = 0; i < n; i++) {
        char c = source[i];

        if (std::isspace(c)) {
            continue; // 跳過空白
        } else if (isSymbol(c)) {
            tokens.push_back(std::string(1, c)); // 符號單獨成 Token
        } else if (c == '"') {
            // 字串常數：讀取直到下一個引號
            buffer = "\"";
            i++;
            while (i < n && source[i] != '"') {
                buffer += source[i];
                i++;
            }
            buffer += "\""; // 補上結束引號
            tokens.push_back(buffer);
        } else if (std::isdigit(c)) {
            // 整數常數
            buffer = "";
            while (i < n && std::isdigit(source[i])) {
                buffer += source[i];
                i++;
            }
            tokens.push_back(buffer);
            i--; // 回退一格，因為 for loop 會 +1
        } else {
            // 關鍵字 或 識別字 (Identifier)
            buffer = "";
            while (i < n && !isSymbol(source[i]) && !std::isspace(source[i])) {
                buffer += source[i];
                i++;
            }
            tokens.push_back(buffer);
            i--; // 回退
        }
    }
}

bool JackTokenizer::hasMoreTokens() {
    return currentTokenIdx + 1 < tokens.size();
}

void JackTokenizer::advance() {
    if (hasMoreTokens()) {
        currentTokenIdx++;
        currentToken = tokens[currentTokenIdx];

        // 判斷類型
        if (keywordsSet.find(currentToken) != keywordsSet.end()) {
            currentTokenType = KEYWORD;
        } else if (symbolsSet.find(currentToken[0]) != symbolsSet.end()) {
            currentTokenType = SYMBOL;
        } else if (std::isdigit(currentToken[0])) {
            currentTokenType = INT_CONST;
        } else if (currentToken[0] == '"') {
            currentTokenType = STRING_CONST;
        } else {
            currentTokenType = IDENTIFIER;
        }
    }
}

TokenType JackTokenizer::tokenType() {
    return currentTokenType;
}

KeyWord JackTokenizer::keyWord() {
    if (currentToken == "class") return CLASS;
    if (currentToken == "method") return METHOD;
    if (currentToken == "function") return FUNCTION;
    if (currentToken == "constructor") return CONSTRUCTOR;
    if (currentToken == "int") return INT;
    if (currentToken == "boolean") return BOOLEAN;
    if (currentToken == "char") return CHAR;
    if (currentToken == "void") return VOID;
    if (currentToken == "var") return VAR;
    if (currentToken == "static") return STATIC;
    if (currentToken == "field") return FIELD;
    if (currentToken == "let") return LET;
    if (currentToken == "do") return DO;
    if (currentToken == "if") return IF;
    if (currentToken == "else") return ELSE;
    if (currentToken == "while") return WHILE;
    if (currentToken == "return") return RETURN;
    if (currentToken == "true") return TRUE;
    if (currentToken == "false") return FALSE;
    if (currentToken == "null") return NULL_VAL;
    if (currentToken == "this") return THIS;
    return CLASS; // Default should not happen
}

char JackTokenizer::symbol() {
    return currentToken[0];
}

std::string JackTokenizer::identifier() {
    return currentToken;
}

int JackTokenizer::intVal() {
    return std::stoi(currentToken);
}

std::string JackTokenizer::stringVal() {
    // 移除前後的雙引號
    return currentToken.substr(1, currentToken.length() - 2);
}