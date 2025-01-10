.data
var_x: .word 0
var_y: .word 0

.text
.globl main
main:
L_func_add:
# function prologue
addi $sp, $sp, -8
sw $ra, 4($sp)
sw $fp, 0($sp)
move $fp, $sp
lw $t1, 8($fp)
lw $t2, 12($fp)
add $t0, $t1, $t2
move $v0, $t0
move $sp, $fp
lw $fp, 0($sp)
lw $ra, 4($sp)
addi $sp, $sp, 8
jr $ra
L_endfunc_add:
li $t4, 5
addi $sp, $sp, -4
sw $t4, 0($sp)
li $t5, 6
addi $sp, $sp, -4
sw $t5, 0($sp)
jal L_func_add
addi $sp, $sp, 8
move $t3, $v0
la $t9, var_x
sw $t3, 0($t9)
la $t9, var_x
lw $t7, 0($t9)
li $t8, 11
seq $t6, $t7, $t8
bne $t6, $zero, L_L_then
li $t9, 3
la $t9, var_y
sw $t9, 0($t9)
j L_L_end
L_L_then:
li $t0, 4
la $t9, var_y
sw $t0, 0($t9)
L_L_end:
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

end:
li $v0, 10
syscall
