#include "directorios.h"

int main(int argc, char **argv) {
    if (argc != 4){   // Validación de sintaxis
        fprintf(stderr, "Sintaxis: ./mi_mkdir <disco> <permisos> </ruta>\n");
        return 0;
    }


    if(argv[3][strlen(argv[3])-1] != '/'){
        fprintf(stderr, RED"La ruta introducida no es un directorio\n"RESET);
        return -1;
    }

    bmount(argv[1]);
    int permisos = atoi(argv[2]);
    if( permisos > 7 || permisos < 0){
        fprintf(stderr, RED"Error:  modo inválido: <<%d>>\n"RESET, permisos);
        return -1;
    }
    mi_creat(argv[3], permisos);
    bumount();
}