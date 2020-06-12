.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra
# FUNCTION fact_f :

fact_f:
  addi $sp, $sp, -4
  sw $fp, 0($29)
  move $fp, $sp
  addi $sp, $sp, -8192
# PARAM n_v
# ts_1 := n_v
  lw $t0, 8($30)
  move $t1, $t0
  sw $t1, -4($30)
# ts_2 := #1
  li $t0, 1
  move $t1, $t0
  sw $t1, -8($30)
# IF ts_1 == ts_2 GOTO label2
  lw $t0, -4($30)
  lw $t1, -8($30)
  beq $t0, $t1, label2
# GOTO label3
  j label3
# LABEL label2 :
label2:
# ts_3 := n_v
  lw $t0, 8($30)
  move $t1, $t0
  sw $t1, -12($30)
# RETURN ts_3
  lw $t0, -12($30)
  move $v0, $t0
  move $sp, $fp
  lw $fp, 0($29)
  addi $sp, $sp, 4
  jr $ra
# GOTO label4
  j label4
# LABEL label3 :
label3:
# ts_5 := n_v
  lw $t0, 8($30)
  move $t1, $t0
  sw $t1, -16($30)
# ts_8 := n_v
  lw $t0, 8($30)
  move $t1, $t0
  sw $t1, -20($30)
# ts_9 := #1
  li $t0, 1
  move $t1, $t0
  sw $t1, -24($30)
# ts_7 := ts_8 - ts_9
  lw $t0, -20($30)
  lw $t1, -24($30)
  sub $t2, $t0, $t1
  sw $t2, -28($30)
# ARG ts_7
  lw $t0, -28($30)
  sw $t0, 0($29)
# ts_6 := CALL fact_f
  addi $sp, $sp, -4
  sw $ra, 0($29)
  jal fact_f
  lw $ra, 0($29)
  addi $sp, $sp, 4
  move $t0, $v0
  sw $t0, -32($30)
# ts_4 := ts_5 * ts_6
  lw $t0, -16($30)
  lw $t1, -32($30)
  mul $t2, $t0, $t1
  sw $t2, -36($30)
# RETURN ts_4
  lw $t0, -36($30)
  move $v0, $t0
  move $sp, $fp
  lw $fp, 0($29)
  addi $sp, $sp, 4
  jr $ra
# LABEL label4 :
label4:
# FUNCTION main :

main:
  addi $sp, $sp, -4
  sw $fp, 0($29)
  move $fp, $sp
  addi $sp, $sp, -8192
# READ ts_10
  addi $sp, $sp, -4
  sw $ra, 0($29)
  jal read
  lw $ra, 0($29)
  addi $sp, $sp, 4
  move $t0, $v0
  sw $t0, -4($30)
# m_v := ts_10
  lw $t0, -4($30)
  move $t1, $t0
  sw $t1, -8($30)
# ts_11 := m_v
  lw $t0, -8($30)
  move $t1, $t0
  sw $t1, -12($30)
# ts_12 := #1
  li $t0, 1
  move $t1, $t0
  sw $t1, -16($30)
# IF ts_11 > ts_12 GOTO label5
  lw $t0, -12($30)
  lw $t1, -16($30)
  bgt $t0, $t1, label5
# GOTO label6
  j label6
# LABEL label5 :
label5:
# ts_14 := m_v
  lw $t0, -8($30)
  move $t1, $t0
  sw $t1, -20($30)
# ARG ts_14
  lw $t0, -20($30)
  sw $t0, 0($29)
# ts_13 := CALL fact_f
  addi $sp, $sp, -4
  sw $ra, 0($29)
  jal fact_f
  lw $ra, 0($29)
  addi $sp, $sp, 4
  move $t0, $v0
  sw $t0, -24($30)
# result_v := ts_13
  lw $t0, -24($30)
  move $t1, $t0
  sw $t1, -28($30)
# GOTO label7
  j label7
# LABEL label6 :
label6:
# result_v := #1
  li $t0, 1
  move $t1, $t0
  sw $t1, -28($30)
# LABEL label7 :
label7:
# ts_16 := result_v
  lw $t0, -28($30)
  move $t1, $t0
  sw $t1, -32($30)
# WRITE ts_16
  lw $t0, -32($30)
  move $a0, $t0
  addi $sp, $sp, -4
  sw $ra, 0($29)
  jal write
  lw $ra, 0($29)
  addi $sp, $sp, 4
  move $v0, $0
# ts_15 := #0
  li $t0, 0
  move $t1, $t0
  sw $t1, -36($30)
# ts_17 := #0
  li $t0, 0
  move $t1, $t0
  sw $t1, -40($30)
# RETURN ts_17
  lw $t0, -40($30)
  move $v0, $t0
  move $sp, $fp
  lw $fp, 0($29)
  addi $sp, $sp, 4
  jr $ra
