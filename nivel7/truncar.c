#include "ficheros.h"


int main(int argc,char **argv){
    if(argc != 4){
        fprintf(stderr,"./truncar <nombre_dispositivo> <ninodo> <nbytes>\n");
    }
    struct inodo inodo;
    bmount(argv[1]);// montamos el dispositivo
    unsigned int ninodo = atoi(argv[2]);
    int nbytes = atoi(argv[3]);
    if(nbytes == 0 ){
        liberar_inodo(ninodo);
    }else{
        mi_truncar_f(ninodo,nbytes);
    }
    leer_inodo(ninodo,&inodo);
    if (inodo.tamEnBytesLog != nbytes){
        //error
        fprintf(stderr,"los bytes logicos no se corresponden a nbytes\n");
        return -1;
    }
    fprintf(stderr,"DATOS INODO %d\n",ninodo);
        fprintf(stderr,"tipo: %c\n", inodo.tipo);
        fprintf(stderr,"permisos: %i\n", inodo.permisos);
        struct tm *ts;
        char atime[80];
        char mtime[80];
        char ctime[80];
        ts = localtime(&inodo.atime);
        strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&inodo.mtime);
        strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&inodo.ctime);
        strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
        fprintf(stderr,"atime: %s \nmtime: %s \nctime: %s\n",atime,mtime,ctime);
        fprintf(stderr,"nlinks: %i\n", inodo.nlinks);
        fprintf(stderr,"tamEnBytesLog: %i\n", inodo.tamEnBytesLog);
        fprintf(stderr,"numBloquesOcupados: %i\n", inodo.numBloquesOcupados);

    return EXIT_SUCCESS;
}