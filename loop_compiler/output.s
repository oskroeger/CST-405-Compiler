.data
var_t_11: .word 0
var_t_10: .word 0
var_x: .word 0
var_y: .word 0
var_z: .word 0
var_arr: .word 0, 0, 0, 0, 0

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
li $s0, 3
move $t7, $s0
li $s0, 7
move $t8, $s0
move $s0, $t7
move $s1, $t8
add $t9, $s0, $s1
move $t9, $t9
la $s7, var_arr
lw $s7, 0($s7)
move $s0, $t6
sll $t0, $s0, 2
add $t0, $s7, $t0
move $s1, $t9
sw $s1, 0($t0)
li $s0, 0
la $s6, var_t_11
sw $s0, 0($s6)
la $s7, var_arr
lw $s7, 0($s7)
la $s6, var_t_11
lw $s0, 0($s6)
sll $t0, $s0, 2
add $t0, $s7, $t0
lw $t9, 0($t0)
la $s6, var_t_10
sw $t9, 0($s6)
la $s6, var_t_10
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
