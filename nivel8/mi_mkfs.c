#include "ficheros_basico.h"

int main(int argc, char**argv){
    if (argc != 3){
        printf("Introduzca bien los parámetros:"
        " ./mi_mkfs <nombre_dispositivo> <nbloques>\n");
        return EXIT_FAILURE;
    }
    char* nombre_dispositivo = argv[1];
    int nbloques = atoi(argv[2]);
    // abre o crea el fichero 
    if(bmount(nombre_dispositivo) == EXIT_FAILURE){   
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    int ninodos=nbloques/4;  
    //inicializamos los metadatos
    initSB(nbloques, ninodos);    
    initMB(); 
    initAI();
    reservar_inodo('d',7); 
    //cierra el descriptor de fichero
    if(bumount() == EXIT_FAILURE){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
}