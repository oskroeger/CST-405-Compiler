.data
var_x: .word 0
var_y: .word 0

.text
.globl main
main:
li $t0, 6
la $t9, var_x
sw $t0, 0($t9)
# Unhandled TAC: load x  t2
li $t4, 4
li $t5, 2
move $t1, $t4
move $t2, $t5
mul $t3, $t1, $t2
move $t1, $t2
move $t2, $t3
add $t1, $t1, $t2
la $t9, var_y
sw $t1, 0($t9)
la $t9, var_x
lw $t0, 0($t9)
move $a0, $t0
li $v0, 1
syscall
li $a0, '\n'
li $v0, 11
syscall
la $t9, var_y
lw $t0, 0($t9)
move $a0, $t0
li $v0, 1
syscall
li $a0, '\n'
li $v0, 11
syscall

# exit
li $v0, 10
syscall
