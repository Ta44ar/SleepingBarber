#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "argsFunctions.h"

int numChairs = -1;
int debugMode = 0;

void printHelp() {
    printf("Użycie programu:\n");
    printf("./main [liczba_krzeseł] [-debug]\n");
    printf("\n");
    printf("Parametry:\n");
    printf("  liczba_krzeseł - określa ilość krzeseł dostępnych u fryzjera\n");
    printf("  -debug - opcjonalny parametr włączający tryb debugowania\n");
}

void argsHandler(int argc, char *argv[]){
    if (argc < 2 || argc > 3) {
        printf("Nieprawidłowa liczba argumentów!\n\n");
        printHelp();
        exit(-1);
    }

    numChairs = atoi(argv[1]);

    if (numChairs <= 0) {
        printf("Nieprawidłowa liczba krzeseł!\n\n");
        printHelp();
        exit(-1);
    }

    if (argc == 3 && strcmp(argv[2], "-debug") == 0) {
        debugMode = 1;
    }

}