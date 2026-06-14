bits 64
default rel

global movimiento, validar_movimiento, contar_caracteres, contar_celdas_libres

section .text

; void movimiento(char *matriz, int columnas, int fila1, int columna1, int fila2, int columna2)
; rcx = direccion inicial del mapa
; edx = numero de columnas del mapa
; r8d = posicion actual del jugador (fila)
; r9d = posicion actual del jugador (columna)
; [rsp+40] = posicion nueva (fila)
; [rsp+48] = posicion nueva (columna)

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

; int validar_movimiento(char *mapa, int cols, int fila, int col)
; rcx = direccion inicial del mapa
; edx = numero de columnas del mapa
; r8d = fila a validar
; r9d = columna a validar
validar_movimiento:
    mov eax, r8d
    imul eax, edx   ;eax= fila * columnas
    add eax, r9d    ; eax = indice de la celda
    movsx rax, eax  ;extender indice a 64 bits (ya que rcx es un puntero de 64 bits) asi se puede usar para acceder a la celda correcta
    mov r10b, [rcx + rax]   ; leer caracter del mapa [fila][columna]
    cmp r10b, '#'
    je es_pared
    mov eax, 1
    ret         ;regresa 1 si el movimiento es valido y 0 si no lo es

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
    ret         ;regresa el numero de veces que aparece el caracter solicitado

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
    ret         ;regresa el numero de celdas libres '.'