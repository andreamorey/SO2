//Aisha Gandarova, Andrea Morey Sanchez y Maria Orell Monserrat
#include "directorios.h"

int main(int argc, char **argv){
    if (argc != 3){ // Validación de sintaxis
        fprintf(stderr, "Sintaxis: ./mi_cat <disco> </ruta_fichero>\n");
        return 0;
    }

    char *camino = argv[2];
    const int TAMBUFFER = BLOCKSIZE*4;
    int offset = 0 ;
    if(camino[strlen(camino)-1]=='/') { //es un directorio
        fprintf(stderr, "La ruta introducida no es un fichero\n");
        return -1;
    }


    //Le pasaremos por comandos un nº de inodo y el nombre del dispositivo
    
    if (bmount(argv[1]) == -1){
        // error no se ha podido abrir el descriptor del fichero
        return -1;
    }

    char buffer_texto[TAMBUFFER];
    long int leidos, total_leidos = 0;

    //para no leer todo el fichero de golpe, iremos haciendo llamadas a mi_read()
    //con el buffer_texto para ir haciendo las lecturas
    memset(buffer_texto,0,TAMBUFFER);
    leidos = mi_read(camino, buffer_texto, offset, TAMBUFFER);
    while(leidos > 0){ // leemos todo el disco
        write(1,buffer_texto, leidos);
        total_leidos = total_leidos + leidos;
        offset += TAMBUFFER;
        memset(buffer_texto, 0 , TAMBUFFER);
        leidos  = mi_read(camino, buffer_texto, offset, TAMBUFFER);
    }

    struct inodo inodo;
    leer_inodo(ultimaEntradaLectura.p_inodo,&inodo);
    if (total_leidos != inodo.tamEnBytesLog){
        fprintf(stderr, "\nNo se ha podido leer todo el fichero.\n");
    }else{
        fprintf(stderr, "\ntotal leidos: %ld\n",total_leidos);
    }
   
    if( bumount() == -1){
        //error no se ha podido cerrar lo que sea
        return -1;
    }
    return 0;
}

