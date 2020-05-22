//Aisha Gandarova, Andrea Morey Sanchez y Maria Orell Monserrat
#include "directorios.h"

int main(int argc, char **argv){
    if (argc != 3){  // Validación de sintaxis
        fprintf(stderr, "Sintaxis: ./mi_stat <disco> </ruta>\n");
        return 0;
    }
    struct STAT p_stat;
    
    bmount(argv[1]);
    int ninodo = mi_stat(argv[2],&p_stat);
    
    fprintf(stderr, "Nº de inodo: %d\n", ninodo);
    
    fprintf(stderr,"tipo: %c\n",  p_stat.tipo);
    fprintf(stderr,"permisos: %d\n", p_stat.permisos);
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];
    ts = localtime(&p_stat.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    
    fprintf(stderr,"atime: %s \nctime: %s \nmtime: %s\n",atime,ctime,mtime);
    fprintf(stderr,"nlinks: %i\n", p_stat.nlinks);
    fprintf(stderr,"tamEnBytesLog: %i\n", p_stat.tamEnBytesLog);
    fprintf(stderr,"numBloquesOcupados: %i\n", p_stat.numBloquesOcupados);  
    bumount();
}