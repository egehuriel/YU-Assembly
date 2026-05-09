mov R2, 0
mov R3, 100
mov R4, 50
L: load R1, mem[R2]
store R1, mem[R3]
addi R2, R2, 1
addi R3, R3, 1
blt R2, R4, L
