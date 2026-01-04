#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定義緩衝區大小
#define MAX_LINE_LENGTH 100
int labelCount = 0;

// 函式宣告
void writeArithmetic(FILE *fp, char *command);
void writePushPop(FILE *fp, char *command, char *segment, int index);

int main() {
    // 1. 開啟檔案 (請確保 SimpleAdd.vm 與執行檔在同目錄)
    FILE *inputFile = fopen("StaticTest.vm", "r");
    FILE *outputFile = fopen("StaticTest.asm", "w");

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
        if (strcmp(command, "add") == 0 || strcmp(command, "sub") == 0 || 
        strcmp(command, "neg") == 0 || strcmp(command, "eq") == 0 || 
        strcmp(command, "gt") == 0 || strcmp(command, "lt") == 0 || 
        strcmp(command, "and") == 0 || strcmp(command, "or") == 0 || 
        strcmp(command, "not") == 0) {
        
        writeArithmetic(outputFile, command);
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
    printf("Translation complete: StaticTest.vm created.\n");

    return 0;
}

// 寫入算術運算 Assembly
void writeArithmetic(FILE *fp, char *command) {
    // 雙元運算 (需要兩個運算元)
    if (strcmp(command, "add") == 0 || strcmp(command, "sub") == 0 || 
        strcmp(command, "and") == 0 || strcmp(command, "or") == 0 ||
        strcmp(command, "eq") == 0 || strcmp(command, "gt") == 0 || strcmp(command, "lt") == 0) {
        
        fprintf(fp, "// %s\n", command);
        fprintf(fp, "@SP\n");
        fprintf(fp, "AM=M-1\n"); // 取出 y
        fprintf(fp, "D=M\n");
        fprintf(fp, "A=A-1\n");  // 指向 x (此時 M 為 x)
        
        if (strcmp(command, "add") == 0) {
            fprintf(fp, "M=D+M\n");
        }
        else if (strcmp(command, "sub") == 0) {
            fprintf(fp, "M=M-D\n"); // x - y
        }
        else if (strcmp(command, "and") == 0) {
            fprintf(fp, "M=D&M\n");
        }
        else if (strcmp(command, "or") == 0) {
            fprintf(fp, "M=D|M\n");
        }
        // --- 比較指令 (大魔王) ---
        else {
            fprintf(fp, "D=M-D\n"); // 計算 x - y，結果放入 D 用於判斷
            
            // 產生兩個唯一標籤
            fprintf(fp, "@TRUE_%d\n", labelCount);
            
            if (strcmp(command, "eq") == 0) fprintf(fp, "D;JEQ\n"); // 若等於0，跳轉
            if (strcmp(command, "gt") == 0) fprintf(fp, "D;JGT\n"); // 若大於0，跳轉
            if (strcmp(command, "lt") == 0) fprintf(fp, "D;JLT\n"); // 若小於0，跳轉
            
            // 若條件不成立 (False)
            fprintf(fp, "@SP\n");
            fprintf(fp, "A=M-1\n");
            fprintf(fp, "M=0\n");   // 設定為 False (0)
            fprintf(fp, "@END_%d\n", labelCount);
            fprintf(fp, "0;JMP\n"); // 跳過 True 的區塊
            
            // 若條件成立 (True)
            fprintf(fp, "(TRUE_%d)\n", labelCount);
            fprintf(fp, "@SP\n");
            fprintf(fp, "A=M-1\n");
            fprintf(fp, "M=-1\n");  // 設定為 True (-1, 所有位元皆為1)
            
            // 結束標籤
            fprintf(fp, "(END_%d)\n", labelCount);
            
            // 計數器 +1，確保下次標籤名稱不同
            labelCount++; 
        }
    }
    // 單元運算 (只需要一個運算元)
    else if (strcmp(command, "neg") == 0) {
        fprintf(fp, "// neg\n");
        fprintf(fp, "@SP\n");
        fprintf(fp, "A=M-1\n");
        fprintf(fp, "M=-M\n");
    }
    else if (strcmp(command, "not") == 0) {
        fprintf(fp, "// not\n");
        fprintf(fp, "@SP\n");
        fprintf(fp, "A=M-1\n");
        fprintf(fp, "M=!M\n");
    }
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
