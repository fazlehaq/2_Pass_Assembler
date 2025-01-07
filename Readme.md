# 2_Pass_Assembler

## Supported Instructions

- mov
- add
- sub
- div
- mul
- xor
- jmp
- jz
- jnz
- cmp
- inc
- dec

## Assumptions

- Only 32 bit registers will be used
- Opcodes are taken from intel manual
- Scale Index Byte not supported yet

## Update

- Symbol Table generation.
- Encoding of instruction is done.
- lst file is generated in the second pass as intermediate file.
- Object file is now being generated.
- Program to display object file in human readable format.

## Object file format

### Magic number

- 4 byte magic number -> **(0XCAFEBABE)**

### Section headers

- Total 3 - Text section | Data Section | Symbol table
- Format : Name (16 bytes) | offset (4 byte) | size (4 byte)

### Text Section

- Contains the actual binary code of text section.

### Data Section

- Contains the data defined in data section.
- But in binary form.

### Symbol Table Section

- Contains All symbols from code.
- Has the structure as : Name 16 bytes | size 4 bytes | section 1 byte | status 1 byte |

## Read_obj_file :

This program given my object file, prints the human readable equivalent.
