#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
int labelCount = 0;

// 函式宣告
void writeArithmetic(FILE *fp, char *command);
void writePushPop(FILE *fp, char *command, char *segment, int index);
void writeBranching(FILE *fp, char *command, char *label); // 新增：處理流程控制
void writeFunction(FILE *fp, char *functionName, int nVars);
void writeReturn(FILE *fp);

int main() {
    // 請確認你的輸入檔名是否正確，BasicLoop 通常在 BasicLoop.vm
    FILE *inputFile = fopen("SimpleFunction.vm", "r");
    FILE *outputFile = fopen("SimpleFunction.asm", "w");

    if (inputFile == NULL || outputFile == NULL) {
        printf("Error: Could not open file.\n");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    char *token;
    char command[20];
    char arg1[50]; // 加大一點以容納較長的標籤名
    int arg2;

   while (fgets(line, sizeof(line), inputFile)) {
        // 處理註解與空行
        if (line[0] == '\n' || line[0] == '/' || line[0] == '\r') continue;
        
        // 【修正點】：加入 \t 以處理 Tab 鍵縮排
        token = strtok(line, " \t\r\n"); 
        if (token == NULL) continue;
        strcpy(command, token);

        // 1. 算術運算
        if (strcmp(command, "add") == 0 || strcmp(command, "sub") == 0 || 
            strcmp(command, "neg") == 0 || strcmp(command, "eq") == 0 || 
            strcmp(command, "gt") == 0 || strcmp(command, "lt") == 0 || 
            strcmp(command, "and") == 0 || strcmp(command, "or") == 0 || 
            strcmp(command, "not") == 0) {
            writeArithmetic(outputFile, command);
        }
        // 2. 記憶體存取 (push / pop)
        else if (strcmp(command, "push") == 0 || strcmp(command, "pop") == 0) {
            // 【修正點】：讀取參數時也要用同樣的分隔符
            token = strtok(NULL, " \t\r\n"); 
            if (token != NULL) strcpy(arg1, token);
            
            token = strtok(NULL, " \t\r\n"); 
            if (token != NULL) arg2 = atoi(token);

            writePushPop(outputFile, command, arg1, arg2);
        }
        // 3. 流程控制
        else if (strcmp(command, "label") == 0 || strcmp(command, "goto") == 0 || 
                 strcmp(command, "if-goto") == 0) {
            token = strtok(NULL, " \t\r\n");
            if (token != NULL) strcpy(arg1, token);
            
            writeBranching(outputFile, command, arg1);
        }
        else if (strcmp(command, "function") == 0) {
            token = strtok(NULL, " \t\r\n");
            char functionName[100];
            strcpy(functionName, token);

            token = strtok(NULL, " \t\r\n");
            int nVars = atoi(token);

            writeFunction(outputFile, functionName, nVars);
        }
        else if (strcmp(command, "return") == 0) {
            writeReturn(outputFile);
        }
        // call 指令我們先留個位置，下一關再實作
        else if (strcmp(command, "call") == 0) {
            // 暫時留空或印個註解
        }
    }

    fclose(inputFile);
    fclose(outputFile);
    printf("Translation complete.\n");

    return 0;
}

void writeArithmetic(FILE *fp, char *command) {
    // ... (這部分保持原本的邏輯不變，為了節省版面省略，請保留你原本的程式碼) ...
    // 請將你原本的 writeArithmetic 函式內容貼回這裡
    // 為了讓 BasicLoop 能跑，這裡簡單列出 add/sub，請務必保留你完整的 eq/gt/lt 實作
    
    fprintf(fp, "// %s\n", command);
    if (strcmp(command, "add") == 0 || strcmp(command, "sub") == 0) {
        fprintf(fp, "@SP\nAM=M-1\nD=M\nA=A-1\n");
        if (strcmp(command, "add") == 0) fprintf(fp, "M=D+M\n");
        else fprintf(fp, "M=M-D\n");
    } else if (strcmp(command, "neg") == 0) {
        fprintf(fp, "@SP\nA=M-1\nM=-M\n");
    } 
    // 注意：請保留你原本完整的 eq, gt, lt, and, or, not 實作
}

// 處理 label, goto, if-goto
void writeBranching(FILE *fp, char *command, char *label) {
    fprintf(fp, "// %s %s\n", command, label);

    if (strcmp(command, "label") == 0) {
        fprintf(fp, "(%s)\n", label);
    } 
    else if (strcmp(command, "goto") == 0) {
        fprintf(fp, "@%s\n", label);
        fprintf(fp, "0;JMP\n");
    } 
    else if (strcmp(command, "if-goto") == 0) {
        // 邏輯：pop 出來的值若 != 0，則跳轉
        fprintf(fp, "@SP\n");
        fprintf(fp, "AM=M-1\n"); // 堆疊指針減1並讀取
        fprintf(fp, "D=M\n");    // D = stack.pop()
        fprintf(fp, "@%s\n", label);
        fprintf(fp, "D;JNE\n");  // 若 D != 0 (True), 跳轉
    }
}

// 擴充後的 writePushPop
void writePushPop(FILE *fp, char *command, char *segment, int index) {
    fprintf(fp, "// %s %s %d\n", command, segment, index);

    // 判斷 Segment 對應的 Base Address Symbol
    char *segmentSymbol = NULL;
    if (strcmp(segment, "local") == 0) segmentSymbol = "LCL";
    else if (strcmp(segment, "argument") == 0) segmentSymbol = "ARG";
    else if (strcmp(segment, "this") == 0) segmentSymbol = "THIS";
    else if (strcmp(segment, "that") == 0) segmentSymbol = "THAT";
    // temp, pointer, static 需要特殊處理，BasicLoop 主要用到上面四個

    if (strcmp(command, "push") == 0) {
        if (strcmp(segment, "constant") == 0) {
            fprintf(fp, "@%d\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n", index);
        } else if (segmentSymbol != NULL) {
            // Push Local/Argument/This/That
            // addr = LCL + index, *SP = *addr, SP++
            fprintf(fp, "@%s\n", segmentSymbol);
            fprintf(fp, "D=M\n");      // D = Base Address
            fprintf(fp, "@%d\n", index);
            fprintf(fp, "A=D+A\n");    // A = Base + index
            fprintf(fp, "D=M\n");      // D = *(Base + index)
            fprintf(fp, "@SP\n");
            fprintf(fp, "A=M\n");
            fprintf(fp, "M=D\n");      // Push to stack
            fprintf(fp, "@SP\n");
            fprintf(fp, "M=M+1\n");
        }
    } 
    else if (strcmp(command, "pop") == 0) {
        if (segmentSymbol != NULL) {
            // Pop Local/Argument/This/That
            // addr = LCL + index, SP--, *addr = *SP
            // 使用 R13 暫存目標記憶體位址
            
            fprintf(fp, "@%s\n", segmentSymbol);
            fprintf(fp, "D=M\n");
            fprintf(fp, "@%d\n", index);
            fprintf(fp, "D=D+A\n");    // 計算目標位址
            fprintf(fp, "@R13\n");
            fprintf(fp, "M=D\n");      // 將位址存入 R13

            fprintf(fp, "@SP\n");
            fprintf(fp, "AM=M-1\n");   // SP--, A=SP
            fprintf(fp, "D=M\n");      // 取出數值放入 D

            fprintf(fp, "@R13\n");
            fprintf(fp, "A=M\n");      // A = 目標位址
            fprintf(fp, "M=D\n");      // 存入數值
        }
    }
}

void writeFunction(FILE *fp, char *functionName, int nVars) {
    fprintf(fp, "// function %s %d\n", functionName, nVars);
    
    // 1. 寫入函式進入點標籤
    fprintf(fp, "(%s)\n", functionName);

    // 2. 初始化區域變數 (push constant 0, nVars 次)
    // 這是為了確保 LCL 區段有乾淨的記憶體空間
    for (int i = 0; i < nVars; i++) {
        fprintf(fp, "@0\n");
        fprintf(fp, "D=A\n");
        fprintf(fp, "@SP\n");
        fprintf(fp, "A=M\n");
        fprintf(fp, "M=D\n"); // push 0
        fprintf(fp, "@SP\n");
        fprintf(fp, "M=M+1\n"); // SP++
    }
}

void writeReturn(FILE *fp) {
    fprintf(fp, "// return\n");

    // 1. FRAME = LCL (將 LCL 的值暫存到 R13)
    fprintf(fp, "@LCL\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@R13\n");
    fprintf(fp, "M=D\n");

    // 2. RET = *(FRAME - 5) (取得返回位址，暫存到 R14)
    // 雖然可以直接用 R13-5，但Hack組語比較囉唆，我們先算好
    fprintf(fp, "@5\n");
    fprintf(fp, "A=D-A\n"); // A = FRAME - 5
    fprintf(fp, "D=M\n");   // D = *(FRAME - 5)
    fprintf(fp, "@R14\n");
    fprintf(fp, "M=D\n");   // R14 存入返回位址

    // 3. *ARG = pop() (將回傳值放到 ARG 指向的位置，這是給呼叫者看的)
    fprintf(fp, "@SP\n");
    fprintf(fp, "AM=M-1\n"); // SP--
    fprintf(fp, "D=M\n");    // 取出 return value
    fprintf(fp, "@ARG\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "M=D\n");    // 寫入 ARG[0]

    // 4. SP = ARG + 1 (恢復 SP，回收被呼叫者的堆疊空間)
    fprintf(fp, "@ARG\n");
    fprintf(fp, "D=M+1\n");
    fprintf(fp, "@SP\n");
    fprintf(fp, "M=D\n");

    // 5. 恢復 Caller 的 Segment 指標 (THAT, THIS, ARG, LCL)
    // 注意順序：我們是從 FRAME (原本的 LCL) 往回推
    
    // THAT = *(FRAME - 1)
    fprintf(fp, "@R13\n");
    fprintf(fp, "AM=M-1\n"); // R13 減 1 變成 FRAME-1
    fprintf(fp, "D=M\n");    // 取出舊的 THAT
    fprintf(fp, "@THAT\n");
    fprintf(fp, "M=D\n");

    // THIS = *(FRAME - 2)
    fprintf(fp, "@R13\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@THIS\n");
    fprintf(fp, "M=D\n");

    // ARG = *(FRAME - 3)
    fprintf(fp, "@R13\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@ARG\n");
    fprintf(fp, "M=D\n");

    // LCL = *(FRAME - 4)
    fprintf(fp, "@R13\n");
    fprintf(fp, "AM=M-1\n");
    fprintf(fp, "D=M\n");
    fprintf(fp, "@LCL\n");
    fprintf(fp, "M=D\n");

    // 6. goto RET (跳轉回 R14 儲存的返回位址)
    fprintf(fp, "@R14\n");
    fprintf(fp, "A=M\n");
    fprintf(fp, "0;JMP\n");
}