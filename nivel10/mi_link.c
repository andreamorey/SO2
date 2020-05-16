//Aisha Gandarova, Andrea Morey Sanchez y Maria Orell Monserrat
#include "directorios.h"

int main(int argc, char **argv){
    if (argc != 4){   // Validación de sintaxis
        fprintf(stderr, "Sintaxis: ./mi_link disco /ruta_fichero_original /ruta_enlace\n");
        return 0;
    }

    if (bmount(argv[1]) ==-1){ // si se ha abierto bien el fichero
        //error
        return -1;
    }

    if(argv[2][strlen(argv[2])-1] == '/'){
        fprintf(stderr, RED"La ruta original no es un fichero\n"RESET);
        return -1;
    }
    if(argv[3][strlen(argv[3])-1] == '/'){
        fprintf(stderr, RED"La ruta de enlace no es un fichero\n"RESET);
        return -1;
    }
    mi_link(argv[2], argv[3]);

    if (bumount() == -1){
        //error no se ha cerrado el disco
        return -1;
    }

}