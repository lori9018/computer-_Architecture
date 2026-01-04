# 期中作業 Project6~12 說明

## 著作狀態聲明

**註明：原創，複製或修改**

- 第6-11章習題：全部由AI生成（Gemini）
- 第12章習題：參考 GitHub 專案以及老師的版本進行理解

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

**理解程度：** 少部分理解
第一階段：程式流程控制 (Program Flow Control)
主要是翻譯 label、goto 和 if-goto 指令。這讓你的虛擬機能夠執行迴圈和條件判斷。
第二階段：函式呼叫指令 (Function Calling Commands)
需要實作 function、call 和 return。
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

