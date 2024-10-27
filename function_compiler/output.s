.data
newline: .asciiz "\n"
.text
.globl main
main:
    li $t0, 20
    li $t1, 22
    li.s $f0, 9.700000
    li.s $f1, 22.940001
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
    mov.s $f12, $f0
    li $v0, 2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    mov.s $f12, $f1
    li $v0, 2
    syscall
    li $v0, 4
    la $a0, newline
    syscall
    li $v0, 10
    syscall
