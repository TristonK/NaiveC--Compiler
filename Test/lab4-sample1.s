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
# FUNCTION main :

main:
  addi $sp, $sp, -4
  sw $fp, 0($29)
  move $fp, $sp
  addi $sp, $sp, -10240
# a_v := #0
  li $t0, 0
  move $t1, $t0
  sw $t1, -4($30)
# b_v := #1
  li $t0, 1
  move $t1, $t0
  sw $t1, -8($30)
# i_v := #0
  li $t0, 0
  move $t1, $t0
  sw $t1, -12($30)
# READ ts_1
  addi $sp, $sp, -4
  sw $ra, 0($29)
  jal read
  lw $ra, 0($29)
  addi $sp, $sp, 4
  move $t0, $v0
  sw $t0, -16($30)
# n_v := ts_1
  lw $t0, -16($30)
  move $t1, $t0
  sw $t1, -20($30)
# ts_3 := n_v
  lw $t0, -20($30)
  move $t1, $t0
  sw $t1, -24($30)
# WRITE ts_3
  lw $t0, -24($30)
  move $a0, $t0
  addi $sp, $sp, -4
  sw $ra, 0($29)
  jal write
  lw $ra, 0($29)
  addi $sp, $sp, 4
  move $v0, $0
# ts_2 := #0
  li $t0, 0
  move $t1, $t0
  sw $t1, -28($30)
# LABEL label2 :
label2:
# ts_4 := i_v
  lw $t0, -12($30)
  move $t1, $t0
  sw $t1, -32($30)
# ts_5 := n_v
  lw $t0, -20($30)
  move $t1, $t0
  sw $t1, -36($30)
# IF ts_4 < ts_5 GOTO label3
  lw $t0, -32($30)
  lw $t1, -36($30)
  blt $t0, $t1, label3
# GOTO label4
  j label4
# LABEL label3 :
label3:
# ts_6 := a_v
  lw $t0, -4($30)
  move $t1, $t0
  sw $t1, -40($30)
# ts_7 := b_v
  lw $t0, -8($30)
  move $t1, $t0
  sw $t1, -44($30)
# c_v := ts_6 + ts_7
  lw $t0, -40($30)
  lw $t1, -44($30)
  add $t2, $t0, $t1
  sw $t2, -48($30)
# ts_9 := b_v
  lw $t0, -8($30)
  move $t1, $t0
  sw $t1, -52($30)
# WRITE ts_9
  lw $t0, -52($30)
  move $a0, $t0
  addi $sp, $sp, -4
  sw $ra, 0($29)
  jal write
  lw $ra, 0($29)
  addi $sp, $sp, 4
  move $v0, $0
# ts_8 := #0
  li $t0, 0
  move $t1, $t0
  sw $t1, -56($30)
# ts_10 := b_v
  lw $t0, -8($30)
  move $t1, $t0
  sw $t1, -60($30)
# a_v := ts_10
  lw $t0, -60($30)
  move $t1, $t0
  sw $t1, -4($30)
# ts_11 := c_v
  lw $t0, -48($30)
  move $t1, $t0
  sw $t1, -64($30)
# b_v := ts_11
  lw $t0, -64($30)
  move $t1, $t0
  sw $t1, -8($30)
# ts_13 := i_v
  lw $t0, -12($30)
  move $t1, $t0
  sw $t1, -68($30)
# ts_14 := #1
  li $t0, 1
  move $t1, $t0
  sw $t1, -72($30)
# ts_12 := ts_13 + ts_14
  lw $t0, -68($30)
  lw $t1, -72($30)
  add $t2, $t0, $t1
  sw $t2, -76($30)
# i_v := ts_12
  lw $t0, -76($30)
  move $t1, $t0
  sw $t1, -12($30)
# GOTO label2
  j label2
# LABEL label4 :
label4:
# ts_15 := #0
  li $t0, 0
  move $t1, $t0
  sw $t1, -80($30)
# RETURN ts_15
  lw $t0, -80($30)
  move $v0, $t0
  move $sp, $fp
  jr $ra
