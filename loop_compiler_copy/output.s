.data
var_t_8: .word 0
var_t_12: .word 0
var_t_13: .word 0
var_t_11: .word 0
var_t_15: .word 0
var_t_16: .word 0
var_t_14: .word 0
var_t_18: .word 0
var_t_19: .word 0
var_t_17: .word 0
var_t_20: .word 0
var_t_21: .word 0
var_t_22: .word 0
var_t_26: .word 0
var_t_27: .word 0
var_t_25: .word 0
var_t_29: .word 0
var_t_30: .word 0
var_t_28: .word 0
var_t_31: .word 0
var_t_32: .word 0
var_t_33: .word 0
var_t_35: .word 0
var_t_37: .word 0
var_t_38: .word 0
var_t_36: .word 0
var_t_34: .word 0
var_t_40: .word 0
var_t_39: .word 0
var_a: .word 0
var_b: .word 0
var_p: .word 0
var_q: .word 0
var_pq: .word 0
var_x: .word 0
var_y: .word 0
var_z: .word 0
var_d: .word 0
var_e: .word 0
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
    lw $s0, var_1.100000
    move $t6, $s0
    move $s0, $t6
    sw $s0, var_p
    lw $s0, var_2.300000
    move $t7, $s0
    move $s0, $t7
    sw $s0, var_q
    lw $s0, var_p
    move $t8, $s0
    lw $s0, var_q
    move $t9, $s0
    # Unhandled TAC: f+ t_9 t_10 t_8
    lw $s0, var_t_8
    sw $s0, var_pq
    li $s0, 4
    sw $s0, var_t_12
    lw $s0, var_t_12
    move $a0, $s0
    li $s0, 5
    sw $s0, var_t_13
    lw $s0, var_t_13
    move $a1, $s0
    jal func_add
    sw $v0, var_t_11
    lw $s0, var_t_11
    sw $s0, var_x
    li $s0, 9
    sw $s0, var_t_15
    lw $s0, var_t_15
    move $a0, $s0
    li $s0, 4
    sw $s0, var_t_16
    lw $s0, var_t_16
    move $a1, $s0
    jal func_sub
    sw $v0, var_t_14
    lw $s0, var_t_14
    sw $s0, var_y
    lw $s0, var_x
    sw $s0, var_t_18
    li $s0, 4
    sw $s0, var_t_19
    lw $s0, var_t_18
    lw $s1, var_t_19
    slt $t9, $s1, $s0
    sw $t9, var_t_17
    lw $s0, var_t_17
    bne $s0, $zero, L_then
    li $s0, 8
    sw $s0, var_t_20
    lw $s0, var_t_20
    sw $s0, var_z
    j L_end
L_then:
    li $s0, 7
    sw $s0, var_t_21
    lw $s0, var_t_21
    sw $s0, var_z
L_end:
    li $s0, 1
    sw $s0, var_t_22
    lw $s0, var_t_22
    sw $s0, var_d
L_while_start_23:
    lw $s0, var_d
    sw $s0, var_t_26
    li $s0, 2
    sw $s0, var_t_27
    lw $s0, var_t_26
    lw $s1, var_t_27
    slt $t9, $s1, $s0
    xori $t9, $t9, 1
    sw $t9, var_t_25
    lw $s0, var_t_25
    beq $s0, $zero, L_while_end_24
    lw $s0, var_d
    sw $s0, var_t_29
    li $s0, 1
    sw $s0, var_t_30
    lw $s0, var_t_29
    lw $s1, var_t_30
    add $t9, $s0, $s1
    sw $t9, var_t_28
    lw $s0, var_t_28
    sw $s0, var_d
    j L_while_start_23
L_while_end_24:
    li $s0, 0
    sw $s0, var_t_31
    lw $s0, var_d
    sw $s0, var_t_32
    la $s7, var_arr
    lw $s0, var_t_31
    sll $t0, $s0, 2
    add $t0, $s7, $t0
    lw $s1, var_t_32
    sw $s1, 0($t0)
    li $s0, 1
    sw $s0, var_t_33
    li $s0, 4
    sw $s0, var_t_35
    li $s0, 9
    sw $s0, var_t_37
    li $s0, 2
    sw $s0, var_t_38
    lw $s0, var_t_37
    lw $s1, var_t_38
    add $t9, $s0, $s1
    sw $t9, var_t_36
    lw $s0, var_t_35
    lw $s1, var_t_36
    mul $t9, $s0, $s1
    sw $t9, var_t_34
    la $s7, var_arr
    lw $s0, var_t_33
    sll $t0, $s0, 2
    add $t0, $s7, $t0
    lw $s1, var_t_34
    sw $s1, 0($t0)
    li $s0, 1
    sw $s0, var_t_40
    la $s7, var_arr
    lw $s0, var_t_40
    sll $t0, $s0, 2
    add $t0, $s7, $t0
    lw $t9, 0($t0)
    sw $t9, var_t_39
    lw $s0, var_t_39
    sw $s0, var_e
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
    lw $s0, var_e
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $s0, var_pq
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $v0, 10
    syscall
