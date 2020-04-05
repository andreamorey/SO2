#include "ficheros.h"

int main(int argc, char **argv){
    if (argc < 3){
        printf("sinatxis: permitir <nombre_dispositivo> <ninodo> <permisos>\n");
        return EXIT_SUCCESS;
    }
    if (bmount(argv[1])==-1){
        //error al abrir el fichero
        return EXIT_FAILURE;
    }

    mi_chmod_f(atoi(argv[2]), atoi(argv[3]));

    if(bumount()==-1){
        //error
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}