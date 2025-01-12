.data
newline: .asciiz "\n"
.text
.globl main
main:
    li $t0, 4
    move $a0, $t0
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $t1, 1
    move $a0, $t1
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li.s $f0, 16.2000
    li $t2, 25
    mov.s $f12, $f0
    li $v0, 2
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
    li $t3, 5
    move $a0, $t3
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
