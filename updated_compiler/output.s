.data
newline: .asciiz "\n"
.text
.globl main
main:
    li $t0, 3
    li $t1, 7
    li $t2, 17
    li $t3, 44
    move $a0, $t0
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    move $a0, $t1
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    move $a0, $t2
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    move $a0, $t3
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall