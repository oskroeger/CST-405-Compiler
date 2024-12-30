.data
var_t_13: .word 0
var_t_14: .word 0
var_t_12: .word 0
var_t_16: .word 0
var_t_17: .word 0
var_t_15: .word 0
var_t_18: .word 0
var_t_19: .word 0
var_t_21: .word 0
var_t_22: .word 0
var_t_20: .word 0
var_t_26: .word 0
var_t_24: .word 0
var_t_25: .word 0
var_t_23: .word 0
var_x: .word 0
var_y: .word 0
var_z: .word 0
var_arr: .word 0, 0, 0, 0, 0

.text
.text
.globl main
main:
li $s0, 3
move $t0, $s0
move $s0, $t0
la $s6, var_x
sw $s0, 0($s6)
li $s0, 0
move $t1, $s0
li $s0, 8
move $t2, $s0
li $s0, 3
move $t3, $s0
li $s0, 4
move $t4, $s0
move $s0, $t3
move $s1, $t4
add $t9, $s0, $s1
move $t5, $t9
move $s0, $t2
move $s1, $t5
mul $t9, $s0, $s1
move $t6, $t9
la $s7, var_arr
move $s0, $t1
sll $t0, $s0, 2
add $t0, $s7, $t0
move $s1, $t6
sw $s1, 0($t0)
L_while_start_7:
la $s6, var_x
lw $s0, 0($s6)
move $t7, $s0
li $s0, 6
move $t8, $s0
move $s0, $t7
move $s1, $t8
slt $t9, $s1, $s0
xori $t9, $t9, 1
move $t9, $t9
move $s0, $t9
beq $s0, $zero, L_while_end_8
la $s6, var_x
lw $s0, 0($s6)
la $s6, var_t_13
sw $s0, 0($s6)
li $s0, 1
la $s6, var_t_14
sw $s0, 0($s6)
la $s6, var_t_13
lw $s0, 0($s6)
la $s6, var_t_14
lw $s1, 0($s6)
add $t9, $s0, $s1
la $s6, var_t_12
sw $t9, 0($s6)
la $s6, var_t_12
lw $s0, 0($s6)
la $s6, var_x
sw $s0, 0($s6)
j L_while_start_7
L_while_end_8:
la $s6, var_x
lw $s0, 0($s6)
la $s6, var_t_16
sw $s0, 0($s6)
li $s0, 4
la $s6, var_t_17
sw $s0, 0($s6)
la $s6, var_t_16
lw $s0, 0($s6)
la $s6, var_t_17
lw $s1, 0($s6)
slt $t9, $s1, $s0
la $s6, var_t_15
sw $t9, 0($s6)
la $s6, var_t_15
lw $s0, 0($s6)
bne $s0, $zero, L_then
li $s0, 4
la $s6, var_t_18
sw $s0, 0($s6)
la $s6, var_t_18
lw $s0, 0($s6)
la $s6, var_y
sw $s0, 0($s6)
j L_end
L_then:
li $s0, 5
la $s6, var_t_19
sw $s0, 0($s6)
la $s6, var_t_19
lw $s0, 0($s6)
la $s6, var_y
sw $s0, 0($s6)
la $s6, var_x
lw $s0, 0($s6)
la $s6, var_t_21
sw $s0, 0($s6)
li $s0, 6
la $s6, var_t_22
sw $s0, 0($s6)
la $s6, var_t_21
lw $s0, 0($s6)
la $s6, var_t_22
lw $s1, 0($s6)
add $t9, $s0, $s1
la $s6, var_t_20
sw $t9, 0($s6)
la $s6, var_t_20
lw $s0, 0($s6)
la $s6, var_x
sw $s0, 0($s6)
L_end:
li $s0, 0
la $s6, var_t_26
sw $s0, 0($s6)
la $s7, var_arr
la $s6, var_t_26
lw $s0, 0($s6)
sll $t0, $s0, 2
add $t0, $s7, $t0
lw $t9, 0($t0)
la $s6, var_t_24
sw $t9, 0($s6)
li $s0, 2
la $s6, var_t_25
sw $s0, 0($s6)
la $s6, var_t_24
lw $s0, 0($s6)
la $s6, var_t_25
lw $s1, 0($s6)
sub $t9, $s0, $s1
la $s6, var_t_23
sw $t9, 0($s6)
la $s6, var_t_23
lw $s0, 0($s6)
la $s6, var_z
sw $s0, 0($s6)
la $s6, var_x
lw $s0, 0($s6)
move $a0, $s0
li $v0, 1
syscall
li $a0, 10
li $v0, 11
syscall
la $s6, var_y
lw $s0, 0($s6)
move $a0, $s0
li $v0, 1
syscall
li $a0, 10
li $v0, 11
syscall
la $s6, var_z
lw $s0, 0($s6)
move $a0, $s0
li $v0, 1
syscall
li $a0, 10
li $v0, 11
syscall
li $v0, 10
syscall
