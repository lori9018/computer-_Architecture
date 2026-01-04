#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定義緩衝區大小
#define MAX_LINE_LENGTH 100

// 函式宣告
void writeArithmetic(FILE *fp, char *command);
void writePushPop(FILE *fp, char *command, char *segment, int index);

int main() {
    // 1. 開啟檔案 (請確保 SimpleAdd.vm 與執行檔在同目錄)
    FILE *inputFile = fopen("SimpleAdd.vm", "r");
    FILE *outputFile = fopen("SimpleAdd.asm", "w");

    if (inputFile == NULL || outputFile == NULL) {
        printf("Error: Could not open file.\n");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    char *token;
    char command[20];
    char arg1[20];
    int arg2;

    // 2. 逐行讀取
    while (fgets(line, sizeof(line), inputFile)) {
        // 去除換行符號與處理空白行
        if (line[0] == '\n' || line[0] == '/' || line[0] == '\r') continue;
        
        // 使用 strtok 切割字串 (以空格為分隔)
        // 第一個 token 是指令 (如 "push" 或 "add")
        token = strtok(line, " \r\n"); 
        if (token == NULL) continue; // 防呆
        strcpy(command, token);

        // 3. 判斷指令類型
        if (strcmp(command, "add") == 0) {
            // 處理算術指令
            writeArithmetic(outputFile, "add");
        } 
        else if (strcmp(command, "push") == 0) {
            // 處理 push 指令，需要讀取後面的參數
            token = strtok(NULL, " \r\n"); // 讀取 segment (如 "constant")
            if (token != NULL) strcpy(arg1, token);
            
            token = strtok(NULL, " \r\n"); // 讀取 index (如 "7")
            if (token != NULL) arg2 = atoi(token);

            writePushPop(outputFile, "push", arg1, arg2);
        }
    }

    // 關閉檔案
    fclose(inputFile);
    fclose(outputFile);
    printf("Translation complete: SimpleAdd.asm created.\n");

    return 0;
}

// 寫入算術運算 Assembly
void writeArithmetic(FILE *fp, char *command) {
    if (strcmp(command, "add") == 0) {
        fprintf(fp, "// add\n");
        fprintf(fp, "@SP\n");
        fprintf(fp, "AM=M-1\n"); // SP-- 並讀取 y
        fprintf(fp, "D=M\n");    // D = y
        fprintf(fp, "A=A-1\n");  // 指向 x
        fprintf(fp, "M=D+M\n");  // x = x + y
    }
    // 未來這裡要擴充 sub, neg, eq, gt, lt, and, or, not
}

// 寫入記憶體存取 Assembly
void writePushPop(FILE *fp, char *command, char *segment, int index) {
    if (strcmp(command, "push") == 0) {
        if (strcmp(segment, "constant") == 0) {
            fprintf(fp, "// push constant %d\n", index);
            fprintf(fp, "@%d\n", index); // @x
            fprintf(fp, "D=A\n");        // D = x
            fprintf(fp, "@SP\n");
            fprintf(fp, "A=M\n");        // A = RAM[0]
            fprintf(fp, "M=D\n");        // RAM[SP] = x
            fprintf(fp, "@SP\n");
            fprintf(fp, "M=M+1\n");      // SP++
        }
        // 未來這裡要擴充 local, argument, static 等其他 segment
    }
}