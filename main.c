#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#define FILAS 60
#define COLS  60

//Funciones que se usan en rutinas.asm
void movimiento(char *matriz, int columnas, int fila1, int columna1, int fila2, int columna2);
int validar_movimiento(char *mapa, int cols, int fila, int col);
int contar_caracteres(char *mapa, int total_celdas, char caracter);
int contar_celdas_libres(char *mapa, int total_celdas);
int calcular_puntaje(int monedas, int pasos, int niveles);

//Arreglo de niveles para seleccionar el nivel que se quiere jugar
const char *archivos_niveles[3] = {
    "Niveles/Nivel_1.txt",
    "Niveles/Nivel_2.txt",
    "Niveles/Nivel_3.txt"
};

char mapa[FILAS][COLS];

//Menu principal (seleccion de niveles)
int seleccionar_nivel() {
    int opcion;
    char buf[100];  //variable auxiliar para validar opciones
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
    //validacion de la opcion seleccionada
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
            mapa[i][j] = '#';   //valor por default por si el mapa esta incompleto, las casillas faltantes las llena con muro
        }
    }
    int fila = 0;
    char linea[256];
    while (fgets(linea, sizeof(linea), f) && fila < FILAS) {    //lee el archivo linea por linea
        int col = 0;
        //Recorre cada carácter de la linea que se acaba de leer y lo copia al mapa
        for (int j = 0; linea[j] != '\0' && linea[j] != '\n' && col < COLS; j++) {  //mientras no se llegue al final de la cadena o salto de linea y no pasa de 60 caracteres (columnas)
            mapa[fila][col] = linea[j];
            col++;
        }
        fila++;
    }
    fclose(f);
}


void imprimir_mapa(int recol, int total, int llaves, int jFila, int jCol, int pasos) {
    int tamCamara=20;
    int centro= tamCamara/2;

    int inicioFila = jFila -centro;
    int inicioCol= jCol -centro;

    if(inicioFila < 0) inicioFila=0;
    if(inicioCol < 0) inicioCol=0;
    if(inicioFila + tamCamara>FILAS)inicioFila =FILAS - tamCamara;
    if(inicioCol +tamCamara> COLS)inicioCol =COLS -tamCamara;

    char impMapa[20 * (20 * 2 + 1) + 1];  // guarda todo el mapa para imprimirlo, (COLS*2+1) es porque es el caracter del mapa mas el espacio mas el salto de linea
    int pos = 0;
    for (int i = inicioFila; i < inicioFila+tamCamara; i++) {
        for (int j = inicioCol; j < inicioCol+tamCamara; j++) {
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
            }else if (mapa[i][j] == 'D') {
                impMapa[pos++] = (char)176;  // puertas
            }else if (mapa[i][j] == 'E') {
                impMapa[pos++] = (char)245;  // salida
            }
            impMapa[pos++] = ' ';           // espacio para que el mapa no se vea alargado
        }
        impMapa[pos++] = '\n';
    }
    printf( "Monedas: %d / %d  |  Llaves: %d  |  Pasos: %d", recol, total, llaves, pasos);
    printf("\n");
    impMapa[pos] = '\0';    //terminar la cadena
    fputs(impMapa, stdout); //imprimir todo de una vez para mayor velocidad
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

//jugar al nivel seleccionado
void jugar_nivel(int num_nivel) {
    cargar_mapa(archivos_niveles[num_nivel - 1]);   //cargar el mapa del nivel seleccionado
    int celdasLib = contar_celdas_libres(&mapa[0][0], FILAS * COLS);
    printf("\nTotal de celdas libres: %d\n\n", celdasLib);
    int totalMonedas = contar_caracteres(&mapa[0][0], FILAS * COLS, 'M');
    printf("\nTotal de monedas: %d\n", totalMonedas);
    int jugador_fila, jugador_col;
    buscar_jugador(&jugador_fila, &jugador_col);

    int Mrecolectadas=0;
    int llaves = 0;            // Contador de llaves en inventario
    int pasos = 0;             // Contador de pasos realizados
    int nivel_completado = 0;  // Bandera para saber si cruzó la salida 'E'

    imprimir_mapa(Mrecolectadas,totalMonedas,llaves,jugador_fila,jugador_col, pasos);    //imprimir el mapa del nivel cargado
    int nueva_fila, nueva_col;
    char tecla;

    while (1) { 
        tecla = _getch();
        nueva_fila = jugador_fila;
        nueva_col  = jugador_col;
        if (tecla == 'w' || tecla == 'W') nueva_fila--;         //moverse hacia arriba
        else if (tecla == 's' || tecla == 'S') nueva_fila++;    //moverse hacia abajo
        else if (tecla == 'a' || tecla == 'A') nueva_col--;     //moverse hacia la izquierda
        else if (tecla == 'd' || tecla == 'D') nueva_col++;     //moverse hacia la derecha
        else if (tecla == 'q' || tecla == 'Q') break;           //salir del nivel (volver al menu principal)
        else continue;      //si se presiona otra tecla no hace nada
        // 1. Validar movimiento basico (que no sea pared '#')
        if (validar_movimiento(&mapa[0][0], COLS, nueva_fila, nueva_col)) {
            char destino = mapa[nueva_fila][nueva_col];
            int puede_pasar = 1;

            // Logica de la Puerta ('D')
            if (destino == 'D') {
                if (llaves > 0) {
                    llaves--;                   // Consumir una llave
                    mapa[nueva_fila][nueva_col] = '.'; // La puerta se abre permanentemente
                } else {
                    puede_pasar = 0;            // Bloquear paso si no hay llaves
                }
            }

            // Procesar movimiento si el paso es permitido
            if (puede_pasar) {
                pasos++;
                
                // Si pisa una Moneda
                if (destino == 'M') {
                    Mrecolectadas++;
                    mapa[nueva_fila][nueva_col] = '.';
                } 
                // Si pisa una Llave ('K')
                else if (destino == 'K') {
                    llaves++;
                    mapa[nueva_fila][nueva_col] = '.'; // Recoger llave
                } 
                // Si pisa la Salida ('E')
                else if (destino == 'E') {
                    nivel_completado = 1;
                    break; // Termina el nivel exitosamente
                }

                // Mover fisicamente al jugador en la matriz 
                movimiento(&mapa[0][0], COLS, jugador_fila, jugador_col, nueva_fila, nueva_col);
                jugador_fila = nueva_fila;
                jugador_col  = nueva_col;
                
                system("cls");
                imprimir_mapa(Mrecolectadas, totalMonedas, llaves, jugador_fila, jugador_col, pasos);
            }
        }
    }

    // Mostrar pantalla de fin de nivel con puntaje 
    system("cls");
    // Si completó el nivel pasamos el número del nivel, si se rindio con 'Q', pasa 0
    int puntaje_final = calcular_puntaje(Mrecolectadas, pasos, nivel_completado ? num_nivel : 0);
    
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
    printf("\nPresiona cualquier tecla para volver al menu principal...\n");
    _getch();
    system("cls");
    // logica del nivel
}

int main() {
    int opcion;
    while (1) {     //ciclo infinito
        opcion = seleccionar_nivel();
        if (opcion == 0) {  //salir del juego
            printf("Saliendo del juego.\n");
            break;
        }
        if (opcion < 1 || opcion > 3) { //opciones fuera del rango
            printf("Opcion invalida.\n");
            continue;
        }
        jugar_nivel(opcion);    //jugar al nivel seleccionado
    }
    return 0;
}