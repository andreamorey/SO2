#define TAMBUFFER 1500
#include "ficheros.h"

//Recorido secuencial de todos los bytes lógicos del inodo mostrando su contenido.
int main(int argc,char **argv){
    char *nombre = argv[1];
    if (bmount(nombre) == -1){
        return EXIT_FAILURE;
    }
    int ninodo = atoi(argv[2]);
    char buffer_texto[TAMBUFFER];
    long int leidos, total_leidos = 0;
    int offset = 0;

    memset(buffer_texto,0,TAMBUFFER);
    leidos = mi_read_f(ninodo, buffer_texto, offset, TAMBUFFER);
    while(leidos > 0){
        write(1,buffer_texto, leidos);
        total_leidos = total_leidos + leidos;
        //fprintf(stderr, "acumulados %ld\n",total_leidos);
        offset += TAMBUFFER;
      //  fprintf(stderr, "offset = %d\t",offset);
        memset(buffer_texto, 0 , TAMBUFFER);
        leidos  = mi_read_f(ninodo, buffer_texto, offset, TAMBUFFER);
        //fprintf(stderr, "leidos %ld\t",leidos);
        
    }
    fprintf(stderr, "\ntotal leidos %ld\n",total_leidos);
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
/*     if(total_leidos != inodo.tamEnBytesLog){
        return EXIT_FAILURE;
    }
    write(1, buffer_texto, total_leidos); */
    char string [128];
/*     sprintf(string,"bytes leidos %ld\n",total_leidos);
    write(2, string, strlen(string)); */
    sprintf(string,"tamaño en bytes log = %d\n", inodo.tamEnBytesLog);
    write(2, string,strlen(string));
    bumount();
    return EXIT_SUCCESS;
}
