#include "ficheros_basicos.h"

// devuelve los bloques que ocupa el mapa de bits
int tamMB(unsigned int nbloques){
    int tamMB = (nbloques/8)/BLOCKSIZE; //numBytes 
    if ((nbloques/8)%BLOCKSIZE != 0){
        tamMB++;
    }
    return tamMB;
}

// devuelve los bloques del array de inodos
int tamAI(unsigned int ninodos){ 
    int tamAI = (ninodos*INODOSIZE)/BLOCKSIZE;
    if ((ninodos*INODOSIZE)%BLOCKSIZE != 0){
        tamAI++;
    }
    return tamAI;
}

// inicializa el superbloque con los datos básicos
int initSB(unsigned int nbloques, unsigned int ninodos){
    struct superbloque SB;
    int tamSB = 1;
    SB.posPrimerBloqueMB = POSSB + tamSB;
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) -1;
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB +1;
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI+tamAI(ninodos)-1;
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI+1;
    SB.posUltimoBloqueDatos = nbloques-1;
    SB.posInodoRaiz = 0;
    SB.posPrimerInodoLibre = 0;
    SB.cantBloquesLibres = nbloques -tamMB(nbloques)-tamAI(ninodos)-tamSB;
    SB.cantInodosLibres = ninodos;
    SB.totBloques = nbloques;
    SB.totInodos = ninodos;
    if(bwrite(POSSB,&SB)==-1){
       // fprintf(stderr, "Error no se ha podido inicializar los datos del "
      //  "superbloque: %s\n", strerror(errno));
        return -1;
    }
    return  EXIT_SUCCESS;
}

//initMB pone a 0 todos los bits del mapa de bits
int initMB(){
	unsigned char buffer[BLOCKSIZE];
	memset(buffer, 0, sizeof(buffer)); // inicializamos el buffer a 0
	struct superbloque SB;
	if (bread(POSSB, &SB) == -1){ //leemos el superbloque
        //fprintf(stderr,"Error no se ha podido leer el "
       // "superbloque: %s\n", strerror(errno));
        return -1;
    }
	//llamamos a la funcion bwrite hasta llenar el mapa de bits
	int posicion = SB.posPrimerBloqueMB;
	while (posicion <= SB.posUltimoBloqueMB) {
        if(bwrite(posicion, buffer)==-1) { //poner 0 en todo el MB
           // fprintf(stderr,"Error no se ha podido guardar los datos en el "
           // "superbloque: %s\n", strerror(errno));
            return -1;
        } 
        posicion++;
    }
    // ponemos a 1 los bits que representan los metadatos
	for (int i=POSSB; i <= SB.posUltimoBloqueAI; i++) {
        escribir_bit(i, 1);
    }
	if(bwrite(POSSB,&SB)==-1){ // guardamos el todo en el superbloque
        //fprintf(stderr,"Error no se ha podido guardar el "
        //"superbloque: %s\n", strerror(errno));
        return -1;
    }
	return 0;
}

/*
int initMB(){
    struct superbloque SB;
    unsigned char buf[BLOCKSIZE];
    memset(buf, 0, sizeof(buf));
    bread(POSSB,&SB);     
    int  numbyte1=(SB.posUltimoBloqueAI+1)/8;//392 bytes que tienen que tenr 1s (metadatos)
    int nbloques = (numbyte1/BLOCKSIZE);
    if(nbloques > 1){
        memset(buf,255,BLOCKSIZE);
        for(int i = SB.posPrimerBloqueMB; i <(SB.posPrimerBloqueMB+ nbloques); i++){
            bwrite(i,buf);
        }
    }
    int modulo=(SB.posUltimoBloqueAI+1)%8;
    bread(nbloques+SB.posPrimerBloqueMB,buf);  // guardamos el bloque  
    for (int i=0;i<numbyte1; i++){
        buf[i] |= 255;
    }
    int res=0;
    for(int i =0, j = 7; i<=modulo;i++, j--){
        res = res + elevar(2,j);                    // crear funcion elevar
    }
    buf[numbyte1] |=res;
    bwrite(nbloques+SB.posPrimerBloqueMB,buf);  
    bwrite(POSSB,&SB);
    return EXIT_SUCCESS;
} 
*/

// nos devulve el num elevado a elevado
int elevar(int num, int elevado){
    int resultado = 1;
    for(int i = 1 ; i<= elevado; i++){  
        resultado = resultado * num;
    }
    return resultado;
}

//initAI pone a 0 todos los inodos del array de inodos
int initAI(){
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE]; 
    if (bread(POSSB,&SB) == -1){
       // fprintf(stderr,"Error no se ha podido leer el "
       // "superbloque: %d\n", strerror(errno));
        return -1;
    }
    int contInodos = SB.posPrimerInodoLibre +1;
    for(int i = SB.posPrimerBloqueAI ; i<= SB.posUltimoBloqueAI; i++){
        for (int j = 0; j < (BLOCKSIZE/INODOSIZE); j++){
            inodos[j].tipo = 'l'; //libre
            if(contInodos<SB.totInodos){
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }
            else{
                inodos[j].punterosDirectos[0]= UINT_MAX;  
            }
        }
        if(bwrite(i,inodos) == -1){
          //  fprintf(stderr,"Error no se han podido guardar los "
          //  "inodos: %s\n", strerror(errno));
            return -1;
        }
    }
    return EXIT_SUCCESS;
}

// pone el valor indicado por el parámetro en un determinado bit
int escribir_bit(unsigned int nbloque, unsigned int bit){
    struct superbloque SB;
    if(bread(POSSB,&SB) == -1){
       // fprintf(stderr,"Error no se ha podido leer el "
       // "superbloque: %s\n", strerror(errno));
        return -1;
    }
    int posbyte = nbloque/8;
    int posbit = nbloque%8;
    int nbloqueMB = posbyte/BLOCKSIZE;
    int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;
    unsigned char bufferMB[BLOCKSIZE];
    if(bread(nbloqueabs,bufferMB) == -1){
        //fprintf(stderr,"Error no se ha podido leer los datos del "
       // "superbloque: %s\n", strerror(errno));
        return -1;
    }
    posbyte = posbyte % BLOCKSIZE;
    unsigned char mascara = 128;
    mascara >>= posbit;
    if(bit == 0){
        bufferMB[posbyte] &= ~mascara;
    }else{
        bufferMB[posbyte] |= mascara;
    }
    if(bwrite(nbloqueabs,bufferMB) == -1){
       // fprintf(stderr,"Error no se han podido guardar los datos al"
       // "superbloque: %s\n", strerror(errno));
        return -1;
    }
    return EXIT_SUCCESS;

}

//Leer bit lee un bit del MB y devuelve el valor
unsigned char leer_bit(unsigned int nbloque){
    struct superbloque SB;
    if(bread(POSSB,&SB) == -1){
      //  fprintf(stderr,"Error no se ha podido leer del "
        //"superbloque: %s\n", strerror(errno));
        return -1;
    }
    int posbyte = nbloque/8, posbit = nbloque%8;  
    int nbloqueMB = posbyte/BLOCKSIZE;
    int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;
    unsigned char bufferMB[BLOCKSIZE];
    if(bread(nbloqueabs,&bufferMB) == -1){
       // fprintf(stderr,"Error no se ha podido leer del "
       // "superbloque: %s\n", strerror(errno));
        return -1;
    }
    posbyte = posbyte % BLOCKSIZE;
    unsigned char mascara = 128;
    mascara >>= posbit;
    mascara &= bufferMB[posbyte];
    mascara >>= (7-posbit); // movemos el bit a la primera posición ..000b
    return mascara;   // devolvemos el bit leido
}

// Función que nos permite reservar un bloque
int reservar_bloque(){
    struct superbloque SB;
    unsigned char bufferMB[BLOCKSIZE], bufferAux[BLOCKSIZE];
    int posBloqueMB;
    if (bread(POSSB,&SB) == -1){
     //   fprintf(stderr,"Error no se ha podido leer el "
       // "superbloque: %s\n", strerror(errno));
        return -1;
    }
    if(SB.cantBloquesLibres == 0){
        fprintf(stderr, "Error, no hay bloques libres:"
        " %s\n", strerror(errno)); 
        return EXIT_FAILURE;
    }
    memset(bufferAux,255,BLOCKSIZE); // ponemos todo el buffer a 1 
    posBloqueMB = SB.posPrimerBloqueMB;
	if(bread(posBloqueMB, bufferMB) == -1){
      //  fprintf(stderr,"Error no se ha podido leer del "
      //  "superbloque: %s\n", strerror(errno));
        return -1;
    }
  	while(memcmp(bufferMB, bufferAux, BLOCKSIZE) == 0){ 
		//incrementamos la posicion del bloque
		posBloqueMB = posBloqueMB + 1;
		if(bread(posBloqueMB, bufferMB) == -1){
        //    fprintf(stderr,"Error no se ha podido leer del "
          //  "superbloque: %s\n", strerror(errno));
            return -1;
        } 
    }
    int posbyte;
    for (int i =0; i< BLOCKSIZE && bufferMB[i] == 255 ; i++){
        posbyte = i;  
    }
    posbyte++;
    unsigned char mascara = 128;
    int posbit = 0;
    while (bufferMB[posbyte] & mascara){ 
        posbit++;
        bufferMB[posbyte] <<=1;
    }
    int nbloque = ((posBloqueMB - SB.posPrimerBloqueMB)*BLOCKSIZE + posbyte)*8+posbit;
    escribir_bit(nbloque,1);
    SB.cantBloquesLibres = SB.cantBloquesLibres - 1;
    if (bwrite(POSSB,&SB) == 1){
       // fprintf(stderr,"Error no se han podido guardar datos "
      //  "en el superbloque: %s\n", strerror(errno));
        return -1;
    }
    memset(bufferAux,0,BLOCKSIZE);
    if(bwrite(nbloque,bufferAux) == -1){
      //  fprintf(stderr,"Error no se han podido guardar los 0s "
      //  "en el bloque: %s\n", strerror(errno));
        return -1;
    } 
    return nbloque;
}

//Funcion que nos permite liberar un bloque pasado por parametro
int liberar_bloque(unsigned int nbloque){
    struct superbloque SB;
    if (bread(POSSB,&SB) == -1){
       // fprintf(stderr,"Error no se ha podido leer el "
      //  "superbloque: %s\n", strerror(errno));
        return -1;

    }
    escribir_bit(nbloque,0);
    SB.cantBloquesLibres = SB.cantBloquesLibres +1;
    if(bwrite(POSSB,&SB) == -1){
       // fprintf(stderr,"Error no se han podido guardar los datos "
       // "del superbloque: %s\n", strerror(errno));
        return -1;
    }
    return (int) nbloque;
}

//Funcion que escribe el contenido de una variable de tipo
// struct inodo en un determinado inodo del array de inodos.
int escribir_inodo(unsigned int ninodo, struct inodo inodo){
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    if (bread(POSSB,&SB)== -1){
      //  fprintf(stderr,"Error no se ha podido leer el "
     //   "superbloque: %s\n", strerror(errno));
        return -1;
    }
    //Localizamos el bloqeu donde se encuentrael inodo:
    int nbloque = ninodo/(BLOCKSIZE/INODOSIZE);
    if(bread((SB.posPrimerBloqueAI+nbloque),inodos) == -1){
      //  fprintf(stderr,"Error no se ha podido leer los "
      //  "inodos del array de inodos: %s\n", strerror(errno));
        return -1;
    }
    //Escribimos el inodo:
    inodos[ninodo%(BLOCKSIZE/INODOSIZE)] = inodo;
    // a la posición del bloque donde se encuentra el inodo 
    // que queremos escribir
    if(bwrite(SB.posPrimerBloqueAI+nbloque,inodos)==-1){
       // fprintf(stderr,"Error no se han podido guardar los "
      //  "inodos: %s\n", strerror(errno));
        return -1;
    }   
    return EXIT_SUCCESS;
}

//Funcion que lee un determinado inodo del array de inodos
int leer_inodo(unsigned int ninodo, struct inodo *inodo){
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    if (bread(POSSB,&SB) == -1){
     //   fprintf(stderr,"Error no se ha podido leer del "
      //  "superbloque: %s\n", strerror(errno));
        return -1;
    }
    //Localizamos el bloque donde se encuentra el inodo:
    int nbloque = ninodo/(BLOCKSIZE/INODOSIZE);
    if(bread(SB.posPrimerBloqueAI+nbloque,inodos) == -1){
      //  fprintf(stderr,"Error no se ha podido leer los "
      //  "inodos del array de inodos: %s\n", strerror(errno));
        return -1;
    }
    //leemos el inodo:
    *inodo = inodos[ninodo%(BLOCKSIZE/INODOSIZE)];
    return EXIT_SUCCESS;  
}

//Funcion que encuentra el primer inodo libre y lo reserva
int reservar_inodo(unsigned char tipo, unsigned char permisos){
    struct superbloque SB;
    if(bread(POSSB,&SB) == -1){
      //  fprintf(stderr,"Error no se ha podido leer el "
      //  "superbloque: %s\n", strerror(errno));
        return -1;
    }
    struct inodo inodo;  
    if(SB.cantInodosLibres == 0){ 
      //  fprintf(stderr, "Error: no hay inodos "
      //  "libres %s\n", strerror(errno)); 
        return -1;
    }
    int posInodoReservado = SB.posPrimerInodoLibre;
    leer_inodo(posInodoReservado,&inodo);
    //reservamos el inodo:
    SB.posPrimerInodoLibre = inodo.punterosDirectos[0] ;
    inodo.tipo = tipo;
    inodo.permisos= permisos;
    inodo.nlinks = 1;
    inodo.tamEnBytesLog = 0;
    inodo.atime = time(NULL);
    inodo.ctime = time(NULL);
    inodo.mtime = time(NULL);
    inodo.numBloquesOcupados = 0;
    for (int i  = 0; i< 12; i++){
        inodo.punterosDirectos[i] = 0;
    }
    for (int i = 0; i<3; i++){
        inodo.punterosIndirectos[i] = 0;
    }
    escribir_inodo(posInodoReservado,inodo);
    SB.cantInodosLibres = SB.cantInodosLibres - 1;
    if (bwrite(POSSB,&SB) == -1){
      //  fprintf(stderr,"Error no se han podido guardar los datos"
        //"en el superbloque: %s\n", strerror(errno));
        return -1;
    }
    return posInodoReservado;
}

int obtener_nrangoBL(struct inodo inodo, unsigned int nblogico, 
unsigned int *ptr){
    if(nblogico < DIRECTOS){
        *ptr = inodo.punterosDirectos[nblogico];
        return 0;
    }else if (nblogico < INDIRECTOS0 ){
        *ptr = inodo.punterosIndirectos[0];
        return 1;
    }else if( nblogico < INDIRECTOS1){
        *ptr = inodo.punterosIndirectos[1];
        return 2;
    }else if(nblogico < INDIRECTOS2){
        *ptr = inodo.punterosIndirectos[2];
        return 3;
    }else{
        *ptr = 0;
        fprintf(stderr, "Error: no hay inodos libres "
        "%s\n", strerror(errno)); 
        return -1;
    }
}       

int obtener_indice(int nblogico, int nivel_punteros){
    if(nblogico< DIRECTOS) return nblogico;
    else if (nblogico<INDIRECTOS0) return nblogico-DIRECTOS;
    else if (nblogico<INDIRECTOS1) {
        if (nivel_punteros==2){
            return (nblogico - INDIRECTOS0)/NPUNTEROS;
        }else if (nivel_punteros ==1){
            return (nblogico - INDIRECTOS0)%NPUNTEROS;
        }

    }else if (nblogico<INDIRECTOS2){
        if (nivel_punteros ==3){
            return (nblogico-INDIRECTOS1)/(NPUNTEROS*NPUNTEROS);
        }else if (nivel_punteros == 2){
            return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))/NPUNTEROS;
        }else if (nivel_punteros == 1){
            return ((nblogico-INDIRECTOS1)%(NPUNTEROS*NPUNTEROS))%NPUNTEROS;
        }
    }
    return -1;  // en caso de error

}    
  
  //hay que añadir los posibles casos de error de obtener
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, 
char reservar){
    struct inodo inodo;
    unsigned int ptr, ptr_ant, salvar_inodo, nRangoBL;
    int nivel_punteros, indice;
    unsigned int buffer[NPUNTEROS];
    leer_inodo(ninodo, &inodo);
    ptr = 0, ptr_ant=0, salvar_inodo=0;
    nRangoBL = obtener_nrangoBL(inodo, nblogico, &ptr);  //0:D, 1:I0, 2:I1, 3:I2
    nivel_punteros = nRangoBL;  //el nivel_punteros +alto es el que cuelga del inodo
    while (nivel_punteros > 0 ){  //iterar para cada nivel de indirectos
        if (ptr == 0){    //no cuelgan bloques de punteros
            if (reservar ==0 ){
                return -1;  //error lectura bloque inexistente
            }else {     //reservar bloques punteros y crear enlaces desde inodo hasta datos
                salvar_inodo = 1;
                ptr = reservar_bloque();        //de punteros    
                inodo.numBloquesOcupados++;
                inodo.ctime = time(NULL);   //fecha actual
                if (nivel_punteros == nRangoBL){    //el bloque cuelga directamente del inodo
                    inodo.punterosIndirectos[nRangoBL-1] = ptr;  
                    // printf(BLUE "[traducir_bloque_inodo() --> inodo.punterosIndirectos[%d] = %d "
                //"(reservar BF %d para punteros_nivel%d)]\n"RESET,nRangoBL-1, ptr, 
                //ptr,nivel_punteros );  // punteros indirectos azul
                }else{      //el bloque cuelga de otro bloque de punteros
                    buffer[indice] = ptr;  
                    //printf(BLUE"[traducir_bloque_inodo() --> punteros_nivel%d[%d] = %d (reservado "
                    //"BF %d para punteros_nivel%d)]\n"RESET,nivel_punteros+1,indice, ptr,ptr, nivel_punteros);
                    if(bwrite(ptr_ant, buffer) == -1){
                     //   fprintf(stderr,"Error no se han podido guardar los "
                       // "datos: %s\n", strerror(errno));
                        return -1;
                    }
                }
            }
        }
        if (bread(ptr, buffer) == -1){
        //    fprintf(stderr,"Error no se han podido leer los "
          //              "datos: %s\n", strerror(errno));
                        return -1;
        }
        indice = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr;     //guardamos el puntero
        ptr = buffer[indice];   // y lo desplazamos al siguiente nivel
        nivel_punteros--;
    } //al salir de este bucle ya estamos al nivel de datos
    if ( ptr == 0){                //no existe bloque de datos
        if (reservar ==0 ){      //error lectura ∄ bloque   
            return -1;
        }else{
            salvar_inodo = 1;
            ptr = reservar_bloque();  // de datos
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);
            if (nRangoBL ==0){
                inodo.punterosDirectos[nblogico] = ptr;  
                //printf( RED "[traducir_bloque_inodo() --> inodo.punterosDirectos[%d] = %d "
                // "(reservado BF %d para BL %d)]\n"RESET,nblogico, ptr, ptr, nblogico);  //solo en punteros directos
            }else {
                buffer[indice] = ptr;   
                //printf(RED  "[traducir_bloque_inodo() --> punteros_nivel%d[%d] = %d "
                // "(reservar BF %d para BL %d)]\n"RESET, nivel_punteros+1,indice, ptr, ptr,nblogico);  // punteros indirectos azul
                if (bwrite(ptr_ant, buffer) == -1){
                   // fprintf(stderr,"Error no se han podido guardar los "
                   // "datos: %s\n", strerror(errno));
                    return -1;

                }
            }
        }
    }
    if (salvar_inodo ==1){
        escribir_inodo(ninodo,inodo);  //sólo si lo hemos actualizado
    }
    return ptr;  //nbfisico
}  
