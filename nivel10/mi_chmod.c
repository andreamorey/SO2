//Aisha Gandarova, Andrea Morey Sanchez y Maria Orell Monserrat
#include "directorios.h"

int main(int argc, char **argv){
    if (argc != 4){  // Validación de sintaxis
        fprintf(stderr, "Sintaxis: ./mi_chmod <disco> <permisos> </ruta>\n");
        return 0;
    }
    int permisos = atoi(argv[2]);
    if(permisos > 7 || permisos < 0){ // comprobamos los permisos
        fprintf(stderr, RED"Error:  modo inválido: <<%d>>\n"RESET, permisos);
        return -1;
    }
    bmount(argv[1]);
    mi_chmod(argv[3],permisos); // le pasamos la ruta y los permisos
    bumount();
}