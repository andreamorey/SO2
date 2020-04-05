#include <stdio.h>  //printf(), fprintf(), stderr, stdout, stdin
#include <fcntl.h> //O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h> //S_IRUSR, S_IWUSR
#include <stdlib.h>  //exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h> // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>  //errno
#include <string.h> // strerror()
#include "directorios.h"

int main(int argc, char**argv){
    if (argc != 3){
        printf("Introduzca bien los datos\n");
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