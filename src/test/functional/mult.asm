// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
// The algorithm is based on repetitive addition.


// Read inputs
@R0
D=M
@n0
M=D

@R1
D=M
@n1
M=D

// Init vars
@i
M=0
@ret
M=0
@R2
M=0


(LOOP)
// exit loop if i >= n0
@i
D=M
@n0
D=D-M
@ENDLOOP
D;JGE

// ret = ret + n1
@n1
D=M
@ret
M=D+M

// i = i + 1
@i
M=M+1

@LOOP
0;JMP


(ENDLOOP)
@ret
D=M
@R2
M=D

(END)
@END
0;JMP
