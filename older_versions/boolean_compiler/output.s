.data
x: .word 0
z: .word 0
x: .word 0
x: .float 0.0
y: .word 0
arr2: .word 0, 0
arr: .float 0.0, 0.0
.text
.globl main
main:
func_one:
addi $sp, $sp, -8
sw $ra, 4($sp)
sw $fp, 0($sp)
move $fp, $sp
li $t0, 5
sw $t0, t0
sw $t0, x
lw $t0, x
move $a0, $t0
li $v0, 1
syscall
li $a0, '\n'
li $v0, 11
syscall
move $sp, $fp
lw $fp, 0($sp)
lw $ra, 4($sp)
addi $sp, $sp, 8
jr $ra
func_two:
addi $sp, $sp, -8
sw $ra, 4($sp)
sw $fp, 0($sp)
move $fp, $sp
li $t0, 0
sw $t0, t1
lwc1 $f0, FLT0
sw $f0, t3
lwc1 $f0, FLT1
sw $f0, t4
add $t2, $t3, $t4
sll $t2, $t1, 2
la $t3, arr
add $t3, $t3, $t2
sw $t2, 0($t3)
li $t0, 1
sw $t0, t5
lwc1 $f0, FLT2
sw $f0, t7
lwc1 $f0, FLT3
sw $f0, t8
mul $t6, $t7, $t8
sll $t2, $t5, 2
la $t3, arr
add $t3, $t3, $t2
sw $t6, 0($t3)
li $t0, 0
sw $t0, t12
sll $t2, $t2, 2
la $t3, arr
add $t3, $t3, $t2
lw $t0, 0($t3)
li $t0, 1
sw $t0, t13
sll $t2, $t3, 2
la $t3, arr
add $t3, $t3, $t2
lw $t0, 0($t3)
add $t9, $t0, $t1
sw $t9, x
li $t0, 0
sw $t0, t14
li $t0, 1
sw $t0, t15
sll $t2, $t4, 2
la $t3, arr2
add $t3, $t3, $t2
sw $t5, 0($t3)
li $t0, 1
sw $t0, t16
li $t0, 4
sw $t0, t17
sll $t2, $t6, 2
la $t3, arr2
add $t3, $t3, $t2
sw $t7, 0($t3)
li $t0, 5
sw $t0, t19
li $t0, 0
sw $t0, t23
sll $t2, $t3, 2
la $t3, arr2
add $t3, $t3, $t2
lw $t0, 0($t3)
li $t0, 1
sw $t0, t24
sll $t2, $t4, 2
la $t3, arr2
add $t3, $t3, $t2
lw $t0, 0($t3)
add $t0, $t1, $t2
mul $t8, $t9, $t0
sw $t8, y
lw $t0, x
move $a0, $t0
li $v0, 1
syscall
li $a0, '\n'
li $v0, 11
syscall
lw $t0, y
move $a0, $t0
li $v0, 1
syscall
li $a0, '\n'
li $v0, 11
syscall
move $sp, $fp
lw $fp, 0($sp)
lw $ra, 4($sp)
addi $sp, $sp, 8
jr $ra
li $t0, 4
sw $t0, t25
sw $t5, x
lw $t0, x
move $a0, $t0
li $v0, 1
syscall
li $a0, '\n'
li $v0, 11
syscall
sll $t2, (null), 2
la $t3, x
add $t3, $t3, $t2
lw $t0, 0($t3)
li $t0, 3
sw $t0, t28
seq $t6, $t7, $t8
bne $t6, $zero, L_then
li $t0, 6
sw $t0, t29
sw $t9, z
j L_end
L_then:
li $t0, 5
sw $t0, t30
sw $t0, z
L_end:
lw $t0, z
move $a0, $t0
li $v0, 1
syscall
li $a0, '\n'
li $v0, 11
syscall
jal two
jal one
li $v0, 10
syscall
