#include "bloques.h"

static int descriptor = 0;

/*
USO FUNCIÓN: se encarga de obtener el descriptor del fichero que
se usará como dispositivo virtual.
PARÁMETROS DE ENTRADA: nombre del fichero
VALORES DE RETORNO: 0(éxito) o 1(fracaso) 
*/
int bmount(const char *camino){
    umask(000);
    descriptor = open(camino,O_RDWR|O_CREAT,0666);
    if (descriptor == -1){
        return -1;
    }
    return 0;
}


/*
USO FUNCIÓN: libera el descriptor de ficheros a través de close.
PARÁMETROS DE ENTRADA: -
VALORES DE RETORNO: 0(éxito) o 1(fracaso) 
*/
int bumount(){
    if (close(descriptor) == -1) {
        return -1;
    }
    return 0;
}


/*
USO FUNCIÓN: vuelca el contenido de un buffer de memoria en
el bloque virtual especificado por el parámetro de entrada.
PARÁMETROS DE ENTRADA: el número de bloque virtual que toca
escribir y un buffer (con los datos a escribir)
VALORES DE RETORNO: número de bytes escritos(éxito) o -1(fracaso) 
*/
int bwrite(unsigned int nbloque, const void *buf){
    lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
    return write(descriptor, buf, BLOCKSIZE);
     // devolvemos bytes leidos o -1 si error
}

/*
USO FUNCIÓN: lee del dispositivo virtual el bloque especificado
por el parámetreo de entrada y copia su contenido en el buffer
VALORES DE RETORNO: número de bytes leídos(éxito) o -1(fracaso) 
*/
int bread(unsigned int nbloque, void *buf){
    lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
    return read(descriptor, buf, BLOCKSIZE);
}