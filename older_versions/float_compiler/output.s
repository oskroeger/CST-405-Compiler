.data
var_a: .word 0
var_c: .word 0
var_f_1: .word 0
var_f_2: .word 0
var_f_0: .word 0
var_d: .word 0
var_f_3: .word 0
var_x: .word 0
var_f_5: .word 0
var_f_6: .word 0
var_f_4: .word 0
var_y: .word 0

.text
func_add:
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    move $fp, $sp
    move $s0, $a0
    move $t0, $s0
    li $s0, 2
    move $t1, $s0
    move $s0, $t0
    move $s1, $t1
    add $t9, $s0, $s1
    move $t2, $t9
    move $s0, $t2
    sw $s0, var_c
    lw $s0, var_4.200000
    sw $s0, var_f_1
    lw $s0, var_5.100000
    sw $s0, var_f_2
    lw $s0, var_f_1
    lw $s1, var_f_2
    add $t9, $s0, $s1
    sw $t9, var_f_0
    lw $s0, var_f_0
    sw $s0, var_d
    lw $s0, var_d
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $s0, var_c
    move $t3, $s0
    li $s0, 5
    move $t4, $s0
    move $s0, $t3
    move $s1, $t4
    add $t9, $s0, $s1
    move $t5, $t9
    move $v0, $t5
    j endfunc_add
endfunc_add:
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra
main:
    li $s0, 4
    move $t6, $s0
    move $s0, $t6
    move $a0, $s0
    lw $s0, var_7.300000
    sw $s0, var_f_3
    lw $s0, var_f_3
    move $a1, $s0
    jal func_add
    move $t7, $v0
    move $s0, $t7
    sw $s0, var_x
    lw $s0, var_4.700000
    sw $s0, var_f_5
    lw $s0, var_3.200000
    sw $s0, var_f_6
    lw $s0, var_f_5
    lw $s1, var_f_6
    mul $t9, $s0, $s1
    sw $t9, var_f_4
    lw $s0, var_f_4
    sw $s0, var_y
    lw $s0, var_x
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $s0, var_y
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $v0, 10
    syscall
