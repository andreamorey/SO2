//Aisha Gandarova, Andrea Morey Sanchez y Maria Orell Monserrat

#include "bloques.h"
#include "semaforo_mutex_posix.h"

static sem_t *mutex;
static unsigned int inside_sc = 0;
static int descriptor = 0;

/*
USO FUNCIÓN: se encarga de obtener el descriptor del fichero que
se usará como dispositivo virtual.
PARÁMETROS DE ENTRADA: nombre del fichero
VALORES DE RETORNO: 0(éxito) o 1(fracaso) 
*/

int bmount(const char *camino) {
   if (descriptor > 0) {
       close(descriptor);
   }
   if ((descriptor = open(camino, O_RDWR | O_CREAT, 0666)) == -1) {
      fprintf(stderr, "Error: bloques.c → bmount() → open()\n");
   }
   if (!mutex) { //mutex == 0
   //el semáforo es único y sólo se ha de inicializar una vez en nuestro sistema (lo hace el padre)
       mutex = initSem(); //lo inicializa a 1
       if (mutex == SEM_FAILED) {
           return -1;
       }
   }
   return descriptor;
}


/*
USO FUNCIÓN: libera el descriptor de ficheros a través de close.
PARÁMETROS DE ENTRADA: -
VALORES DE RETORNO: 0(éxito) o 1(fracaso) 
*/
int bumount() {
   descriptor = close(descriptor); 
   // hay que asignar el resultado de la operación a la variable ya que bmount() la utiliza
   if (descriptor == -1) {
       fprintf(stderr, "Error: bloques.c → bumount() → close(): %d: %s\n", errno, strerror(errno));
       return -1;
   }
   deleteSem(); // borramos semaforo 
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


void mi_waitSem(){
    if (!inside_sc) {
        waitSem(mutex);
    }
    inside_sc++;
}

void mi_signalSem() {
    inside_sc--;
    if (!inside_sc) {
        signalSem(mutex);
    }
}


