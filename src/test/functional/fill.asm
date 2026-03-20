// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

@kbd_prev
M=-1

(MAIN)
// kbd_current = KBD
@KBD
D=M
@kbd_current
M=D

// kbd_changed=kbd_current-kbp_prev
@kbd_prev
D=D-M
@kbd_changed
M=D

// kbd_prev=kbd_current
@kbd_current
D=M
@kbd_prev
M=D

// If KBD has not changed since last loop,
// Go back to MAIN since we have nothing to do
@kbd_changed
D=M
@MAIN
D;JEQ

// Else
@kbd_current
D=M
@CLEAR_SCREEN
D;JEQ
@FILL_SCREEN
0;JMP


(CLEAR_SCREEN)
@i
M=0

(CLEAR_SCREEN_LOOP)
@i
D=M
@8192
D=D-A
@MAIN
D;JGE

// SCREEN[i] = 0
@SCREEN
D=A
@i
A=D+M
M=0

@i
M=M+1

@CLEAR_SCREEN_LOOP
0;JMP


(FILL_SCREEN)
@i
M=0

(FILL_SCREEN_LOOP)
@i
D=M
@8192
D=D-A
@MAIN
D;JEQ

// SCREEN[i] = -1
@SCREEN
D=A
@i
D=D+M
A=D
M=-1

@i
M=M+1

@FILL_SCREEN_LOOP
0;JMP
