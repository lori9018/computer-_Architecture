#include "CompilationEngine.h"
#include <iostream>

// 建構子
CompilationEngine::CompilationEngine(std::string inputFile, std::string outputFile) {
    tokenizer = new JackTokenizer(inputFile);
    this->outputFile.open(outputFile);
    indentLevel = 0;
}

// 解構子
CompilationEngine::~CompilationEngine() {
    if (outputFile.is_open()) outputFile.close();
    delete tokenizer;
}

// 輔助：處理縮排並輸出 <tag> value </tag>
void CompilationEngine::printXML(std::string tagName, std::string value) {
    std::string spaces(indentLevel * 2, ' ');
    // 處理 XML 特殊字元
    if (value == "<") value = "&lt;";
    else if (value == ">") value = "&gt;";
    else if (value == "&") value = "&amp;";
    else if (value == "\"") value = "&quot;";
    
    outputFile << spaces << "<" << tagName << "> " << value << " </" << tagName << ">\n";
}

// 輔助：開標籤 <tag>
void CompilationEngine::printOpenTag(std::string tagName) {
    std::string spaces(indentLevel * 2, ' ');
    outputFile << spaces << "<" << tagName << ">\n";
    indentLevel++;
}

// 輔助：關標籤 </tag>
void CompilationEngine::printCloseTag(std::string tagName) {
    indentLevel--;
    std::string spaces(indentLevel * 2, ' ');
    outputFile << spaces << "</" << tagName << ">\n";
}

// 核心：吃掉預期的 Token 並推進
void CompilationEngine::process(std::string expected) {
    if (tokenizer->getCurrentTokenString() == expected) {
        std::string tag = "";
        TokenType type = tokenizer->tokenType();
        if (type == KEYWORD) tag = "keyword";
        else if (type == SYMBOL) tag = "symbol";
        
        printXML(tag, tokenizer->getCurrentTokenString());
        tokenizer->advance();
    } else {
        std::cerr << "Syntax Error: Expected " << expected << " but got " << tokenizer->getCurrentTokenString() << std::endl;
        exit(1);
    }
}

// 核心：吃掉特定類型的 Token (如 Identifier)
void CompilationEngine::processType(std::string expectedTag) {
    std::string val = tokenizer->getCurrentTokenString();
    
    // 這裡做個簡單對應，實際可以更嚴謹
    std::string currentTag = "";
    TokenType type = tokenizer->tokenType();
    
    if (type == IDENTIFIER) currentTag = "identifier";
    else if (type == INT_CONST) currentTag = "integerConstant";
    else if (type == STRING_CONST) currentTag = "stringConstant";
    else if (type == KEYWORD) currentTag = "keyword"; // 有時 type 會是 int/char 等 keyword

    printXML(expectedTag.empty() ? currentTag : expectedTag, val);
    tokenizer->advance();
}
void CompilationEngine::compileClass() {
    if (tokenizer->hasMoreTokens()) tokenizer->advance(); // 讀入第一個 token

    printOpenTag("class");
    process("class");
    processType("identifier"); // className
    process("{");

    // 處理 ClassVarDec (static/field)
    while (tokenizer->getCurrentTokenString() == "static" || tokenizer->getCurrentTokenString() == "field") {
        compileClassVarDec();
    }

    // 處理 Subroutine (constructor/function/method)
    while (tokenizer->getCurrentTokenString() == "constructor" || 
           tokenizer->getCurrentTokenString() == "function" || 
           tokenizer->getCurrentTokenString() == "method") {
        compileSubroutine();
    }

    process("}");
    printCloseTag("class");
}

void CompilationEngine::compileClassVarDec() {
    printOpenTag("classVarDec");
    // static or field
    process(tokenizer->getCurrentTokenString()); 
    // type (int, boolean, ClassName...)
    processType(""); 
    // varName
    processType("identifier");

    // 處理逗號分隔的變數: , var2, var3
    while (tokenizer->getCurrentTokenString() == ",") {
        process(",");
        processType("identifier");
    }
    process(";");
    printCloseTag("classVarDec");
}

void CompilationEngine::compileSubroutine() {
    printOpenTag("subroutineDec");
    
    // 1. constructor/function/method
    process(tokenizer->getCurrentTokenString());
    // 2. void or type
    processType(""); 
    // 3. subroutineName
    processType("identifier");
    // 4. ( parameterList )
    process("(");
    compileParameterList();
    process(")");

    // 5. Body
    printOpenTag("subroutineBody");
    process("{");
    // varDecs
    while (tokenizer->getCurrentTokenString() == "var") {
        compileVarDec();
    }
    // statements
    compileStatements();
    process("}");
    printCloseTag("subroutineBody");

    printCloseTag("subroutineDec");
}

void CompilationEngine::compileParameterList() {
    printOpenTag("parameterList");
    
    // 如果不是 ')', 表示有參數
    if (tokenizer->getCurrentTokenString() != ")") {
        processType(""); // type
        processType("identifier"); // name

        while (tokenizer->getCurrentTokenString() == ",") {
            process(",");
            processType("");
            processType("identifier");
        }
    }
    printCloseTag("parameterList");
}

void CompilationEngine::compileVarDec() {
    printOpenTag("varDec");
    process("var");
    processType(""); // type
    processType("identifier"); // name

    while (tokenizer->getCurrentTokenString() == ",") {
        process(",");
        processType("identifier");
    }
    process(";");
    printCloseTag("varDec");
}

void CompilationEngine::compileStatements() {
    printOpenTag("statements");
    // 只要是 let, if, while, do, return 開頭，就一直編譯
    while (true) {
        std::string token = tokenizer->getCurrentTokenString();
        if (token == "let") compileLet();
        else if (token == "if") compileIf();
        else if (token == "while") compileWhile();
        else if (token == "do") compileDo();
        else if (token == "return") compileReturn();
        else break;
    }
    printCloseTag("statements");
}

void CompilationEngine::compileLet() {
    printOpenTag("letStatement");
    process("let");
    processType("identifier"); // varName

    // 處理陣列賦值: let arr[i] = ...
    if (tokenizer->getCurrentTokenString() == "[") {
        process("[");
        compileExpression();
        process("]");
    }

    process("=");
    compileExpression();
    process(";");
    printCloseTag("letStatement");
}

void CompilationEngine::compileDo() {
    printOpenTag("doStatement");
    process("do");
    // do subroutineCall; 
    // 這裡比較tricky，因為 subroutineCall 可能是 name(args) 或 Class.name(args)
    // 我們先讀第一個 identifier
    processType("identifier");
    
    if (tokenizer->getCurrentTokenString() == "(") {
        process("(");
        compileExpressionList();
        process(")");
    } else if (tokenizer->getCurrentTokenString() == ".") {
        process(".");
        processType("identifier"); // method name
        process("(");
        compileExpressionList();
        process(")");
    }
    
    process(";");
    printCloseTag("doStatement");
}

void CompilationEngine::compileReturn() {
    printOpenTag("returnStatement");
    process("return");
    if (tokenizer->getCurrentTokenString() != ";") {
        compileExpression();
    }
    process(";");
    printCloseTag("returnStatement");
}

// If 和 While 類似，請依此類推
void CompilationEngine::compileIf() {
    printOpenTag("ifStatement");
    process("if");
    process("(");
    compileExpression();
    process(")");
    process("{");
    compileStatements();
    process("}");
    
    if (tokenizer->getCurrentTokenString() == "else") {
        process("else");
        process("{");
        compileStatements();
        process("}");
    }
    printCloseTag("ifStatement");
}

void CompilationEngine::compileWhile() {
    printOpenTag("whileStatement");
    process("while");
    process("(");
    compileExpression();
    process(")");
    process("{");
    compileStatements();
    process("}");
    printCloseTag("whileStatement");
}

// 判斷是否為運算子 (+, -, *, /, &, |, <, >, =)
bool CompilationEngine::isOp() {
    std::string t = tokenizer->getCurrentTokenString();
    return (t == "+" || t == "-" || t == "*" || t == "/" || 
            t == "&" || t == "|" || t == "<" || t == ">" || t == "=");
}

void CompilationEngine::compileExpression() {
    printOpenTag("expression");
    compileTerm();
    // (op term)*
    while (isOp()) {
        process(tokenizer->getCurrentTokenString()); // op
        compileTerm();
    }
    printCloseTag("expression");
}

void CompilationEngine::compileTerm() {
    printOpenTag("term");
    
    std::string token = tokenizer->getCurrentTokenString();
    TokenType type = tokenizer->tokenType();

    if (type == INT_CONST || type == STRING_CONST || token == "true" || token == "false" || token == "null" || token == "this") {
        // 常數或關鍵字常數
        processType(""); 
    } else if (token == "(") {
        // (expression)
        process("(");
        compileExpression();
        process(")");
    } else if (token == "-" || token == "~") {
        // UnaryOp term (例如 -x, ~b)
        process(token);
        compileTerm();
    } else if (type == IDENTIFIER) {
        // 這部分最複雜，可能是 var, var[i], func(), Class.func()
        // 我們需要偷看下一個符號 (Lookahead)
        // 但目前的 Tokenizer 架構只能取得當前。
        // 不過因為我們還沒 advance，我們可以先記錄下當前 identifier
        
        // 技巧：先 advance 讀下一個，如果不匹配再處理比較麻煩。
        // 在我們的架構中，tokenizer->advance() 會破壞狀態。
        // 所以這裡我們利用一個小技巧：
        // 我們先不要 process identifier，而是先確認下一個是什麼。
        // 但我們的 tokenizer 沒有 peek()。
        // 解決方案：先輸出 identifier，然後看下一個 token 決定做什麼。
        
        processType("identifier"); // 先輸出變數名
        
        std::string nextToken = tokenizer->getCurrentTokenString();
        
        if (nextToken == "[") {
            // Array: var[expression]
            process("[");
            compileExpression();
            process("]");
        } else if (nextToken == "(") {
            // Subroutine: func(args)
            process("(");
            compileExpressionList();
            process(")");
        } else if (nextToken == ".") {
            // Method: Class.func(args)
            process(".");
            processType("identifier");
            process("(");
            compileExpressionList();
            process(")");
        }
    }
    
    printCloseTag("term");
}

int CompilationEngine::compileExpressionList() {
    printOpenTag("expressionList");
    int count = 0;
    if (tokenizer->getCurrentTokenString() != ")") {
        compileExpression();
        count++;
        while (tokenizer->getCurrentTokenString() == ",") {
            process(",");
            compileExpression();
            count++;
        }
    }
    printCloseTag("expressionList");
    return count;
}