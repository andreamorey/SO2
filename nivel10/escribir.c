#define NUMOFFSETS 5
#include "ficheros.h"


int main(int argc,char **argv){
    int numInodos = atoi(argv[3]); //con la función atoi convertimos el string en un valor int
    int offset[NUMOFFSETS] = {9000, 209000, 30725000, 409605000, 480000000};
    int arrayInodos[NUMOFFSETS];
    if (argc != 4 ){
        printf("Sintaxis: escribir <nombre_dispositivo>" 
           " <'$(cat fichero)'> <diferentes_inodos> \n");
        printf("Offsets: 9000, 209000, 30725000, 409605000, 480000000\n");
        printf("Si diferentes_inodos=0 se reserva un solo inodo para todos los offsets\n");
        return EXIT_SUCCESS;
    }
    bmount(argv[1]);
    int longitud = strlen(argv[2]); //obtenemos el texto y su longitud
    fprintf(stderr,"Longitud del texto: %d\n",longitud);
    int ninodo;

    if (numInodos == 0){
        ninodo = reservar_inodo('f',6);
        for (int i =0; i < NUMOFFSETS; i++){ //para un mismo inodo, rellenamos
            //el arrayInodos con todos los diferentes OFFSETS
            arrayInodos[i] = ninodo;
        }
    }else if (numInodos == 1){
        for (int i =0; i < NUMOFFSETS; i++){
            //vamos reservando un inodo diferente con cada uno de los OFFSETS
            ninodo = reservar_inodo('f',6);
            arrayInodos[i] = ninodo;
        }
    }
    char buf_original[4000];
    strcpy(buf_original, argv[2]);
    //Vamos recorriendo el arrayInodos, ya sea el que tiene el mismo inodo
    //repetido o el formado por inodos diferentes, e imprimimos sus datos.
    for(int i=0; i< NUMOFFSETS; i++){
        fprintf(stderr,"\nNº inodo reservado: %d\n", arrayInodos[i]);
        fprintf(stderr,"offset: %d\n",offset[i]);
        int nbytes = (int) strlen(argv[2]);        
        mi_write_f(arrayInodos[i],buf_original,offset[i],nbytes);
        fprintf(stderr,"Bytes escritos: %d\n",nbytes);
        memset(buf_original,0,4000);
        mi_read_f(arrayInodos[i],buf_original,offset[i],nbytes);
        //write(1, buf_original,nbytes);
        fprintf(stderr,"\nDATOS INODO %d\n\n",arrayInodos[i]);
        struct inodo inodoReservado;
        leer_inodo(arrayInodos[i], &inodoReservado);
        fprintf(stderr,"tipo: %c\n", inodoReservado.tipo);
        fprintf(stderr,"permisos: %i\n", inodoReservado.permisos);
        struct tm *ts;
        char atime[80];
        char mtime[80];
        char ctime[80];
        ts = localtime(&inodoReservado.atime);
        strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&inodoReservado.mtime);
        strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&inodoReservado.ctime);
        strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
        fprintf(stderr,"atime: %s \nmtime: %s \nctime: %s\n",atime,mtime,ctime);
        fprintf(stderr,"nlinks: %i\n", inodoReservado.nlinks);
        fprintf(stderr,"tamEnBytesLog: %i\n", inodoReservado.tamEnBytesLog);
        fprintf(stderr,"numBloquesOcupados: %i\n", inodoReservado.numBloquesOcupados);
        printf("*******************************************************\n");
    }
    bumount(); 
}

