#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // 用於讀取資料夾
#include <sys/stat.h>

#define MAX_LINE_LENGTH 200
#define MAX_FILENAME 256

// 全域計數器
int labelCount = 0;
int callCount = 0;

// 函式宣告
void processFile(FILE *outputFile, char *filePath, char *fileNameOnly);
void writeBootstrap(FILE *fp);
void writeArithmetic(FILE *fp, char *command);
void writePushPop(FILE *fp, char *command, char *segment, int index, char *fileName);
void writeBranching(FILE *fp, char *command, char *label);
void writeFunction(FILE *fp, char *functionName, int nVars);
void writeReturn(FILE *fp);
void writeCall(FILE *fp, char *functionName, int numArgs);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <directory path>\n", argv[0]);
        return 1;
    }

    char *dirPath = argv[1];
    
    // 處理輸出檔名：使用資料夾名稱作為輸出檔名 (例如 directory.asm)
    char outputFileName[MAX_FILENAME];
    char *lastSlash = strrchr(dirPath, '/');
    char *dirNameOnly = (lastSlash != NULL) ? lastSlash + 1 : dirPath;
    // 如果路徑結尾是 / (例如 ./dir/)，需特殊處理，這裡簡化處理直接拼路徑
    // 簡單作法：直接在該目錄下產生 Output.asm 或者 目錄名.asm
    // 為了相容測試腳本，我們嘗試產生 <dirPath>/<dirName>.asm
    
    char outputPath[MAX_FILENAME * 2];
    sprintf(outputPath, "%s/%s.asm", dirPath, dirNameOnly);
    
    // 如果是在當前目錄執行，路徑修正
    if(strcmp(dirPath, ".") == 0) {
        strcpy(outputPath, "Output.asm");
    }

    FILE *outputFile = fopen(outputPath, "w");
    if (outputFile == NULL) {
        printf("Error: Could not create output file %s\n", outputPath);
        return 1;
    }

    printf("Generating: %s\n", outputPath);

    // 1. 寫入 Bootstrap Code (FibonacciElement 必須)
    writeBootstrap(outputFile);

    // 2. 開啟資料夾並遍歷所有 .vm 檔案
    DIR *d;
    struct dirent *dir;
    d = opendir(dirPath);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // 檢查副檔名是否為 .vm
            char *dot = strrchr(dir->d_name, '.');
            if (dot && strcmp(dot, ".vm") == 0) {
                // 組合完整路徑
                char inputPath[MAX_FILENAME * 2];
                sprintf(inputPath, "%s/%s", dirPath, dir->d_name);
                
                // 取得純檔名 (不含副檔名)，用於 Static 變數標籤
                char fileNameOnly[MAX_FILENAME];
                strncpy(fileNameOnly, dir->d_name, dot - dir->d_name);
                fileNameOnly[dot - dir->d_name] = '\0';

                printf("Processing: %s\n", dir->d_name);
                processFile(outputFile, inputPath, fileNameOnly);
            }
        }
        closedir(d);
    } else {
        // 如果無法開啟資料夾，嘗試當作單一檔案處理
        processFile(outputFile, dirPath, "SingleFile");
    }

    fclose(outputFile);
    printf("Done.\n");
    return 0;
}

// 寫入引導程式
void writeBootstrap(FILE *fp) {
    fprintf(fp, "// Bootstrap Code\n");
    fprintf(fp, "@256\n");
    fprintf(fp, "D=A\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=D\n");
    writeCall(fp, "Sys.init", 0);
}

// 處理單一檔案的核心邏輯
void processFile(FILE *outputFile, char *filePath, char *fileNameOnly) {
    FILE *inputFile = fopen(filePath, "r");
    if (inputFile == NULL) {
        printf("Error: Could not open input file %s\n", filePath);
        return;
    }

    char line[MAX_LINE_LENGTH];
    char *token;
    char command[50], arg1[100];
    int arg2;

    while (fgets(line, sizeof(line), inputFile)) {
        // 去除註解與空白
        char *comment = strchr(line, '/');
        if (comment) *comment = '\0';
        
        token = strtok(line, " \t\r\n");
        if (token == NULL) continue;
        strcpy(command, token);

        // 判斷指令
        if (strcmp(command, "add") == 0 || strcmp(command, "sub") == 0 || 
            strcmp(command, "neg") == 0 || strcmp(command, "eq") == 0 || 
            strcmp(command, "gt") == 0 || strcmp(command, "lt") == 0 || 
            strcmp(command, "and") == 0 || strcmp(command, "or") == 0 || 
            strcmp(command, "not") == 0) {
            writeArithmetic(outputFile, command);
        }
        else if (strcmp(command, "push") == 0 || strcmp(command, "pop") == 0) {
            token = strtok(NULL, " \t\r\n");
            if (token) strcpy(arg1, token);
            token = strtok(NULL, " \t\r\n");
            if (token) arg2 = atoi(token);
            writePushPop(outputFile, command, arg1, arg2, fileNameOnly);
        }
        else if (strcmp(command, "label") == 0 || strcmp(command, "goto") == 0 || 
                 strcmp(command, "if-goto") == 0) {
            token = strtok(NULL, " \t\r\n");
            if (token) strcpy(arg1, token);
            writeBranching(outputFile, command, arg1);
        }
        else if (strcmp(command, "function") == 0) {
            token = strtok(NULL, " \t\r\n");
            if (token) strcpy(arg1, token); // funcName
            token = strtok(NULL, " \t\r\n");
            if (token) arg2 = atoi(token);  // nVars
            writeFunction(outputFile, arg1, arg2);
        }
        else if (strcmp(command, "call") == 0) {
            token = strtok(NULL, " \t\r\n");
            if (token) strcpy(arg1, token); // funcName
            token = strtok(NULL, " \t\r\n");
            if (token) arg2 = atoi(token);  // nArgs
            writeCall(outputFile, arg1, arg2);
        }
        else if (strcmp(command, "return") == 0) {
            writeReturn(outputFile);
        }
    }
    fclose(inputFile);
}

// --- 以下為實作細節 (同先前討論) ---

void writeArithmetic(FILE *fp, char *command) {
    fprintf(fp, "// %s\n", command);
    if (strcmp(command, "add") == 0)      { fprintf(fp, "@SP\nAM=M-1\nD=M\nA=A-1\nM=D+M\n"); }
    else if (strcmp(command, "sub") == 0) { fprintf(fp, "@SP\nAM=M-1\nD=M\nA=A-1\nM=M-D\n"); }
    else if (strcmp(command, "and") == 0) { fprintf(fp, "@SP\nAM=M-1\nD=M\nA=A-1\nM=D&M\n"); }
    else if (strcmp(command, "or") == 0)  { fprintf(fp, "@SP\nAM=M-1\nD=M\nA=A-1\nM=D|M\n"); }
    else if (strcmp(command, "neg") == 0) { fprintf(fp, "@SP\nA=M-1\nM=-M\n"); }
    else if (strcmp(command, "not") == 0) { fprintf(fp, "@SP\nA=M-1\nM=!M\n"); }
    else {
        // eq, gt, lt
        fprintf(fp, "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n");
        fprintf(fp, "@TRUE_%d\n", labelCount);
        if (strcmp(command, "eq") == 0) fprintf(fp, "D;JEQ\n");
        if (strcmp(command, "gt") == 0) fprintf(fp, "D;JGT\n");
        if (strcmp(command, "lt") == 0) fprintf(fp, "D;JLT\n");
        fprintf(fp, "@SP\nA=M-1\nM=0\n"); // False
        fprintf(fp, "@END_%d\n", labelCount);
        fprintf(fp, "0;JMP\n");
        fprintf(fp, "(TRUE_%d)\n", labelCount);
        fprintf(fp, "@SP\nA=M-1\nM=-1\n"); // True
        fprintf(fp, "(END_%d)\n", labelCount);
        labelCount++;
    }
}

void writePushPop(FILE *fp, char *command, char *segment, int index, char *fileName) {
    // 簡化版的 segment 符號對應
    char *segmentSymbol = NULL;
    if (strcmp(segment, "local") == 0) segmentSymbol = "LCL";
    else if (strcmp(segment, "argument") == 0) segmentSymbol = "ARG";
    else if (strcmp(segment, "this") == 0) segmentSymbol = "THIS";
    else if (strcmp(segment, "that") == 0) segmentSymbol = "THAT";

    if (strcmp(command, "push") == 0) {
        if (strcmp(segment, "constant") == 0) {
            fprintf(fp, "@%d\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", index);
        } else if (strcmp(segment, "static") == 0) {
            fprintf(fp, "@%s.%d\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", fileName, index);
        } else if (strcmp(segment, "pointer") == 0) {
            fprintf(fp, "@%s\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", (index==0?"THIS":"THAT"));
        } else if (strcmp(segment, "temp") == 0) {
            fprintf(fp, "@5\nD=A\n@%d\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", index);
        } else if (segmentSymbol != NULL) {
            fprintf(fp, "@%s\nD=M\n@%d\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", segmentSymbol, index);
        }
    } else if (strcmp(command, "pop") == 0) {
        if (strcmp(segment, "static") == 0) {
            fprintf(fp, "@SP\nAM=M-1\nD=M\n@%s.%d\nM=D\n", fileName, index);
        } else if (strcmp(segment, "pointer") == 0) {
            fprintf(fp, "@SP\nAM=M-1\nD=M\n@%s\nM=D\n", (index==0?"THIS":"THAT"));
        } else if (strcmp(segment, "temp") == 0) {
            fprintf(fp, "@5\nD=A\n@%d\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n", index);
        } else if (segmentSymbol != NULL) {
            fprintf(fp, "@%s\nD=M\n@%d\nD=D+A\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n", segmentSymbol, index);
        }
    }
}

void writeBranching(FILE *fp, char *command, char *label) {
    if (strcmp(command, "label") == 0) {
        fprintf(fp, "(%s)\n", label);
    } else if (strcmp(command, "goto") == 0) {
        fprintf(fp, "@%s\n0;JMP\n", label);
    } else if (strcmp(command, "if-goto") == 0) {
        fprintf(fp, "@SP\nAM=M-1\nD=M\n@%s\nD;JNE\n", label);
    }
}

void writeFunction(FILE *fp, char *functionName, int nVars) {
    fprintf(fp, "(%s)\n", functionName);
    for (int i = 0; i < nVars; i++) {
        fprintf(fp, "@SP\nA=M\nM=0\n@SP\nM=M+1\n"); // push 0
    }
}

void writeCall(FILE *fp, char *functionName, int numArgs) {
    char returnLabel[100];
    sprintf(returnLabel, "%s$ret.%d", functionName, callCount++);
    
    // Push returnAddr
    fprintf(fp, "@%s\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", returnLabel);
    // Push LCL, ARG, THIS, THAT
    fprintf(fp, "@LCL\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
    fprintf(fp, "@ARG\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
    fprintf(fp, "@THIS\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
    fprintf(fp, "@THAT\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
    
    // ARG = SP - n - 5
    fprintf(fp, "@SP\nD=M\n@%d\nD=D-A\n@5\nD=D-A\n@ARG\nM=D\n", numArgs);
    // LCL = SP
    fprintf(fp, "@SP\nD=M\n@LCL\nM=D\n");
    // goto function
    fprintf(fp, "@%s\n0;JMP\n", functionName);
    // return label
    fprintf(fp, "(%s)\n", returnLabel);
}

void writeReturn(FILE *fp) {
    fprintf(fp, "@LCL\nD=M\n@R13\nM=D\n"); // FRAME = LCL
    fprintf(fp, "@5\nA=D-A\nD=M\n@R14\nM=D\n"); // RET = *(FRAME-5)
    
    fprintf(fp, "@SP\nAM=M-1\nD=M\n@ARG\nA=M\nM=D\n"); // *ARG = pop()
    
    fprintf(fp, "@ARG\nD=M+1\n@SP\nM=D\n"); // SP = ARG + 1
    
    fprintf(fp, "@R13\nAM=M-1\nD=M\n@THAT\nM=D\n"); // THAT = *(FRAME-1)
    fprintf(fp, "@R13\nAM=M-1\nD=M\n@THIS\nM=D\n"); // THIS = *(FRAME-2)
    fprintf(fp, "@R13\nAM=M-1\nD=M\n@ARG\nM=D\n");  // ARG = *(FRAME-3)
    fprintf(fp, "@R13\nAM=M-1\nD=M\n@LCL\nM=D\n");  // LCL = *(FRAME-4)
    
    fprintf(fp, "@R14\nA=M\n0;JMP\n"); // goto RET
}