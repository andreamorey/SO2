//Aisha Gandarova, Andrea Morey Sanchez y Maria Orell Monserrat
#include "ficheros.h"


int main(int argc,char **argv){
    if(argc != 4){ // Validación de sintaxis
        fprintf(stderr,"./truncar <nombre_dispositivo> <ninodo> <nbytes>\n");
    }
    struct inodo inodo;
    if(bmount(argv[1]) == -1){ // montamos el dispositivo
        fprintf(stderr,"No se ha podido montamor el dispositivo virtual\n");
        return -1;
    }
    unsigned int ninodo = atoi(argv[2]);
    int nbytes = atoi(argv[3]);
    if(nbytes == 0 ){
        liberar_inodo(ninodo);
    }else{
        mi_truncar_f(ninodo,nbytes);  // truncamos el fichero a los bytes indicados
    }
    leer_inodo(ninodo,&inodo);
    if (inodo.tamEnBytesLog != nbytes){  
        fprintf(stderr,"los bytes logicos no se corresponden a nbytes\n");
        return -1;
    }
    // Imprimimos la información del inodo:
    fprintf(stderr,"\nDATOS INODO %d\n",ninodo);
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
    if (bumount() == -1){   // desmonatmoos el dispositivo virtual
        fprintf(stderr,"No se ha podido desmontar el dispositivo virtual\n");
        return -1;
    }
    return 0;
}