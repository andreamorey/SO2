#include "ficheros.h"

int mi_write_f(unsigned int ninodo, const void *buf_original,
unsigned int offset, unsigned int nbytes){
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
    if ((inodo.permisos & 2) != 2) {
        fprintf(stderr, "Error: no hay permiso de "
        "escritura %s\n", strerror(errno)); 
        return -1;
    }
    int primerBLogico = offset/BLOCKSIZE;  
    int ultimoBLogico = (nbytes + offset - 1)/BLOCKSIZE; 
    char buf_bloque[BLOCKSIZE];
    int desp1 = offset%BLOCKSIZE;
    int desp2 = (nbytes + offset - 1)%BLOCKSIZE; 
    int escritos = 0;
    // si el primero  y segundo coinciden
    if (primerBLogico == ultimoBLogico){  
        int primerBFisico = traducir_bloque_inodo(ninodo,primerBLogico,1);
        if(bread(primerBFisico,buf_bloque) == -1){
         //   fprintf(stderr,"Error no se ha podido leer los datos"
           //     ": %s\n", strerror(errno));
            return -1;
        }  
        // info que no queremos que se solape:
        memcpy(buf_bloque+desp1,buf_original,desp2+1-desp1); 
        if(bwrite(primerBFisico,buf_bloque) == -1){
          //  fprintf(stderr,"Error no se han podido guardar los "
           // "inodos: %s\n", strerror(errno));
            return -1;
        }
        escritos = desp2+1-desp1;
    }else{ // primer bloque:
        int primerBFisico = traducir_bloque_inodo(ninodo,primerBLogico,1);
        bread(primerBFisico,buf_bloque);
        memcpy (buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
        bwrite(primerBFisico, buf_bloque);
        escritos = BLOCKSIZE - desp1;;
        // bloques intermedios
        for (int i = primerBLogico+1; i< ultimoBLogico;i++){
            int bfisico = traducir_bloque_inodo(ninodo,i,1);
            bwrite(bfisico,buf_original + (BLOCKSIZE - desp1) + (i - primerBLogico - 1) * BLOCKSIZE);
        }
        escritos += BLOCKSIZE;
        // último bloque
        int ultimoBFisico = traducir_bloque_inodo(ninodo, ultimoBLogico, 1);
        bread(ultimoBFisico,buf_bloque);
        memcpy (buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);
        bwrite(ultimoBFisico,buf_bloque);
        escritos += desp2 + 1;
    }
    leer_inodo(ninodo,&inodo);
    //inodo.ctime = ;
    if ((offset+nbytes) > inodo.tamEnBytesLog){
        inodo.tamEnBytesLog = offset+ nbytes;
        inodo.ctime = time(NULL); // Se actualiza siempre que modificamos campos del inodo.
    }
    inodo.mtime = time(NULL); //Se actualiza ya que hemos modificado la zona de datos.
    escribir_inodo(ninodo,inodo);
    return escritos;
}

int mi_read_f(unsigned int ninodo, void *buf_original,
  unsigned int offset, unsigned int nbytes){
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
    if ((inodo.permisos & 4) != 4) {
        fprintf(stderr, "Error: no hay permiso de escritura %s\n", strerror(errno)); 
        return -1;
    }
    int bleidos = 0; // bytes almacenados en el buf_original
     //No podemos leer más allà del "end of file".
    if(offset >= inodo.tamEnBytesLog){ // no podemos leer nada
        bleidos=0;
        return bleidos;
    }
    //En este caso no podríamos leer los nbytes que nos pasan por parámetro.
    if((offset+nbytes) >= inodo.tamEnBytesLog) {
        nbytes = inodo.tamEnBytesLog-offset; 
        //leesmos solo los bytes desde el offset hasta eof
    }
    int primerBLogico = offset/BLOCKSIZE;  // BL 22
    int ultimoBLogico = (nbytes + offset - 1)/BLOCKSIZE;  //BL 26
    char buf_bloque[BLOCKSIZE];
    int desp1 = offset%BLOCKSIZE;
    int desp2 = (nbytes + offset -1)%BLOCKSIZE;
    //fprintf(stderr, "desp1: %d, desp2: %d\n", desp1, desp2);
    int bfisico;
    // caso en el que el primer bloque logico y ultimo coiciden
    if (primerBLogico == ultimoBLogico){
        bfisico = traducir_bloque_inodo(ninodo,primerBLogico,0);
        if (bfisico != -1){ //error: no existe ese bloque fisico
            bread(bfisico,buf_bloque);  
            memcpy(buf_original,buf_bloque+desp1,nbytes);
        }
        bleidos = desp2+1-desp1 ;
            

    }else{  // si hay bloques intermedios
        bfisico = traducir_bloque_inodo(ninodo,primerBLogico,0);
        if (bfisico != -1){ //error: no existe ese bloque fisico
            if(bread(bfisico, buf_bloque) == -1){
                return -1;
            }
            memcpy(buf_original,buf_bloque+desp1,BLOCKSIZE-desp1);
        }
        bleidos = BLOCKSIZE-desp1;
        //intermedios
        for (int i = primerBLogico+1; i< ultimoBLogico;i++){
            bfisico = traducir_bloque_inodo(ninodo,i,0);
            if (bfisico != -1){ //error: no existe ese bloque fisico
                if(bread(bfisico,buf_bloque) < 0){
                    return -1;
                }
                memcpy(buf_original+(BLOCKSIZE-desp1)+(i-primerBLogico-1)*BLOCKSIZE,buf_bloque,BLOCKSIZE);
    
            }
            bleidos += BLOCKSIZE;
        }
        //ultimio
        bfisico = traducir_bloque_inodo(ninodo, ultimoBLogico,0);
        if (bfisico != -1){
            if(bread(bfisico,buf_bloque) == -1){
                return -1;
            }
            memcpy(buf_original +(nbytes-desp2-1),buf_bloque,desp2 + 1);
        }
        bleidos += desp2+1;
    }
    leer_inodo(ninodo,&inodo);
    inodo.atime = time(NULL);
    escribir_inodo(ninodo,inodo);
    return bleidos;
}

/*
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes) {
    unsigned int leidos, primerBL, ultimoBL, desp1, desp2, nbfisico, bl;
    struct inodo inodo;
    unsigned char buf_bloque[BLOCKSIZE];

    #if SEMAFOROS
    mi_waitSem();
    #endif 
    leer_inodo(ninodo, &inodo);
    inodo.atime = time(NULL);
    escribir_inodo(ninodo, inodo);
    #if SEMAFOROS
    mi_signalSem();
    #endif 

    if ((inodo.permisos & 4) != 4) {
        fprintf(stderr, "No hay permisos de lectura\n");
        return -1;
    }
    if (offset >= inodo.tamEnBytesLog) {
        leidos = 0; // No podemos leer nada
        return leidos;
    }

    if (offset + nbytes >= inodo.tamEnBytesLog) // pretende leer más allá de EOF
        nbytes = inodo.tamEnBytesLog - offset;  // leemos sólo los bytes que podemos desde el offset hasta EOF



    primerBL = offset / BLOCKSIZE;
    ultimoBL = (offset + nbytes - 1) / BLOCKSIZE;
    // fprintf(stderr, "primerBL: %d, ultimoBL: %d\n", primerBL, ultimoBL);

    desp1 = offset % BLOCKSIZE;
    if (primerBL == ultimoBL) {
        nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
        // fprintf(stderr, "\nBL: %d BF: %d\n", primerBL, nbfisico);
        if (nbfisico != -1) {
            // fprintf(stderr,"nbfisico!=0");
            if (bread(nbfisico, buf_bloque) < 0)
                return -1;
            memcpy(buf_original, buf_bloque + desp1, nbytes);
        }
        leidos = nbytes;

    } else {
        // lectura parcial primer bloque
        nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
        // fprintf(stderr, "\nBL: %d BF: %d\n", primerBL, nbfisico);
        if (nbfisico != -1) {
            if (bread(nbfisico, buf_bloque) < 0)
                return -1;
            memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
        }
        leidos = BLOCKSIZE - desp1;

        // lectura total bloques intermedios
        for (bl = primerBL + 1; bl < ultimoBL; bl++) {
            nbfisico = traducir_bloque_inodo(ninodo, bl, 0);
            // fprintf(stderr, "\nBL: %d BF: %d\n", bl, nbfisico);
            if (nbfisico != -1) {
                if (bread(nbfisico, buf_bloque) < 0)
                    return -1;
                memcpy(buf_original + (BLOCKSIZE - desp1) + (bl - primerBL - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE);
            }
            leidos += BLOCKSIZE;
        }

        // lectura parcial último bloque
        nbfisico = traducir_bloque_inodo(ninodo, ultimoBL, 0);
        desp2 = (offset + nbytes - 1) % BLOCKSIZE;
        // fprintf(stderr, "\nBL: %d BF: %d\n", ultimoBL, nbfisico);
        if (nbfisico != -1) {
            if (bread(nbfisico, buf_bloque) < 0)
                return -1;
            memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 + 1);
        }
        leidos += desp2 + 1;
    }

    return leidos;
}
*/
//stat: obtener los valores de los campos del inodo sin tener en cuenta los punteros
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    p_stat->atime = inodo.atime;
    p_stat->ctime = inodo.ctime;
    p_stat->mtime = inodo.mtime;
    p_stat->nlinks = inodo.nlinks;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;
    p_stat->permisos = inodo.permisos;
    return EXIT_SUCCESS;
}

int mi_chmod_f(unsigned int ninodo, unsigned char permisos){
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
    inodo.permisos = permisos;
    inodo.ctime = time(NULL);
    escribir_inodo(ninodo,inodo);
    return EXIT_SUCCESS;
}