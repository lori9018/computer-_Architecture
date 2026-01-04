#ifndef COMPILATIONENGINE_H
#define COMPILATIONENGINE_H

#include "JackTokenizer.h"
#include <fstream>
#include <string>

class CompilationEngine {
private:
    JackTokenizer* tokenizer;
    std::ofstream outputFile;
    int indentLevel; // 用於控制 XML 縮排

    // 內部輔助函式
    void printXML(std::string tagName, std::string value);
    void printOpenTag(std::string tagName);
    void printCloseTag(std::string tagName);
    
    // 驗證並輸出當前 Token，然後推進到下一個
    void process(std::string expected); 
    // 驗證並輸出特定類型的 Token (如 Identifier, Integer)
    void processType(std::string expectedType); 
    // 用於檢查當前 Token 是否為運算子 (+, -, *, / ...)
    bool isOp();

public:
    CompilationEngine(std::string inputFile, std::string outputFile);
    ~CompilationEngine();

    // 所有的編譯規則 (對應書中的 API)
    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    int compileExpressionList(); // 回傳參數個數
};

#endif