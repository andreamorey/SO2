//Aisha Gandarova, Andrea Morey Sanchez y Maria Orell Monserrat
#include "ficheros.h"

// Función que escribe el contenido de un buffer de memoria en un fichero o directorio
int mi_write_f(unsigned int ninodo, const void *buf_original,
unsigned int offset, unsigned int nbytes){
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
    int escritos = 0;
    // comprobamos los permisos del inodo

    if ((inodo.permisos & 2) != 2) {
        fprintf(stderr, "Error: no hay permiso de "
        "escritura\n"); 
        return escritos;
    }
    int primerBLogico = offset/BLOCKSIZE;
    int ultimoBLogico = (nbytes + offset - 1)/BLOCKSIZE; 
    char buf_bloque[BLOCKSIZE];
    int desp1 = offset%BLOCKSIZE;  //desplazamiento en el bloque para el offset
    int desp2 = (nbytes + offset - 1)%BLOCKSIZE; //miramos en el bloque donde llegan los nbytes escritos a partir del offse
    // si el primer y último bloque coinciden
    if (primerBLogico == ultimoBLogico){  
        int primerBFisico = traducir_bloque_inodo(ninodo,primerBLogico,1);
        if(bread(primerBFisico,buf_bloque) == -1){
            fprintf(stderr,"[mi_write_f-> no se ha podido leer el bloque]\n");
            return escritos;
        }
        // escribimos los nuevos datos preservando los que ya tenía
        memcpy(buf_bloque+desp1,buf_original,desp2+1-desp1); 
        if(bwrite(primerBFisico,buf_bloque) == -1){
            fprintf(stderr,"[mi_write_f-> no se han podido guardar el "
            "buffer en el bloque físico]\n");
            return escritos;
        }
        escritos = desp2+1-desp1;
        
    //caso en que la escritura afecta a más de un bloque
    }else{
        // primer bloque lógico, preservamos el contenido original
        int primerBFisico = traducir_bloque_inodo(ninodo,primerBLogico,1);
        bread(primerBFisico,buf_bloque);
        memcpy (buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
        bwrite(primerBFisico, buf_bloque);
        escritos = BLOCKSIZE - desp1;;
        // bloques lógicos intermedios, no hace falta preservar porque sobreesccribimos bloques completos
        for (int i = primerBLogico+1; i< ultimoBLogico;i++){
            int bfisico = traducir_bloque_inodo(ninodo,i,1);
            bwrite(bfisico,buf_original + (BLOCKSIZE - desp1) + (i - primerBLogico - 1) * BLOCKSIZE);
            escritos += BLOCKSIZE;
        }
        
        // último bloque lógico, preservamos como en el primero
        int ultimoBFisico = traducir_bloque_inodo(ninodo, ultimoBLogico, 1);
        bread(ultimoBFisico,buf_bloque);
        memcpy (buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
        bwrite(ultimoBFisico,buf_bloque);
        escritos += desp2 + 1;
    }
    // actualizamos la metainformación del inodo
    leer_inodo(ninodo,&inodo);
    if ((offset+nbytes) > inodo.tamEnBytesLog){
        
        inodo.tamEnBytesLog = offset+ nbytes;
        inodo.ctime = time(NULL); // se actualiza si modificamos campos del inodo
    }
    inodo.mtime = time(NULL); //actualizamos ya que hemos escrito en la zona de datos
    escribir_inodo(ninodo,inodo);
    //fprintf(stderr, "Bytes escritos en mi_write_f: %d\n", escritos);
   
    return escritos;
}

// Función que lee información de un fichero o directorio y la almacena en un buffer de memoria
int mi_read_f(unsigned int ninodo, void *buf_original,
  unsigned int offset, unsigned int nbytes){
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
    int leidos = 0;
    // comprobamos los permisos del inodo
    if ((inodo.permisos & 4) != 4) {
        fprintf(stderr, "No hay permisos de lectura\n"); 
        return leidos;
    }
    
     //No podemos leer más allá del tamaño en bytes lógicos del inodo
    if(offset >= inodo.tamEnBytesLog){
        return leidos;
    }
    //En este caso leemos solo los bytes desde el offset hasta EOF
    if((offset+nbytes) >= inodo.tamEnBytesLog) {
        nbytes = inodo.tamEnBytesLog-offset;
    }
    int primerBLogico = offset/BLOCKSIZE;
    int ultimoBLogico = (nbytes + offset - 1)/BLOCKSIZE;
    char buf_bloque[BLOCKSIZE];
    int desp1 = offset%BLOCKSIZE;   //desplazamiento en el bloque para el offset
    int desp2 = (nbytes + offset -1)%BLOCKSIZE; //miramos en el bloque donde llegan los nbytes que hay que leer a partir del offset
    int bfisico;
    // si el primer y último bloque coinciden
    if (primerBLogico == ultimoBLogico){
        bfisico = traducir_bloque_inodo(ninodo,primerBLogico,0);
        if (bfisico != -1){ // si el bloque físico existe
            bread(bfisico,buf_bloque);
            // nos interesan solo los datos a partir de desp1
            memcpy(buf_original,buf_bloque+desp1,nbytes);
        }
        leidos = desp2+1-desp1 ;
            
    //caso en que la lectura afecta a más de un bloque
    }else{
        // primer bloque lógico, copiamos solo los datos a partir de desp1
        bfisico = traducir_bloque_inodo(ninodo,primerBLogico,0);
        if (bfisico != -1){ // si el bloque físico existe
            if(bread(bfisico, buf_bloque) == -1){
                return leidos;
            }
            memcpy(buf_original,buf_bloque+desp1,BLOCKSIZE-desp1);
        }
        leidos = BLOCKSIZE-desp1;
        // bloques lógicos intermedios, los copiamos enteros
        for (int i = primerBLogico+1; i< ultimoBLogico;i++){
            bfisico = traducir_bloque_inodo(ninodo,i,0);
            if (bfisico != -1){ // si el bloque físico existe
                if(bread(bfisico,buf_bloque) < 0){
                    return leidos;
                }
        memcpy(buf_original+(BLOCKSIZE-desp1)+(i-primerBLogico-1)*BLOCKSIZE,buf_bloque,BLOCKSIZE);
    
            }
            leidos += BLOCKSIZE;
        }
        // último bloque lógico, copiamos solo hasta desp2
        bfisico = traducir_bloque_inodo(ninodo, ultimoBLogico,0);
        if (bfisico != -1){ // si el bloque físico existe
            if(bread(bfisico,buf_bloque) == -1){
                return leidos;
            }
            memcpy(buf_original +(nbytes-desp2-1),buf_bloque,desp2 + 1);
        }
        leidos += desp2+1;
    }
    // actualizamos la metainformación del inodo
    leer_inodo(ninodo,&inodo);
    inodo.atime = time(NULL);   //actualizamos ya que hemos accedido a la zona de datos
    escribir_inodo(ninodo,inodo);
    return leidos;
}

// Función que devuelve la metainformación de un fichero o directorio
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    p_stat->atime = inodo.atime;
    p_stat->ctime = inodo.ctime;
    p_stat->mtime = inodo.mtime;
    p_stat->nlinks = inodo.nlinks;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
    p_stat->permisos = inodo.permisos;
    p_stat->tipo = inodo.tipo;
    p_stat->tamEnBytesLog= inodo.tamEnBytesLog;
    return 0;
}

// Función que cambia los permisos de un fichero o directorio
int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
    inodo.permisos = permisos;
    inodo.ctime = time(NULL);
    escribir_inodo(ninodo,inodo);
    return 0;
}

// Función que trunca un fichero o directorio a los bytes indicados
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
    // comprobamos los permisos del inodo
    if ((inodo.permisos & 2) != 2) {
        fprintf(stderr, "Error: no hay permiso de "
        "escritura %s\n", strerror(errno)); 
        return -1;
    }
    // no se puede truncar más allá del tamaño en bytes lógicos del fichero o directorio
    if (nbytes >= inodo.tamEnBytesLog){
        fprintf(stderr, "Error: No se puede truncar mas allá del fichero "
        " %s\n", strerror(errno)); 
        return -1;
    }
    int nblogico;
    // Calculamos el primer bloque lógico a liberar
    if((nbytes % BLOCKSIZE) == 0){
        nblogico= nbytes/BLOCKSIZE;
    }else{
        nblogico = nbytes/BLOCKSIZE +1;
    }
    int bloquesLiberados = liberar_bloques_inodo(nblogico,&inodo);
    // actualizamos la metainformación del inodo
    inodo.ctime = time(NULL);
    inodo.mtime = time(NULL);
    inodo.tamEnBytesLog = nbytes;
    inodo.numBloquesOcupados -= bloquesLiberados;
    escribir_inodo(ninodo,inodo);
    return bloquesLiberados;
}
