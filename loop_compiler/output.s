.data
var_x: .word 0
var_y: .word 0

.text
.text
.globl main
main:
li $s0, 3
move $t0, $s0
move $s0, $t0
la $s6, var_x
sw $s0, 0($s6)
la $s6, var_x
lw $s0, 0($s6)
move $t1, $s0
li $s0, 4
move $t2, $s0
move $s0, $t1
move $s1, $t2
beq $s0, $s1, L_eq_true_0
li $t9, 0
j L_eq_end_1
L_eq_true_0:
li $t9, 1
L_eq_end_1:
move $t3, $t9
move $s0, $t3
bne $s0, $zero, L_then
li $s0, 7
move $t4, $s0
move $s0, $t4
la $s6, var_y
sw $s0, 0($s6)
j L_end
L_then:
li $s0, 5
move $t5, $s0
move $s0, $t5
la $s6, var_y
sw $s0, 0($s6)
L_end:
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
li $v0, 10
syscall
