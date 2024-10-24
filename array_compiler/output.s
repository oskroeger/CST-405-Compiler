.data
newline: .asciiz "\n"
.text
.globl main
main:
    li $t0, 30
    li.s $f0, 9.000000
    move $a0, $t0
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    mov.s $f12, $f0
    li $v0, 2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
