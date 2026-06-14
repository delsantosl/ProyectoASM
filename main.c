#include <stdio.h>
#include <stdlib.h>

#define FILAS 60
#define COLS  60

void movimiento(char *matriz, int columnas, int fila1, int columna1, int fila2, int columna2);
int validar_movimiento(char *mapa, int cols, int fila, int col);
int contar_caracteres(char *mapa, int total_celdas, char caracter);
int contar_celdas_libres(char *mapa, int total_celdas);

const char *archivos_niveles[3] = {
    "Niveles/Nivel_1.txt",
    "Niveles/Nivel_2.txt",
    "Niveles/Nivel_3.txt"
};

char mapa[FILAS][COLS];

int seleccionar_nivel() {
    int opcion;
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
    scanf("%d", &opcion);
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
            mapa[i][j] = '#';
        }
    }

    int fila = 0;
    char linea[256];
    while (fgets(linea, sizeof(linea), f) && fila < FILAS) {
        int col = 0;
        for (int j = 0; linea[j] != '\0' && linea[j] != '\n' && col < COLS; j++) {
            mapa[fila][col] = linea[j];
            col++;
        }
        fila++;
    }
    fclose(f);
}

void imprimir_mapa() {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c ", mapa[i][j]);
        }
        printf("%c", '\n');
    }
}

void jugar_nivel(int num_nivel) {
    cargar_mapa(archivos_niveles[num_nivel - 1]);
    int celdasLib = contar_celdas_libres(&mapa[0][0], FILAS * COLS);
    printf("\nTotal de celdas libres: %d\n\n", celdasLib);
    imprimir_mapa();
    int totalMonedas = contar_caracteres(&mapa[0][0], FILAS * COLS, 'M');
    printf("\nTotal de monedas: %d\n", totalMonedas);
    // logica del nivel
}

int main() {
    int opcion;
    while (1) {
        opcion = seleccionar_nivel();
        if (opcion == 0) {
            printf("Saliendo del juego.\n");
            break;
        }
        if (opcion < 1 || opcion > 3) {
            printf("Opcion invalida.\n");
            continue;
        }
        jugar_nivel(opcion);
    }

    return 0;
}