# 期中作業 Project6~12 說明

**註明：原創，複製或修改**

- 第6-10章習題：全部由AI生成（Gemini）
- 第11~12章習題：參考 GitHub 專案以及老師的版本進行理解

### Project 6: Assembler

**習題完成狀態：** AI生成

**理解程度：** 少部分理解

1. 理解核心目標
編寫一個程式（可以用 Python、Java、C++ 或任何熟悉的語言），該程式需要讀取文本文件，並將每一行翻譯成 16-bit 的二進制字串。
2. 使用nand2tetris的工具驗證輸出答案是否相同

**AI對話網址：**

- [gemini對話網址](https://gemini.google.com/share/3721c5b5b8f0)

---


### Project 7: VM I: Stack Arithmetic

**習題完成狀態：** AI生成

**理解程度：** 少部分理解
1. 理解核心目標
程式需要讀取 .vm 檔案，並針對每一行指令生成對應的 Hack Assembly 代碼。 
2. 實作，如何用c語言寫出VM Translator，並執行對應的.vm檔案，並輸出對應的asm

```c
gcc VMTranslator.c -o VMTranslator.exe
```

```c
.\VMTranslator.exe
```

**AI對話網址：**

- [gemini對話網址](https://gemini.google.com/share/d36cfb10d690)

---

### Project 8: VM II: Program Control

**習題完成狀態：** AI生成

**理解程度：** 少部分理解<br>
第一階段：程式流程控制 (Program Flow Control)
主要是翻譯 label、goto 和 if-goto 指令。這讓虛擬機能夠執行迴圈和條件判斷。<br>
第二階段：函式呼叫指令 (Function Calling Commands)
需要實作 function、call 和 return。<br>
第三階段：引導程式 (Bootstrap Code)
為了讓 VM Translator 能處理完整的程式（包含多個 .vm 檔案），需要在輸出的 Assembly 檔案最開頭加入這段程式碼：
1. SP = 256：初始化堆疊指標。
2. Call Sys.init：呼叫系統的入口函式 Sys.init（這會自動處理後續的邏輯）。


```c
gcc VMTranslator.c -o VMTranslator.exe
```

```c
.\VMTranslator.exe .
```

**AI對話網址：**

- [gemini對話網址](https://gemini.google.com/share/7ce46b46ee81)

---

### Project 9: High-Level Language (乒乓球遊戲)

**習題完成狀態：** AI生成

**理解程度：** 少部分理解

1. 理解目標與限制
目標： 寫一個用 Jack 語言編寫的互動式程式（通常是遊戲）。<br>
環境： 程式將在標準的 Hack 平台上運行（256x512 像素螢幕，標準鍵盤）。<br>
限制： Jack 很原始。沒有 Garbage Collection（垃圾回收），運算能力有限（16-bit），沒有進階的除錯器。
2. 核心工作流程 (Workflow)
你需要習慣這個開發循環：<br>
編寫程式碼 (.jack)：使用文字編輯器（VS Code 等）。<br>
編譯 (JackCompiler)：使用課程提供的 tools/JackCompiler 將 .jack 檔轉為 .vm 檔。<br>
測試 (VMEmulator)：載入生成的 .vm 檔（或含有 vm 檔的資料夾），並運行程式。注意： 這裡還不需要用 CPU Emulator，用 VM Emulator 速度才夠快。

```c
cd C:\Users\Arthur\Downloads\nand2tetris\nand2tetris\tools
```
```c
.\JackCompiler.bat C:\arther\computer_arch\_co\final\9\Pong
```
**AI對話網址：**
- [gemini對話網址](https://gemini.google.com/share/babebe7307fe)

---

### Project 10: Compiler I: Parsing

**習題完成狀態：** AI生成

**理解程度：** 少部分理解

1. 理解核心目標
目標是編寫一個程式（可以用 Java, Python, C++），它接收 .jack 檔案，並輸出對應的 .xml 檔案。<br>
這個過程分為兩個主要模組：
1. Tokenizer (詞法分析器): 把原始程式碼拆解成一個個有意義的 "Token"（單詞），並丟棄註解和空白。
2. Compilation Engine (編譯引擎): 根據 Jack 的語法規則，將這些 Token 組織成樹狀結構（Parse Tree），並輸出 XML。

```c
g++ JackAnalyzer.cpp CompilationEngine.cpp JackTokenizer.cpp -o JackAnalyzer.exe
./JackAnalyzer.exe ../ArrayTest/Main.jack
```
使用TextComparer
```c
cd C:\Users\Arthur\Downloads\nand2tetris\nand2tetris\tools
```
```c
.\TextComparer.bat C:\arther\computer_arch\_co\final\10\theonewanttotest
```
**AI對話網址：**
- [gemini對話網址](https://gemini.google.com/share/5b818714efcc)

---

### Project 11: Compiler II: Code Generation

**習題完成狀態：** 未完成，僅閱讀理解老師的範例

**理解程度：** 少部分理解

1. 理解核心目標
目標是編寫一個程式（Compiler），將 Jack 語言 (.jack) 翻譯成 VM 代碼 (.vm)。 需要將第 10 章的 JackAnalyzer 進行改造。不再需要輸出 XML，而是要輸出 VM 指令。

**AI對話網址：**
- [老師的範例](https://github.com/ccc114a/cpu2os/tree/master/_nand2tetris/11)

### Project 12: Operating System

**習題完成狀態：** 未完成，僅閱讀理解老師的範例

**理解程度：** 少部分理解

1. 核心觀念與準備
在這個階段，已經有了 CPU、記憶體和編譯器。任務是填補硬體與應用程式軟體之間的空白。要實作 8 個 Jack 類別檔案（.jack）。
使用 Jack 語言實作 8 個核心類別（Class），每個類別負責不同的系統服務 ：

1. Math.jack：處理數學運算（如乘除法、平方根等） 。
2. String.jack：處理字串處理與轉換 。
3. Array.jack：管理陣列的建立 。
4. Memory.jack：負責記憶體管理（分配與釋放，如 alloc 和 deAlloc） 。
5. Screen.jack：負責繪圖功能（如繪製直線、矩形、圓形） 。
6. Output.jack：處理文字輸出與螢幕字元顯示 。
7. Keyboard.jack：處理鍵盤輸入與讀取按鍵訊息 。
8. Sys.jack：系統服務（如程式初始化 init、暫停執行 wait 以及終止程式 halt） 。


**AI對話網址：**
- [老師的範例](https://github.com/ccc114a/cpu2os/tree/master/_nand2tetris/12)