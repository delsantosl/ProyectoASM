BITQUEST - JUEGO HIBRIDO
C & Ensamblador x86_64 (Windows Architecture)

DESCRIPCION DEL PROYECTO:

BitQuest es un videojuego de exploracion de laberintos desarrollado en
formato hibrido. Combina la flexibilidad y control de interfaz de un
lenguaje de alto nivel como C, con la velocidad y optimizacion de bajo
nivel de un lenguaje ensamblador de 64 bits (NASM).

El jugador debe guiar a su personaje ("P") a traves de laberintos,
recolectando monedas ("M") y llaves ("K") para abrir puertas cerradas ("D"),
con el fin de encontrar la salida ("E") con la menor cantidad de pasos
posible para maximizar su puntaje.

INTEGRANTES:

Cristian Alejandro Silva Zamarripa 
Angel Alejandro De Los Santos Lares
Joel Daniel Cuevas Zozaya
Giovanni Emmanuel De La Rosa Contreras


REQUISITOS DEL SISTEMA:

Sistema Operativo: Windows (64 bits)

Compilador de C: GCC v14.0+ (Ubicado en el PATH mediante MSYS2 / UCRT64)

Ensamblador: NASM v2.16+ (Ubicado en el PATH)

ESTRUCTURA DEL PROYECTO:

PROYECTOASM/
│
├── main.c              # Codigo principal en C 
├── rutinas.asm         # Subrutinas en Ensamblador x86_64 (Calculo de puntaje, conteos, colisiones)
│
├── Niveles/            # Carpeta contenedora de los mapas del juego
│   ├── Nivel_1.txt
│   ├── Nivel_2.txt
│   └── Nivel_3.txt
│
├── clean_build.bat     # Script de compilacion automatica y ejecucion limpia
└── README.txt          

INSTRUCCIONES DE COMPILACION Y EJECUCION:

El proyecto cuenta con un script de automatizacion que elimina residuos
de compilaciones previas, ensambla con NASM, compila con GCC, realiza
el enlazado dinamico y arranca el programa.

Para compilar y jugar:

Abre una terminal en la raiz de la carpeta del proyecto.

Ejecuta el script de compilacion:

.\build.bat

De forma manual:

Ensamblar modulo ASM:
nasm -f win64 rutinas.asm -o rutinas.o

Compilar C y enlazar:
gcc main.c rutinas.o -o BitQuest.exe

Ejecutar:
.\BitQuest.exe

CONTROLES DEL JUEGO:

[W] / [w] : Mover hacia Arriba

[S] / [s] : Mover hacia Abajo

[A] / [a] : Mover hacia la Izquierda

[D] / [d] : Mover hacia la Derecha

[Q] / [q] : Abandonar nivel actual (ir al Menu/Siguiente)

Formula de Puntaje:
La formula esta optimizada para procesarse mediante instrucciones
aritmeticas de registros basicos (add, sub, imul, cmp) en RAX:

Puntaje = (Monedas * 100) + (Niveles * 500) - (Pasos * 2)

Cada moneda recolectada: +100 puntos

Cada nivel completado: +500 puntos

Penalizacion por eficiencia (Cada paso): -2 puntos

Si el puntaje calculado resulta negativo, la subrutina en ASM
hace un truncado de seguridad a 0 puntos minimos.
