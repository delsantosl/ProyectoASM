#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define FILAS 60
#define COLS  60

// Funciones que se usan en rutinas.asm
void movimiento(char *matriz, int columnas, int fila1, int columna1, int fila2, int columna2);
int validar_movimiento(char *mapa, int cols, int fila, int col);
int contar_caracteres(char *mapa, int total_celdas, char caracter);
int contar_celdas_libres(char *mapa, int total_celdas);
int calcular_puntaje(int monedas, int pasos, int niveles);
int detectar_objeto(char *mapa, int cols, int fila, int col, char objeto);

// Arreglo de niveles para seleccionar el nivel que se quiere jugar
const char *archivos_niveles[3] = {
    "Niveles/Nivel_1.txt",
    "Niveles/Nivel_2.txt",
    "Niveles/Nivel_3.txt"
};

char mapa[FILAS][COLS];

// Variables globales para el resumen final de BitQuest
int global_monedas_recolectadas = 0;
int global_monedas_totales = 0;
int global_pasos = 0;
int global_niveles_completados = 0;
int global_puntaje = 0;

// Menu principal (seleccion de niveles)
int seleccionar_nivel() {
    int opcion;
    char buf[100];  // variable auxiliar para validar opciones
    printf("=================================\n");
    printf("            BitQuest\n");
    printf("=================================\n");
    printf("Selecciona un nivel:\n");
    printf("  1. Nivel 1\n");
    printf("  2. Nivel 2\n");
    printf("  3. Nivel 3\n");
    printf("  0. Salir\n");
    printf("=================================\n");
    printf("Opcion: ");
    // validacion de la opcion seleccionada
    fgets(buf, sizeof(buf), stdin);
    opcion = atoi(buf);
    if (buf[0] >= '1' && buf[0] <= '3') {
        opcion = atoi(buf);
    } else if (buf[0] == '0') {
        opcion = 0;
    } else {
        opcion = -1;  // cualquier cosa que no sea numero es invalida
    }
    return opcion;
}

void cargar_mapa(const char *archivo) {
    FILE *f = fopen(archivo, "r");
    if (f == NULL) {
        printf("Error: no se pudo abrir %s\n", archivo);
        exit(1);
    }
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLS; j++) {
            mapa[i][j] = '#';   // valor por default por si el mapa esta incompleto
        }
    }
    int fila = 0;
    char linea[256];
    while (fgets(linea, sizeof(linea), f) && fila < FILAS) {    // lee el archivo linea por linea
        int col = 0;
        // Recorre cada carácter de la linea que se acaba de leer y lo copia al mapa
        for (int j = 0; linea[j] != '\0' && linea[j] != '\n' && col < COLS; j++) {
            mapa[fila][col] = linea[j];
            col++;
        }
        fila++;
    }
    fclose(f);
}


void imprimir_mapa(int recol, int total, int llaves, int jFila, int jCol, int pasos) {
    int tamCamara = 20;
    int centro = tamCamara / 2;

    int inicioFila = jFila - centro;
    int inicioCol = jCol - centro;

    if (inicioFila < 0) inicioFila = 0;
    if (inicioCol < 0) inicioCol = 0;
    if (inicioFila + tamCamara > FILAS) inicioFila = FILAS - tamCamara;
    if (inicioCol + tamCamara > COLS) inicioCol = COLS - tamCamara;

    char impMapa[20 * (20 * 2 + 1) + 1];  // guarda todo el mapa para imprimirlo
    int pos = 0;
    for (int i = inicioFila; i < inicioFila + tamCamara; i++) {
        for (int j = inicioCol; j < inicioCol + tamCamara; j++) {
            if (mapa[i][j] == '#') {
                impMapa[pos++] = (char)254;  // bloque solido para pared
            } else if (mapa[i][j] == '.') {
                impMapa[pos++] = (char)250;  // camino libre (es un punto centrado)
            } else if (mapa[i][j] == 'P') {
                impMapa[pos++] = (char)143;  // jugador
            } else if (mapa[i][j] == 'M') {
                impMapa[pos++] = (char)184;  // monedas
            } else if (mapa[i][j] == 'K') {
                impMapa[pos++] = (char)190;  // llaves
            } else if (mapa[i][j] == 'D') {
                impMapa[pos++] = (char)176;  // puertas
            } else if (mapa[i][j] == 'E') {
                impMapa[pos++] = (char)245;  // salida
            }
            impMapa[pos++] = ' ';           // espacio para balance visual
        }
        impMapa[pos++] = '\n';
    }
    printf("Monedas: %d / %d  |  Llaves: %d  |  Pasos: %d", recol, total, llaves, pasos);
    printf("\n");
    impMapa[pos] = '\0';    // terminar la cadena
    fputs(impMapa, stdout); // imprimir todo de una vez para mayor velocidad

    // Panel de Simbología dinámico en tiempo real bajo el mapa
    printf("\n=======================================================\n");
    printf("                      SIMBOLOGIA\n");
    printf("=======================================================\n");
    printf("  %c : Jugador (Tu)     %c : Camino Libre    %c : Pared\n", (char)143, (char)250, (char)254);
    printf("  %c : Moneda           %c : Llave           %c : Puerta Cerrada\n", (char)184, (char)190, (char)176);
    printf("  %c : Salida (Meta)    Q : Salir del Nivel\n", (char)245);
    printf("=======================================================\n");
}

// buscar la posicion inicial del jugador en el mapa
void buscar_jugador(int *fila, int *col) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (mapa[i][j] == 'P') {
                *fila = i;
                *col = j;
                return;
            }
        }
    }
}

// jugar al nivel seleccionado
int jugar_nivel(int num_nivel) {
    cargar_mapa(archivos_niveles[num_nivel - 1]);   // cargar el mapa del nivel seleccionado
    int celdasLib = contar_celdas_libres(&mapa[0][0], FILAS * COLS);
    printf("\nTotal de celdas libres: %d\n\n", celdasLib);
    int totalMonedas = contar_caracteres(&mapa[0][0], FILAS * COLS, 'M');
    printf("\nTotal de monedas: %d\n", totalMonedas);
    int jugador_fila, jugador_col;
    buscar_jugador(&jugador_fila, &jugador_col);

    int Mrecolectadas = 0;
    int llaves = 0;            // Contador de llaves en inventario
    int pasos = 0;             // Contador de pasos realizados
    int nivel_completado = 0;  // Bandera para saber si cruzó la salida 'E'

    imprimir_mapa(Mrecolectadas, totalMonedas, llaves, jugador_fila, jugador_col, pasos);    // imprimir el mapa del nivel cargado
    int nueva_fila, nueva_col;
    char tecla;

    while (1) { 
        tecla = _getch();
        nueva_fila = jugador_fila;
        nueva_col  = jugador_col;
        if (tecla == 'w' || tecla == 'W') nueva_fila--;         // moverse hacia arriba
        else if (tecla == 's' || tecla == 'S') nueva_fila++;    // moverse hacia abajo
        else if (tecla == 'a' || tecla == 'A') nueva_col--;     // moverse hacia la izquierda
        else if (tecla == 'd' || tecla == 'D') nueva_col++;     // moverse hacia la derecha
        else if (tecla == 'q' || tecla == 'Q') break;           // salir del nivel (volver al menu principal)
        else continue;      // si se presiona otra tecla no hace nada
        
        // 1. Validar movimiento basico (que no sea pared '#')
        if (validar_movimiento(&mapa[0][0], COLS, nueva_fila, nueva_col)) {
            char destino = mapa[nueva_fila][nueva_col];
            int puede_pasar = 1;

            // Lógica de la Puerta ('D')
            if (detectar_objeto(&mapa[0][0], COLS, nueva_fila, nueva_col, 'D')) {
                if (llaves > 0) {
                    llaves--;                   
                    mapa[nueva_fila][nueva_col] = '.'; 
                } else {
                    lock_door:
                    puede_pasar = 0;            
                }
            }

            // Procesar movimiento si el paso es permitido
            if (puede_pasar) {
                pasos++;
                
                // Si pisa una Moneda
                if (detectar_objeto(&mapa[0][0], COLS, nueva_fila, nueva_col, 'M')) {
                    Mrecolectadas++;
                    mapa[nueva_fila][nueva_col] = '.';
                } 
                // Si pisa una Llave ('K')
                else if (detectar_objeto(&mapa[0][0], COLS, nueva_fila, nueva_col, 'K')) {
                    llaves++;
                    mapa[nueva_fila][nueva_col] = '.'; 
                } 
                // Si pisa la Salida ('E')
                else if (detectar_objeto(&mapa[0][0], COLS, nueva_fila, nueva_col, 'E')) {
                    nivel_completado = 1;
                    break; 
                }

                // Mover fisicamente al jugador en la matriz 
                movimiento(&mapa[0][0], COLS, jugador_fila, jugador_col, nueva_fila, nueva_col);
                jugador_fila = nueva_fila;
                jugador_col  = nueva_col;
                
                system("cls");
                imprimir_mapa(Mrecolectadas, totalMonedas, llaves, jugador_fila, jugador_col, pasos);
            }
        }
    } // FIN del ciclo while (1)

    system("cls");
    int puntaje_final = calcular_puntaje(Mrecolectadas, pasos, nivel_completado ? num_nivel : 0);
    
    // Acumulación de estadísticas en las variables globales para el resumen del Juego Completo
    global_monedas_recolectadas += Mrecolectadas;
    global_monedas_totales += totalMonedas;
    global_pasos += pasos;
    if (nivel_completado) {
        global_niveles_completados++;
    }
    global_puntaje += puntaje_final;

    printf("=================================\n");
    if (nivel_completado) {
        printf("    Felicidades! NIVEL COMPLETADO\n");
    } else {
        printf("       NIVEL ABANDONADO (Q)\n");
    }
    printf("=================================\n");
    printf(" Monedas recolectadas: %d / %d\n", Mrecolectadas, totalMonedas);
    printf(" Pasos realizados:     %d\n", pasos);
    printf(" Nivel completado:     %s\n", nivel_completado ? "SI" : "NO");
    printf("---------------------------------\n");
    printf(" TU PUNTAJE FINAL:    %d pts!\n", puntaje_final);
    printf("=================================\n");
    printf("\nPresiona cualquier tecla para avanzar...\n");
    _getch();
    system("cls");

    return nivel_completado;
}

int main() {
    system("cls");
    printf("=================================\n");
    printf("            BitQuest\n");
    printf("=================================\n");
    printf("\nPresiona cualquier tecla para comenzar tu aventura...\n");
    _getch();

    // Recorrer los niveles de corrido (del 1 al 3)
    for (int nivel_actual = 1; nivel_actual <= 3; nivel_actual++) {
        int superado = jugar_nivel(nivel_actual);
        
        // Si el jugador abandona con 'Q' (superado == 0), salimos del juego
        if (!superado) {
            printf("\nHas abandonado la partida. Saliendo del juego...\n");
            break;
        }
    }

    // Si logró pasar los 3 niveles, mostramos el Resumen Final
    if (global_niveles_completados == 3) {
        system("cls");
        printf("=================================\n");
        printf("        JUEGO COMPLETADO\n");
        printf("=================================\n");
        printf(" Monedas totales:     %d / %d\n", global_monedas_recolectadas, global_monedas_totales);
        printf(" Pasos totales:       %d\n", global_pasos);
        printf(" Niveles completados: %d\n", global_niveles_completados);
        printf(" Puntaje final:       %d pts!\n", global_puntaje);
        printf("=================================\n");
        printf("\nPresiona cualquier tecla para salir...\n");
        _getch();
    }

    return 0;
}