.data
var_t_11: .word 0
var_t_9: .word 0
var_a: .word 0
var_b: .word 0
var_x: .word 0
var_y: .word 0

.text
func_add:
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    move $fp, $sp
    move $s0, $a0
    move $t0, $s0
    move $s0, $a1
    move $t1, $s0
    move $s0, $t0
    move $s1, $t1
    add $t9, $s0, $s1
    move $t2, $t9
    move $v0, $t2
    j endfunc_add
endfunc_add:
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra
func_sub:
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    move $fp, $sp
    lw $s0, var_a
    move $t3, $s0
    lw $s0, var_b
    move $t4, $s0
    move $s0, $t3
    move $s1, $t4
    sub $t9, $s0, $s1
    move $t5, $t9
    move $v0, $t5
    j endfunc_sub
endfunc_sub:
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
    li $s0, 5
    move $t7, $s0
    move $s0, $t7
    move $a1, $s0
    jal func_add
    move $t8, $v0
    move $s0, $t8
    sw $s0, var_x
    li $s0, 9
    move $t9, $s0
    move $s0, $t9
    move $a2, $s0
    li $s0, 4
    sw $s0, var_t_11
    lw $s0, var_t_11
    move $a3, $s0
    jal func_sub
    sw $v0, var_t_9
    lw $s0, var_t_9
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
