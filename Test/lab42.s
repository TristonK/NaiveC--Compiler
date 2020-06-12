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

fact:
  addi $sp, $sp, -4
  sw $s8, 0($sp)
  move $s8, $sp
  addi $sp, $sp, -8192
  lw $t0, 8($s8)
  li $t1, 1
  bne $t0, $t1, label1
  lw $t0, 8($s8)
  move $v0, $t0
  move $sp, $s8
  lw $s8, 0($sp)
  addi $sp, $sp, 4
  jr $ra
  j label2
label1:
  lw $t0, 8($s8)
  li $t1, 1
  sub $t2, $t0, $t1
  sw $t2, -4($s8)
  lw $t0, -4($s8)
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t0, $v0
  sw $t0, -8($s8)
  lw $t0, 8($s8)
  lw $t1, -8($s8)
  mul $t2, $t0, $t1
  sw $t2, -12($s8)
  lw $t0, -12($s8)
  move $v0, $t0
  move $sp, $s8
  lw $s8, 0($sp)
  addi $sp, $sp, 4
  jr $ra
label2:

main:
  addi $sp, $sp, -4
  sw $s8, 0($sp)
  move $s8, $sp
  addi $sp, $sp, -8192
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t0, $v0
  sw $t0, -4($s8)
  lw $t0, -4($s8)
  move $t1, $t0
  sw $t1, -8($s8)
  lw $t0, -8($s8)
  li $t1, 1
  ble $t0, $t1, label3
  lw $t0, -8($s8)
  sw $t0, 0($sp)
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal fact
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t0, $v0
  sw $t0, -12($s8)
  lw $t0, -12($s8)
  move $t1, $t0
  sw $t1, -16($s8)
  j label4
label3:
  li $t0, 1
  move $t1, $t0
  sw $t1, -16($s8)
label4:
  lw $t0, -16($s8)
  move $a0, $t0
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $v0, $0
  li $t0, 0
  move $v0, $t0
  move $sp, $s8
  lw $s8, 0($sp)
  addi $sp, $sp, 4
  jr $ra
