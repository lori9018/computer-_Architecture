#include <iostream>
#include <string>
#include "CompilationEngine.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./JackAnalyzer <filename.jack>" << std::endl;
        return 1;
    }

    std::string inputFilename = argv[1];
    // 根據題目要求，Compiler 的輸出檔名應該是 .xml (例如 Main.xml)
    // 注意：之前的 tokenizer 測試是 T.xml，現在是正式編譯，只要 .xml
    std::string outputFilename = inputFilename.substr(0, inputFilename.find_last_of('.')) + ".xml";

    std::cout << "Compiling: " << inputFilename << " -> " << outputFilename << std::endl;

    CompilationEngine engine(inputFilename, outputFilename);
    
    // 啟動編譯！
    engine.compileClass(); 

    std::cout << "Done." << std::endl;
    return 0;
}