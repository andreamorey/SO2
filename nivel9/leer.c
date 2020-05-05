#define TAMBUFFER 1500
#include "ficheros.h"

// Recorido secuencial de todos los bytes lógicos del inodo mostrando su contenido.
int main(int argc,char **argv){
    if (argc < 3){   // Validación de sintaxis
        fprintf(stderr, "sinatxis: ./leer <nombre_dispositivo> <ninodo>\n");
        return 0;
    }

    //Le pasaremos por comandos un nº de inodo y el nombre del dispositivo
    char *nombre = argv[1];
    if (bmount(nombre) == -1){
        return -1;
    }
    int ninodo = atoi(argv[2]);
    char buffer_texto[TAMBUFFER];
    long int leidos, total_leidos = 0;
    int offset = 0;
    //para no leer todo el fichero de golpe, iremos haciendo llamadas a mi_read_f()
    //con el buffer_texto para ir haciendo las lecturas
    memset(buffer_texto,0,TAMBUFFER);
    leidos = mi_read_f(ninodo, buffer_texto, offset, TAMBUFFER);
    while(leidos > 0){ // leemos todo el disco
        write(1,buffer_texto, leidos);
        total_leidos = total_leidos + leidos;
        offset += TAMBUFFER;
        memset(buffer_texto, 0 , TAMBUFFER);
        leidos  = mi_read_f(ninodo, buffer_texto, offset, TAMBUFFER);
    }
    fprintf(stderr, "\ntotal leidos: %ld\n",total_leidos);
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
    
    if(total_leidos != inodo.tamEnBytesLog){
        bumount();
        return -1;
    }
    fprintf(stderr, "tamaño en Bytes Lógico  del inodo: %d\n",inodo.tamEnBytesLog);
    bumount();
    return 0;
}
