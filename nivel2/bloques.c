#include "bloques.h"

static int descriptor = 0;

/*
USO FUNCIÓN: se encarga de obtener el descriptor del fichero que
se usará como dispositivo virtual.
PARÁMETROS DE ENTRADA: puntero al nombre del fichero ¿?¿?¿?¿?¿?¿?¿?¿?¿?
VALORES DE RETORNO: 0(éxito) o 1(fracaso) 
FUNCIONES A LAS QUE LLAMA: OPEN, umask etc¿?¿?¿?
FUNCIONES QUE LA LLAMAN:
*/
int bmount(const char *camino){
    umask(000);
    descriptor = open(camino,O_RDWR|O_CREAT,0666); // NOS FALTA EL MODO
    if (descriptor == -1){
        //ERROR
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


/*
USO FUNCIÓN: libera el descriptor de ficheros a través de close.
PARÁMETROS DE ENTRADA: -
VALORES DE RETORNO: 0(éxito) o 1(fracaso) 
FUNCIONES A LAS QUE LLAMA: close
FUNCIONES QUE LA LLAMAN:
*/
int bumount(){
    if (close(descriptor) == -1) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


/*
USO FUNCIÓN: vuelca el contenido de un buffer de memoria en
el bloque virtual especificado por el parámetro de entrada.
PARÁMETROS DE ENTRADA: el número de bloque virtual que toca
escribir y el puntero del buffer ¿?¿?¿??
VALORES DE RETORNO: número de bytes escritos(éxito) o -1(fracaso) 
FUNCIONES A LAS QUE LLAMA: write, lseek
FUNCIONES QUE LA LLAMAN:
*/
int bwrite(unsigned int nbloque, const void *buf){
    lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
    size_t wrbyte = write(descriptor, buf, BLOCKSIZE);
    if (wrbyte == -1){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        //return EXIT_FAILURE;¿?¿?¿?¿?¿?
    }
    return wrbyte;
}

/*
USO FUNCIÓN: lee del dispositivo virtual el bloque especificado
por el parámetreo de entrada y copia su contenido en el buffer
de  memoria.
VALORES DE RETORNO: número de bytes leídos(éxito) o -1(fracaso) 
FUNCIONES A LAS QUE LLAMA: read, lseek
FUNCIONES QUE LA LLAMAN:
*/
int bread(unsigned int nbloque, void *buf){
    lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
    size_t rbyte = read(descriptor, buf, BLOCKSIZE);
    if (rbyte == -1){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        //return EXIT_FAILURE;¿?¿?¿?¿?
    }
    return rbyte;
}
