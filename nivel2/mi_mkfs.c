#include <stdio.h>  //printf(), fprintf(), stderr, stdout, stdin
#include <fcntl.h> //O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h> //S_IRUSR, S_IWUSR
#include <stdlib.h>  //exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h> // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>  //errno
#include <string.h> // strerror()
#include "ficheros_basicos.h"

int main(int argc, char**argv){
    if (argc != 3){
        printf("Introduce bien los datos pinche pendejo\n");
        return EXIT_FAILURE;
    }
    char* nombre_dispositivo = argv[1];
    int nbloques = atoi(argv[2]);
    bmount(nombre_dispositivo);   //abre el fichero o lo crea
    //unsigned char* buf;
    //buf = malloc(nbloques*BLOCKSIZE);  //guarda espacio para el superbloque
    int ninodos=nbloques/4;   //
    initSB(nbloques, ninodos);    
    initMB(); 
    initAI();
   // for (int i = 0; i< nbloques; i++){   // nivel 1
   //     memset(buf,0,BLOCKSIZE);
   //     bwrite(i,buf);
   //}
    bumount();
    printf("mi_mkfs ejecutado correctamente (ª~ª)\n");
}