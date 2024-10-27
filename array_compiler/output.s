.data
newline: .asciiz "\n"
.text
.globl main
main:
    li.s $f0, 16.2000
    li $t0, 25
    mov.s $f12, $f0
    li $v0, 2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    move $a0, $t0
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
