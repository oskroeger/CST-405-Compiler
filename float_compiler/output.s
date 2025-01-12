.data
var_t_11: .word 0
var_t_12: .word 0
var_t_10: .word 0
var_t_14: .word 0
var_t_15: .word 0
var_t_13: .word 0
var_t_17: .word 0
var_t_18: .word 0
var_t_16: .word 0
var_t_19: .word 0
var_t_20: .word 0
var_t_22: .word 0
var_t_23: .word 0
var_t_21: .word 0
var_t_26: .word 0
var_t_27: .word 0
var_t_25: .word 0
var_t_28: .word 0
var_t_24: .word 0
var_t_29: .word 0
var_t_30: .word 0
var_t_32: .word 0
var_t_33: .word 0
var_t_31: .word 0
var_t_35: .word 0
var_t_36: .word 0
var_t_34: .word 0
var_t_38: .word 0
var_t_39: .word 0
var_t_37: .word 0
var_t_41: .word 0
var_t_42: .word 0
var_t_40: .word 0
var_t_43: .word 0
var_t_45: .word 0
var_t_46: .word 0
var_t_44: .word 0
var_t_47: .word 0
var_t_49: .word 0
var_t_50: .word 0
var_t_48: .word 0
var_t_51: .word 0
var_t_52: .word 0
var_t_54: .word 0
var_t_55: .word 0
var_t_53: .word 0
var_t_57: .word 0
var_t_60: .word 0
var_t_61: .word 0
var_t_59: .word 0
var_t_58: .word 0
var_t_56: .word 0
var_t_62: .word 0
var_t_63: .word 0
var_t_64: .word 0
var_t_65: .word 0
var_t_66: .word 0
var_t_67: .word 0
var_t_68: .word 0
var_t_73: .word 0
var_t_74: .word 0
var_t_72: .word 0
var_t_75: .word 0
var_t_71: .word 0
var_t_77: .word 0
var_t_78: .word 0
var_t_76: .word 0
var_t_70: .word 0
var_t_79: .word 0
var_t_69: .word 0
var_t_81: .word 0
var_t_82: .word 0
var_t_80: .word 0
var_t_84: .word 0
var_t_85: .word 0
var_t_83: .word 0
var_t_87: .word 0
var_t_88: .word 0
var_t_86: .word 0
var_t_90: .word 0
var_t_91: .word 0
var_t_89: .word 0
var_t_92: .word 0
var_t_93: .word 0
var_t_95: .word 0
var_t_96: .word 0
var_t_94: .word 0
var_t_98: .word 0
var_t_100: .word 0
var_t_101: .word 0
var_t_99: .word 0
var_t_97: .word 0
var_t_103: .word 0
var_t_104: .word 0
var_t_102: .word 0
var_t_106: .word 0
var_t_107: .word 0
var_t_105: .word 0
var_t_108: .word 0
var_t_109: .word 0
var_t_110: .word 0
var_t_112: .word 0
var_t_113: .word 0
var_t_111: .word 0
var_t_114: .word 0
var_t_115: .word 0
var_t_116: .word 0
var_t_117: .word 0
var_t_118: .word 0
var_t_119: .word 0
var_a: .word 0
var_testInt: .word 0
var_b: .word 0
var_sum: .word 0
var_temp: .word 0
var_doubleSum: .word 0
var_localCopy: .word 0
var_printed: .word 0
var_product: .word 0
var_square: .word 0
var_tempResult: .word 0
var_test: .word 0
var_result: .word 0
var_localInt: .word 0
var_localIntTwo: .word 0
var_i: .word 0

.text
func_printInt:
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    move $fp, $sp
    move $s0, $a0
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
endfunc_printInt:
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra
func_computeSum:
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    move $fp, $sp
    li $s0, 4
    move $t0, $s0
    move $s0, $t0
    sw $s0, var_testInt
    move $s0, $a0
    move $t1, $s0
    move $s0, $a1
    move $t2, $s0
    move $s0, $t1
    move $s1, $t2
    add $t9, $s0, $s1
    move $t3, $t9
    move $s0, $t3
    sw $s0, var_sum
    lw $s0, var_sum
    move $t4, $s0
    li $s0, 5
    move $t5, $s0
    move $s0, $t4
    move $s1, $t5
    add $t9, $s0, $s1
    move $t6, $t9
    move $s0, $t6
    sw $s0, var_temp
    lw $s0, var_temp
    move $t7, $s0
    li $s0, 2
    move $t8, $s0
    move $s0, $t7
    move $s1, $t8
    mul $t9, $s0, $s1
    move $t9, $t9
    move $s0, $t9
    sw $s0, var_doubleSum
    lw $s0, var_doubleSum
    sw $s0, var_t_11
    li $s0, 50
    sw $s0, var_t_12
    lw $s0, var_t_11
    lw $s1, var_t_12
    slt $t9, $s1, $s0
    sw $t9, var_t_10
    lw $s0, var_t_10
    bne $s0, $zero, L_then_0
    lw $s0, var_testInt
    sw $s0, var_t_14
    li $s0, 5
    sw $s0, var_t_15
    lw $s0, var_t_14
    lw $s1, var_t_15
    sub $t9, $s0, $s1
    sw $t9, var_t_13
    lw $s0, var_t_13
    sw $s0, var_testInt
    j L_end_0
L_then_0:
    lw $s0, var_testInt
    sw $s0, var_t_17
    li $s0, 10
    sw $s0, var_t_18
    lw $s0, var_t_17
    lw $s1, var_t_18
    add $t9, $s0, $s1
    sw $t9, var_t_16
    lw $s0, var_t_16
    sw $s0, var_testInt
L_end_0:
    lw $s0, var_testInt
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $s0, var_doubleSum
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $s0, var_doubleSum
    sw $s0, var_t_19
    lw $v0, var_t_19
    j endfunc_computeSum
endfunc_computeSum:
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra
func_displayResult:
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    move $fp, $sp
    move $s0, $a0
    sw $s0, var_t_20
    lw $s0, var_t_20
    sw $s0, var_localCopy
    lw $s0, var_localCopy
    sw $s0, var_t_22
    li $s0, 1
    sw $s0, var_t_23
    lw $s0, var_t_22
    lw $s1, var_t_23
    add $t9, $s0, $s1
    sw $t9, var_t_21
    lw $s0, var_t_21
    sw $s0, var_localCopy
    lw $s0, var_localCopy
    sw $s0, var_t_26
    li $s0, 2
    sw $s0, var_t_27
    lw $s0, var_t_26
    lw $s1, var_t_27
    div $s0, $s1
    mflo $t9
    sw $t9, var_t_25
    li $s0, 0
    sw $s0, var_t_28
    lw $s0, var_t_25
    lw $s1, var_t_28
    bne $s0, $s1, L_neq_true_0
    li $t9, 0
    j L_neq_end_1
L_neq_true_0:
    li $t9, 1
L_neq_end_1:
    sw $t9, var_t_24
    lw $s0, var_t_24
    bne $s0, $zero, L_then_1
    li $s0, 0
    sw $s0, var_t_29
    lw $s0, var_t_29
    sw $s0, var_printed
    j L_end_1
L_then_1:
    li $s0, 5
    sw $s0, var_t_30
    lw $s0, var_t_30
    sw $s0, var_printed
L_end_1:
    lw $s0, var_printed
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $s0, var_localCopy
    sw $s0, var_t_32
    li $s0, 3
    sw $s0, var_t_33
    lw $s0, var_t_32
    lw $s1, var_t_33
    mul $t9, $s0, $s1
    sw $t9, var_t_31
    lw $s0, var_t_31
    sw $s0, var_localCopy
    lw $s0, var_localCopy
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
endfunc_displayResult:
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra
func_multiply:
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    move $fp, $sp
    move $s0, $a0
    sw $s0, var_t_35
    move $s0, $a1
    sw $s0, var_t_36
    lw $s0, var_t_35
    lw $s1, var_t_36
    mul $t9, $s0, $s1
    sw $t9, var_t_34
    lw $s0, var_t_34
    sw $s0, var_product
    move $s0, $a0
    sw $s0, var_t_38
    move $s0, $a0
    sw $s0, var_t_39
    lw $s0, var_t_38
    lw $s1, var_t_39
    mul $t9, $s0, $s1
    sw $t9, var_t_37
    lw $s0, var_t_37
    sw $s0, var_square
    lw $s0, var_product
    sw $s0, var_t_41
    lw $s0, var_square
    sw $s0, var_t_42
    lw $s0, var_t_41
    lw $s1, var_t_42
    add $t9, $s0, $s1
    sw $t9, var_t_40
    lw $s0, var_t_40
    sw $s0, var_tempResult
    move $s0, $a0
    sw $s0, var_t_43
    lw $s0, var_t_43
    sw $s0, var_test
    lw $s0, var_tempResult
    sw $s0, var_t_45
    li $s0, 50
    sw $s0, var_t_46
    lw $s0, var_t_45
    lw $s1, var_t_46
    slt $t9, $s1, $s0
    sw $t9, var_t_44
    lw $s0, var_t_44
    bne $s0, $zero, L_then_2
    li $s0, 18
    sw $s0, var_t_47
    lw $s0, var_t_47
    sw $s0, var_test
    j L_end_2
L_then_2:
    lw $s0, var_test
    sw $s0, var_t_49
    li $s0, 2
    sw $s0, var_t_50
    lw $s0, var_t_49
    lw $s1, var_t_50
    div $s0, $s1
    mflo $t9
    sw $t9, var_t_48
    lw $s0, var_t_48
    sw $s0, var_test
L_end_2:
    lw $s0, var_test
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $s0, var_tempResult
    sw $s0, var_t_51
    lw $s0, var_t_51
    move $a0, $s0
    jal func_printInt
    lw $s0, var_tempResult
    sw $s0, var_t_52
    lw $v0, var_t_52
    j endfunc_multiply
endfunc_multiply:
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra
func_factorial:
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    move $fp, $sp
    move $s0, $a0
    sw $s0, var_t_54
    li $s0, 1
    sw $s0, var_t_55
    lw $s0, var_t_54
    lw $s1, var_t_55
    slt $t9, $s1, $s0
    xori $t9, $t9, 1
    sw $t9, var_t_53
    lw $s0, var_t_53
    bne $s0, $zero, L_then_3
    move $s0, $a0
    sw $s0, var_t_57
    move $s0, $a0
    sw $s0, var_t_60
    li $s0, 1
    sw $s0, var_t_61
    lw $s0, var_t_60
    lw $s1, var_t_61
    sub $t9, $s0, $s1
    sw $t9, var_t_59
    lw $s0, var_t_59
    move $a0, $s0
    jal func_factorial
    sw $v0, var_t_58
    lw $s0, var_t_57
    lw $s1, var_t_58
    mul $t9, $s0, $s1
    sw $t9, var_t_56
    lw $s0, var_t_56
    sw $s0, var_result
    lw $s0, var_result
    sw $s0, var_t_62
    lw $s0, var_t_62
    move $a0, $s0
    jal func_printInt
    lw $s0, var_result
    sw $s0, var_t_63
    lw $v0, var_t_63
    j endfunc_factorial
    j L_end_3
L_then_3:
    li $s0, 1
    sw $s0, var_t_64
    lw $s0, var_t_64
    move $a0, $s0
    jal func_printInt
    li $s0, 1
    sw $s0, var_t_65
    lw $v0, var_t_65
    j endfunc_factorial
L_end_3:
endfunc_factorial:
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra
func_main:
    addi $sp, $sp, -8
    sw $ra, 4($sp)
    sw $fp, 0($sp)
    move $fp, $sp
    li $s0, 15
    sw $s0, var_t_66
    lw $s0, var_t_66
    sw $s0, var_localInt
    li $s0, 7
    sw $s0, var_t_67
    lw $s0, var_t_67
    sw $s0, var_localIntTwo
    lw $s0, var_localInt
    sw $s0, var_t_68
    lw $s0, var_t_68
    move $a0, $s0
    jal func_printInt
    lw $s0, var_localInt
    sw $s0, var_t_73
    li $s0, 2
    sw $s0, var_t_74
    lw $s0, var_t_73
    lw $s1, var_t_74
    mul $t9, $s0, $s1
    sw $t9, var_t_72
    li $s0, 3
    sw $s0, var_t_75
    lw $s0, var_t_72
    lw $s1, var_t_75
    add $t9, $s0, $s1
    sw $t9, var_t_71
    li $s0, 5
    sw $s0, var_t_77
    li $s0, 2
    sw $s0, var_t_78
    lw $s0, var_t_77
    lw $s1, var_t_78
    sub $t9, $s0, $s1
    sw $t9, var_t_76
    lw $s0, var_t_71
    lw $s1, var_t_76
    div $s0, $s1
    mflo $t9
    sw $t9, var_t_70
    lw $s0, var_localIntTwo
    sw $s0, var_t_79
    lw $s0, var_t_70
    lw $s1, var_t_79
    add $t9, $s0, $s1
    sw $t9, var_t_69
    lw $s0, var_t_69
    sw $s0, var_result
    lw $s0, var_result
    move $a0, $s0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $s0, var_result
    sw $s0, var_t_81
    li $s0, 10
    sw $s0, var_t_82
    lw $s0, var_t_81
    lw $s1, var_t_82
    slt $t9, $s1, $s0
    sw $t9, var_t_80
    lw $s0, var_t_80
    bne $s0, $zero, L_then_4
    lw $s0, var_result
    sw $s0, var_t_84
    li $s0, 1
    sw $s0, var_t_85
    lw $s0, var_t_84
    lw $s1, var_t_85
    sub $t9, $s0, $s1
    sw $t9, var_t_83
    lw $s0, var_t_83
    sw $s0, var_temp
    lw $s0, var_temp
    sw $s0, var_t_87
    li $s0, 3
    sw $s0, var_t_88
    lw $s0, var_t_87
    lw $s1, var_t_88
    add $t9, $s0, $s1
    sw $t9, var_t_86
    lw $s0, var_t_86
    move $a0, $s0
    jal func_displayResult
    j L_end_4
L_then_4:
    lw $s0, var_result
    sw $s0, var_t_90
    li $s0, 2
    sw $s0, var_t_91
    lw $s0, var_t_90
    lw $s1, var_t_91
    mul $t9, $s0, $s1
    sw $t9, var_t_89
    lw $s0, var_t_89
    sw $s0, var_temp
    lw $s0, var_temp
    sw $s0, var_t_92
    lw $s0, var_t_92
    move $a0, $s0
    jal func_displayResult
L_end_4:
    li $s0, 0
    sw $s0, var_t_93
    lw $s0, var_t_93
    sw $s0, var_i
L_while_start_5:
    lw $s0, var_i
    sw $s0, var_t_95
    li $s0, 10
    sw $s0, var_t_96
    lw $s0, var_t_95
    lw $s1, var_t_96
    slt $t9, $s0, $s1
    sw $t9, var_t_94
    lw $s0, var_t_94
    beq $s0, $zero, L_while_end_5
    lw $s0, var_i
    sw $s0, var_t_98
    lw $s0, var_t_98
    move $a0, $s0
    lw $s0, var_i
    sw $s0, var_t_100
    li $s0, 1
    sw $s0, var_t_101
    lw $s0, var_t_100
    lw $s1, var_t_101
    add $t9, $s0, $s1
    sw $t9, var_t_99
    lw $s0, var_t_99
    move $a1, $s0
    jal func_multiply
    sw $v0, var_t_97
    lw $s0, var_t_97
    sw $s0, var_result
    lw $s0, var_result
    sw $s0, var_t_103
    li $s0, 20
    sw $s0, var_t_104
    lw $s0, var_t_103
    lw $s1, var_t_104
    slt $t9, $s1, $s0
    sw $t9, var_t_102
    lw $s0, var_t_102
    bne $s0, $zero, L_then_6
    lw $s0, var_localInt
    sw $s0, var_t_106
    li $s0, 1
    sw $s0, var_t_107
    lw $s0, var_t_106
    lw $s1, var_t_107
    add $t9, $s0, $s1
    sw $t9, var_t_105
    lw $s0, var_t_105
    sw $s0, var_localInt
    j L_end_6
L_then_6:
    lw $s0, var_result
    sw $s0, var_t_108
    lw $s0, var_t_108
    sw $s0, var_localInt
    li $s0, 21
    sw $s0, var_t_109
    lw $s0, var_t_109
    sw $s0, var_i
L_end_6:
    lw $s0, var_i
    sw $s0, var_t_110
    lw $s0, var_t_110
    move $a0, $s0
    jal func_printInt
    lw $s0, var_i
    sw $s0, var_t_112
    li $s0, 1
    sw $s0, var_t_113
    lw $s0, var_t_112
    lw $s1, var_t_113
    add $t9, $s0, $s1
    sw $t9, var_t_111
    lw $s0, var_t_111
    sw $s0, var_i
    j L_while_start_5
L_while_end_5:
    li $s0, 2
    sw $s0, var_t_114
    lw $s0, var_t_114
    move $a0, $s0
    li $s0, 6
    sw $s0, var_t_115
    lw $s0, var_t_115
    move $a1, $s0
    jal func_computeSum
    li $s0, 7
    sw $s0, var_t_116
    lw $s0, var_t_116
    move $a0, $s0
    jal func_displayResult
    li $s0, 2
    sw $s0, var_t_117
    lw $s0, var_t_117
    move $a0, $s0
    li $s0, 5
    sw $s0, var_t_118
    lw $s0, var_t_118
    move $a1, $s0
    jal func_multiply
    li $s0, 6
    sw $s0, var_t_119
    lw $s0, var_t_119
    move $a0, $s0
    jal func_factorial
endfunc_main:
    move $sp, $fp
    lw $fp, 0($sp)
    lw $ra, 4($sp)
    addi $sp, $sp, 8
    jr $ra
main:
    jal func_main
    li $v0, 10
    syscall
