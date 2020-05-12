#include "directorios.h"

int main(int argc, char **argv){
    if (argc != 5){  // Validación de sintaxis
        fprintf(stderr, "Sintaxis: ./mi_escribir <disco> </ruta_fichero> <texto> <offset>\n");
        return 0;
    }

    bmount(argv[1]);

    char *camino = argv[2];
    int offset = atoi(argv[4]);

    if(camino[strlen(camino)-1]=='/') { //si es un directorio 
        fprintf(stderr, "La ruta introducida no es un fichero\n");
        return -1;
    }
    fprintf(stderr, "Longitud texto: %ld\n", strlen(argv[3]));
    
    int totalEscritos = mi_write(camino, argv[3], offset, strlen(argv[3]));
    fprintf(stderr, "Bytes escritos: %d\n", totalEscritos);
    if(totalEscritos == -1){
        return -1;
    }
    
    
    bumount(); 
    return 0;
}