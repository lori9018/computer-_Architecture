// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

(LOOP)
    @KBD        // Load Keyboard memory address (24576)
    D=M         // Read the value at that address
    @ON
    D;JGT       // If value > 0 (key is pressed), jump to (ON)
    
    @OFF
    0;JMP       // Else (value == 0), jump to (OFF)

(ON)
    @-1         // 1111111111111111 in binary (Black)
    D=A         // Store -1 (as a value, though actually A holds 65535, we use it as data)
                // Note: In Hack ASM 'M=-1' or 'D=-1' is safer, but let's use a variable approach
    D=-1        // Explicitly set D to all 1s
    @color
    M=D         // Save 'black' to the color variable
    @DRAW
    0;JMP       // Jump to the drawing logic

(OFF)
    @0          // 0000000000000000 in binary (White)
    D=A
    @color
    M=D         // Save 'white' to the color variable
    @DRAW
    0;JMP       // Jump to the drawing logic

(DRAW)
    @SCREEN     // Load Screen memory start address (16384)
    D=A
    @ptr        // Initialize a pointer variable
    M=D

(NEXT_PIXEL)
    @ptr        // Get current pixel address
    D=M
    @24576      // Load Keyboard address (which is Screen End + 1)
    D=D-A       // Calculate (Current Address - End Address)
    @LOOP
    D;JEQ       // If result is 0 (we reached the end), go back to main LOOP

    @color      // Load the chosen color (0 or -1)
    D=M
    @ptr        // Load the address of the pixel pointer
    A=M         // Set A-register to the address stored in ptr
    M=D         // Paint the pixels at that address

    @ptr
    M=M+1       // Increment the pixel pointer to the next word
    @NEXT_PIXEL
    0;JMP       // Continue the drawing loop
