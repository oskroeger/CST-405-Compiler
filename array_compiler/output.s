.data
newline: .asciiz "\n"
.text
.globl main
main:
    li $t0, 12
    move $a0, $t0
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
