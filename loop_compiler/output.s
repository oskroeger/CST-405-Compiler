.data
var_t_9: .word 0
var_t_7: .word 0
var_t_12: .word 0
var_t_14: .word 0
var_t_15: .word 0
var_t_13: .word 0
var_t_17: .word 0
var_t_16: .word 0
var_t_18: .word 0
var_t_19: .word 0
var_t_21: .word 0
var_t_20: .word 0
var_t_23: .word 0
var_t_22: .word 0
var_x: .word 0
var_y: .word 0
var_z: .word 0
var_a: .word 0
var_b: .word 0
var_arr: .word 0, 0, 0, 0, 0
var_arr2: .word 0, 0, 0, 0, 0

.text
.text
.globl main
main:
li $s0, 6
move $t0, $s0
move $s0, $t0
la $s6, var_x
sw $s0, 0($s6)
la $s6, var_x
lw $s0, 0($s6)
move $t1, $s0
li $s0, 4
move $t2, $s0
li $s0, 2
move $t3, $s0
move $s0, $t2
move $s1, $t3
mul $t9, $s0, $s1
move $t4, $t9
move $s0, $t1
move $s1, $t4
add $t9, $s0, $s1
move $t5, $t9
move $s0, $t5
la $s6, var_y
sw $s0, 0($s6)
li $s0, 0
move $t6, $s0
li $s0, 8
move $t7, $s0
li $s0, 3
move $t8, $s0
li $s0, 7
move $t9, $s0
move $s0, $t8
move $s1, $t9
add $t9, $s0, $s1
la $s6, var_t_9
sw $t9, 0($s6)
move $s0, $t7
la $s6, var_t_9
lw $s1, 0($s6)
mul $t9, $s0, $s1
la $s6, var_t_7
sw $t9, 0($s6)
la $s7, var_arr
move $s0, $t6
sll $t0, $s0, 2
add $t0, $s7, $t0
la $s6, var_t_7
lw $s1, 0($s6)
sw $s1, 0($t0)
li $s0, 1
la $s6, var_t_12
sw $s0, 0($s6)
li $s0, 5
la $s6, var_t_14
sw $s0, 0($s6)
la $s6, var_x
lw $s0, 0($s6)
la $s6, var_t_15
sw $s0, 0($s6)
la $s6, var_t_14
lw $s0, 0($s6)
la $s6, var_t_15
lw $s1, 0($s6)
add $t9, $s0, $s1
la $s6, var_t_13
sw $t9, 0($s6)
la $s7, var_arr
la $s6, var_t_12
lw $s0, 0($s6)
sll $t0, $s0, 2
add $t0, $s7, $t0
la $s6, var_t_13
lw $s1, 0($s6)
sw $s1, 0($t0)
li $s0, 0
la $s6, var_t_17
sw $s0, 0($s6)
la $s7, var_arr
la $s6, var_t_17
lw $s0, 0($s6)
sll $t0, $s0, 2
add $t0, $s7, $t0
lw $t9, 0($t0)
la $s6, var_t_16
sw $t9, 0($s6)
la $s6, var_t_16
lw $s0, 0($s6)
la $s6, var_z
sw $s0, 0($s6)
li $s0, 0
la $s6, var_t_18
sw $s0, 0($s6)
li $s0, 4
la $s6, var_t_19
sw $s0, 0($s6)
la $s7, var_arr2
la $s6, var_t_18
lw $s0, 0($s6)
sll $t0, $s0, 2
add $t0, $s7, $t0
la $s6, var_t_19
lw $s1, 0($s6)
sw $s1, 0($t0)
li $s0, 0
la $s6, var_t_21
sw $s0, 0($s6)
la $s7, var_arr2
la $s6, var_t_21
lw $s0, 0($s6)
sll $t0, $s0, 2
add $t0, $s7, $t0
lw $t9, 0($t0)
la $s6, var_t_20
sw $t9, 0($s6)
la $s6, var_t_20
lw $s0, 0($s6)
la $s6, var_a
sw $s0, 0($s6)
li $s0, 1
la $s6, var_t_23
sw $s0, 0($s6)
la $s7, var_arr
la $s6, var_t_23
lw $s0, 0($s6)
sll $t0, $s0, 2
add $t0, $s7, $t0
lw $t9, 0($t0)
la $s6, var_t_22
sw $t9, 0($s6)
la $s6, var_t_22
lw $s0, 0($s6)
la $s6, var_b
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
la $s6, var_a
lw $s0, 0($s6)
move $a0, $s0
li $v0, 1
syscall
li $a0, 10
li $v0, 11
syscall
la $s6, var_b
lw $s0, 0($s6)
move $a0, $s0
li $v0, 1
syscall
li $a0, 10
li $v0, 11
syscall
li $v0, 10
syscall
