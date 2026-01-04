// push constant 3030
@3030
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 3040
@3040
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 32
@32
D=A
@SP
A=M
M=D
@SP
M=M+1
// push constant 46
@46
D=A
@SP
A=M
M=D
@SP
M=M+1
// add
@SP
AM=M-1
D=M
A=A-1
M=D+M
// sub
@SP
AM=M-1
D=M
A=A-1
M=M-D
// add
@SP
AM=M-1
D=M
A=A-1
M=D+M
