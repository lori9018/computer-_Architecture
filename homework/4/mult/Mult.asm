// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
// The algorithm is based on repetitive addition.

@R2     // Select register R2
    M=0     // Initialize R2 (result) to 0

    @R1     // Select register R1
    D=M     // Load value of R1 into D
    @i      // Select variable i (counter)
    M=D     // i = R1

(LOOP)
    @i      // Select i
    D=M     // Load current value of i into D
    @END    // Select END label
    D;JEQ   // If i == 0, jump to END

    @R0     // Select register R0
    D=M     // Load value of R0 into D
    @R2     // Select R2
    M=D+M   // R2 = R2 + R0 (Add R0 to current total)

    @i      // Select i
    M=M-1   // Decrement counter i

    @LOOP   // Select LOOP label
    0;JMP   // Jump back to start of loop

(END)
    @END    // Select END label
    0;JMP   // Infinite loop to terminate program safely
