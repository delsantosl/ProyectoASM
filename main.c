#include <stdio.h>
#include <stdlib.h>


const char *archivos_niveles[3] = {
    "Nivel_1.txt",
    "Nivel_2.txt",
    "Nivel_3.txt"
};

int seleccionar_nivel() {
    int opcion;
    printf("=================================\n");
    printf("   BitQuest: Explorador de Matrices\n");
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

int main() {
    int opcion = seleccionar_nivel();

    if (opcion == 0) {
        printf("Saliendo del juego.\n");
        return 0;
    }

    if (opcion < 1 || opcion > 3) {
        printf("Opcion invalida.\n");
        return 1;
    }

    printf("\nNivel %d seleccionado.\n", opcion);
    printf("Archivo: %s\n", archivos_niveles[opcion - 1]);

    return 0;
}