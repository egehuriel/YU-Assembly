mov R0, 0
mov R1, 10
LOOP1: addi R0, R0, 1
blt R0, R1, LOOP1
mov R2, 0
mov R3, 5
LOOP2: addi R2, R2, 1
blt R2, R3, LOOP2
