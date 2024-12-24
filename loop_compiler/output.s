.data
var_t_1: .word 0
var_t_2: .word 0
var_t_0: .word 0
var_a: .word 0
var_t_4: .word 0
var_t_5: .word 0
var_t_3: .word 0
var_b: .word 0
var_t_6: .word 0
var_x: .word 0
var_t_10: .word 0
var_t_11: .word 0
var_t_9: .word 0
var_t_8: .word 0
var_t_7: .word 0
var_y: .word 0

.text
.globl main
main:
la $s7, var_4.300000
lw $t0, 0($s7)
la $s7, var_t_1
sw $t0, 0($s7)
la $s7, var_2.300000
lw $t0, 0($s7)
la $s7, var_t_2
sw $t0, 0($s7)
la $s7, var_t_1
lw $t1, 0($s7)
la $s7, var_t_2
lw $t2, 0($s7)
add $t0, $t1, $t2
la $s7, var_t_0
sw $t0, 0($s7)
la $s7, var_t_0
lw $t0, 0($s7)
la $s7, var_a
sw $t0, 0($s7)
la $s7, var_8.400000
lw $t0, 0($s7)
la $s7, var_t_4
sw $t0, 0($s7)
la $s7, var_2.100000
lw $t0, 0($s7)
la $s7, var_t_5
sw $t0, 0($s7)
la $s7, var_t_4
lw $t1, 0($s7)
la $s7, var_t_5
lw $t2, 0($s7)
mul $t0, $t1, $t2
la $s7, var_t_3
sw $t0, 0($s7)
la $s7, var_t_3
lw $t0, 0($s7)
la $s7, var_b
sw $t0, 0($s7)
li $t0, 6
la $s7, var_t_6
sw $t0, 0($s7)
la $s7, var_t_6
lw $t0, 0($s7)
la $s7, var_x
sw $t0, 0($s7)
la $s7, var_x
lw $t0, 0($s7)
la $s7, var_t_8
sw $t0, 0($s7)
li $t0, 4
la $s7, var_t_10
sw $t0, 0($s7)
li $t0, 2
la $s7, var_t_11
sw $t0, 0($s7)
la $s7, var_t_10
lw $t1, 0($s7)
la $s7, var_t_11
lw $t2, 0($s7)
mul $t0, $t1, $t2
la $s7, var_t_9
sw $t0, 0($s7)
la $s7, var_t_8
lw $t1, 0($s7)
la $s7, var_t_9
lw $t2, 0($s7)
add $t0, $t1, $t2
la $s7, var_t_7
sw $t0, 0($s7)
la $s7, var_t_7
lw $t0, 0($s7)
la $s7, var_y
sw $t0, 0($s7)
la $s7, var_a
lw $t0, 0($s7)
move $a0, $t0
li $v0, 1
syscall
li $a0, '\n'
li $v0, 11
syscall
la $s7, var_b
lw $t0, 0($s7)
move $a0, $t0
li $v0, 1
syscall
li $a0, '\n'
li $v0, 11
syscall
la $s7, var_x
lw $t0, 0($s7)
move $a0, $t0
li $v0, 1
syscall
li $a0, '\n'
li $v0, 11
syscall
la $s7, var_y
lw $t0, 0($s7)
move $a0, $t0
li $v0, 1
syscall
li $a0, '\n'
li $v0, 11
syscall

# exit
li $v0, 10
syscall
