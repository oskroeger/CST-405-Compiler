.data
x: .word 0
y: .word 0

.text
.globl main
    li $t0, 4
    sw $t0, t0
    lw $t0, t0
    sw $t0, x
    li $t0, 3
    sw $t0, t2
    li $t0, 5
    sw $t0, t4
    li $t0, 2
    sw $t0, t5
    lw $t0, t4
    lw $t1, t5
    add $t2, $t0, $t1
    sw $t2, t3
    lw $t0, t2
    lw $t1, t3
    mul $t2, $t0, $t1
    sw $t2, t1
    lw $t0, t1
    sw $t0, y
    lw $a0, x
    li $v0, 1
    syscall
    lw $a0, y
    li $v0, 1
    syscall
    li $v0, 10
    syscall
