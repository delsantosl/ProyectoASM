bits 64
default rel

global movimiento, validar_movimiento

section .text

movimiento:
  mov eax,r8d
  imul eax, edx
  add eax,r9d

  lea r10, [rcx +rax]

  mov r11d,[rsp +40]
  mov eax, r11d
  imul eax, edx

  mov r11d, [rsp +48]
  add eax, r11d
  
  lea r11d, [rcx + rax]

  mov al, [r10]
  mov r8b, [r11]

  mov [r10], r8b
  mov [r11], al 
  ret

validar_movimiento:
    mov eax, r8d
    imul eax, edx
    add eax, r9d
    movsx rax, eax
    mov r10b, [rcx + rax]
    cmp r10b, '#'
    je es_pared
    mov eax, 1
    ret

es_pared:
    mov eax, 0
    ret
  

