//Aisha Gandarova, Andrea Morey Sanchez y Maria Orell Monserrat
#include "ficheros.h"

int main(int argc, char **argv){
    if (argc < 3){  // Validación de sintaxis
        printf("sinatxis: ./permitir <nombre_dispositivo> <ninodo> <permisos>\n");
        return 0;
    }
    if (bmount(argv[1])==-1){ // montamos el dispositivo virtual
        fprintf(stderr,"No se ha podido montamor el dispositivo virtual\n");
        return -1;
    }

    mi_chmod_f(atoi(argv[2]), atoi(argv[3])); // cambiamos los permisos  por los permisos pasados por parámetro
    if(bumount()==-1){ // desmontamos el dispositivo virtual
        fprintf(stderr,"No se ha podido desmontar el dispositivo virtual\n");
        return -1;
    }
    return 0;
}