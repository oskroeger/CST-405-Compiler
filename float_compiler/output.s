.data
newline: .asciiz "\n"
.text
.globl main
main:
    li $t0, 4
    move $t0, $3.000000
    move $a0, $t0
    li $v0, 1
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