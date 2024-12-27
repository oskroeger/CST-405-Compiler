.data
var_t_0: .word 0
var_x: .word 0
var_t_4: .word 0
var_t_5: .word 0
var_t_3: .word 0
var_t_2: .word 0
var_t_1: .word 0
var_y: .word 0

.text
.globl main
main:
li $t0, 6
la $s7, var_t_0
sw $t0, 0($s7)
la $s7, var_t_0
lw $t0, 0($s7)
la $s7, var_x
sw $t0, 0($s7)
la $s7, var_x
lw $t0, 0($s7)
la $s7, var_t_2
sw $t0, 0($s7)
li $t0, 4
la $s7, var_t_4
sw $t0, 0($s7)
li $t0, 2
la $s7, var_t_5
sw $t0, 0($s7)
la $s7, var_t_4
lw $t1, 0($s7)
la $s7, var_t_5
lw $t2, 0($s7)
mul $t0, $t1, $t2
la $s7, var_t_3
sw $t0, 0($s7)
la $s7, var_t_2
lw $t1, 0($s7)
la $s7, var_t_3
lw $t2, 0($s7)
add $t0, $t1, $t2
la $s7, var_t_1
sw $t0, 0($s7)
la $s7, var_t_1
lw $t0, 0($s7)
la $s7, var_y
sw $t0, 0($s7)
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
