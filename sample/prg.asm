section .data
    num1 dd 10                  
    num2 dd 20                 

section .bss
    temp resd 1                

section .text
    mov eax, 5             
    mov ebx, eax           
    mov ecx, [num1]     
    mov edx, [temp]        

    add eax, 10            
    add eax, ebx          
    add eax, [num2]      

    sub eax, num1           
    sub [eax], ebx          
    sub eax, [num1]        

bz: mov ebx, -2             
    mul ecx        
    div ebx               

    xor eax, eax           
    xor ebx, edx   
    jnz le                 

    inc eax                
    dec ebx                

    cmp eax, ebx           
    jz le

le: mov eax, 1             
    