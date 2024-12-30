.data
var_t_11: .word 0
var_t_9: .word 0
var_t_13: .word 0
var_t_14: .word 0
var_t_12: .word 0
var_t_15: .word 0
var_t_16: .word 0
var_t_17: .word 0
var_t_21: .word 0
var_t_22: .word 0
var_t_20: .word 0
var_t_24: .word 0
var_t_25: .word 0
var_t_23: .word 0
var_t_26: .word 0
var_t_27: .word 0
var_a: .word 0
var_b: .word 0
var_x: .word 0
var_y: .word 0
var_z: .word 0
var_d: .word 0
var_arr: .word 0, 0, 0, 0, 0

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
    move $s0, $a0
    move $t3, $s0
    move $s0, $a1
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
    move $a0, $s0
    li $s0, 4
    sw $s0, var_t_11
    lw $s0, var_t_11
    move $a1, $s0
    jal func_sub
    sw $v0, var_t_9
    lw $s0, var_t_9
    sw $s0, var_y
    lw $s0, var_x
    sw $s0, var_t_13
    li $s0, 4
    sw $s0, var_t_14
    lw $s0, var_t_13
    lw $s1, var_t_14
    slt $t9, $s1, $s0
    sw $t9, var_t_12
    lw $s0, var_t_12
    bne $s0, $zero, L_then
    li $s0, 8
    sw $s0, var_t_15
    lw $s0, var_t_15
    sw $s0, var_z
    j L_end
L_then:
    li $s0, 7
    sw $s0, var_t_16
    lw $s0, var_t_16
    sw $s0, var_z
L_end:
    li $s0, 1
    sw $s0, var_t_17
    lw $s0, var_t_17
    sw $s0, var_d
L_while_start_18:
    lw $s0, var_d
    sw $s0, var_t_21
    li $s0, 2
    sw $s0, var_t_22
    lw $s0, var_t_21
    lw $s1, var_t_22
    slt $t9, $s1, $s0
    xori $t9, $t9, 1
    sw $t9, var_t_20
    lw $s0, var_t_20
    beq $s0, $zero, L_while_end_19
    lw $s0, var_d
    sw $s0, var_t_24
    li $s0, 1
    sw $s0, var_t_25
    lw $s0, var_t_24
    lw $s1, var_t_25
    add $t9, $s0, $s1
    sw $t9, var_t_23
    lw $s0, var_t_23
    sw $s0, var_d
    j L_while_start_18
L_while_end_19:
    li $s0, 0
    sw $s0, var_t_26
    lw $s0, var_d
    sw $s0, var_t_27
    la $s7, var_arr
    lw $s0, var_t_26
    sll $t0, $s0, 2
    add $t0, $s7, $t0
    lw $s1, var_t_27
    sw $s1, 0($t0)
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
    lw $s0, var_z
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $s0, var_d
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $v0, 10
    syscall
