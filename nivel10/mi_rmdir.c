#include "directorios.h"

int main(int argc, char **argv){
    if (argc != 3){   // Validación de sintaxis
        fprintf(stderr, "Sintaxis: ./mi_rm disco /ruta\n");
        return 0;
    }

    if (bmount(argv[1]) ==-1){ // si se ha abierto bien el fichero
        //error
        return -1;
    }



    if(argv[2][strlen(argv[2])-1] != '/'){
        fprintf(stderr, RED"La ruta original no es un directorio\n"RESET);
        return -1;
    }
    
    mi_unlink(argv[2]);

    if (bumount() == -1){
        //error no se ha cerrado el disco
        return -1;
    }

}