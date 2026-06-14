bits 64
default rel

global movimiento, validar_movimiento, contar_caracteres, contar_celdas_libres

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
  
; int contar_caracteres(char *mapa, int total_celdas, char caracter)
; rcx = direccion del mapa
; edx = total de celdas
; r8b = caracter a buscar
contar_caracteres:
    xor eax, eax        ; contador = 0
    xor r9d, r9d        ; indice = 0

.loop:
    cmp r9d, edx        ; si indice >= total_celdas, terminar
    jge .fin
    mov r10b, [rcx + r9]  ; caracter actual del mapa
    cmp r10b, r8b          ; comparar con el caracter buscado
    jne .siguiente
    inc eax             ; encontrado, incrementar contador

.siguiente:
    inc r9d
    jmp .loop

.fin:
    ret

; int contar_celdas_libres(char *mapa, int total_celdas)
; rcx = direccion del mapa
; edx = total de celdas
contar_celdas_libres:
    xor eax, eax        ; contador = 0
    xor r9d, r9d        ; indice = 0

.loop_libres:
    cmp r9d, edx
    jge .fin_libres
    mov r10b, [rcx + r9]
    cmp r10b, '.'
    jne .siguiente_libre
    inc eax

.siguiente_libre:
    inc r9d
    jmp .loop_libres

.fin_libres:
    ret