#include "directorios.h"

int main(int argc, char **argv){
    if (argc != 3){  // Validación de sintaxis
        fprintf(stderr, "Sintaxis: ./mi_ls <disco> </ruta_directorio>\n");
        return 0;
    }
    char buffer[10000];
    bmount(argv[1]);
    int entradas = mi_dir(argv[2],buffer);
    
    if(entradas > 0){
        fprintf(stderr, LILA"Tipo\tPermisos\tmTime\t\t\tTamaño\tNombre"RESET);
        fprintf(stderr, "\n-----------------------------------------------------------------\n");
        for (int i = 0; i < strlen(buffer); i++){
            if(buffer[i] != '|'){
                fprintf(stderr,"%c",buffer[i]);
            } else{
                fprintf(stderr, "\n");
            }
        }
    }
    bumount();
}