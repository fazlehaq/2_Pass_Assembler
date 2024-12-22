## MOV

- **mov r/m32 r32** : 89 /r
- **mov r32 r/m32** : 8B /r
- **mov r32 imm32** : B8+rd
- **mov r/m32 imm32** : C7 /0

## ADD

- **add r/m32 r32** : 01 /r
- **add r32 r/m32** : 03 /r
- **add eax imm32** : 05 id(For some reason 8 bits are also converted to the 32 bits this is happening in hex only)

```
   0000000B 05FE000000 add eax,0xFE
   00000010 81C1FE000000 add ecx,0xFE
   00000016 83C178 add ecx,120
```

- **add r/m32 imm32/16** : 81 /0 id
- **add r/m32 imm8** : 83 /0 ib\*

## SUB

- **sub r/m32 , r32** : 29 /r (also r - r)
- **sub r32 , r/m32** : 2B /r
- **sub eax , imm32** : 2D id
- **sub r32 , imm32** : 81 /5 id
- **sub r32 , imm8** : 83 /5 ib

## DIV

- **div r/m32** : F7 /6

## MUL

- **mul r/m32** : F7 /4

## XOR

- **xor eax , imm32** : 35 id
- **xor r/m32 , imm32** : 81 /6 id
- **xor r/m32 , imm8** : 83 /6 ib
- **xor r/m32 , r32** : 31 /r (Defualt for register register)
- **xor r32 , r/m32** : 33 /r

## JMP

- **jmp rel8** : EB cb
- **jmp rel16** : E9 cw
- **jmp rel32** : E9 CD
- **jmp r/m32** : FF /4 (absolute) (not supported in 64 bit mode)

## JZ

- **jz rel8** : 74 cb
- **jz rel16** : 0F 84 cw
- **jz rel32** : 0F 84 cd

## JNZ

- **jnz rel8** : 75 cb
- **jnz rel32** : 0F 85 cd
- **jnz rel16** : 0F 85 cw

## CMP

- **cmp eax , imm32** : 3D id
- **cmp r/m32 , imm32** : 81 /7 id
- **cmp r/m32 , imm8** : 83 /7 ib
- **cmp r/m32 , r32** : 39 /r
- **cmp r32 , r/m32** : 3B /r

## INC

- **inc r/m32** : FF /0

## DEC

- **dec r32** : 48+rd
- **dec r/m32** : FF /1
