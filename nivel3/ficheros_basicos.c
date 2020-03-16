#include "ficheros_basicos.h"

int tamMB(unsigned int nbloques){
    int tamMB = (nbloques/8)/BLOCKSIZE; //Así tenemos el num de Bytes
    if ((nbloques/8)%BLOCKSIZE != 0){
        tamMB++;
    }
    return tamMB;
}

int tamAI(unsigned int ninodos){
    //el ninodos q nos pasan en teoria ya esta: ninodos = nbloques/4
    int tamAI = (ninodos*INODOSIZE)/BLOCKSIZE;
    if ((ninodos*INODOSIZE)%BLOCKSIZE != 0){
        tamAI++;
    }
    return tamAI;
}


//initSB nos permite rellenar los datos básicos del superbloque
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
    bwrite(POSSB,&SB);
    return  EXIT_SUCCESS;
}

/*
//initMB pone a 0 todos los bits del mapa de bits
int initMB(){
	unsigned char buffer[BLOCKSIZE];
	//inicializamos el buffer.
	memset(buffer, 0, sizeof(buffer));
	struct superbloque SB;
	//leemos el superbloque
	bread(POSSB, &SB);

	//llamamos a la funcion bwrite hasta llenar el mapa de bits
	int posicion = SB.posPrimerBloqueMB;
	while (posicion <= SB.posUltimoBloqueMB) {
        bwrite(posicion, buffer); //poner 0 en todo el MB
        posicion++;
    }
	for (int i=POSSB; i <= SB.posUltimoBloqueAI; i++) {
        escribir_bit(i, 1);
    }
	bwrite(POSSB,&SB);
	return 0;
}
*/
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
        res = res + (2^j);                    // crear funcion elevar
    }
    buf[numbyte1] |=res;
    bwrite(nbloques+SB.posPrimerBloqueMB,buf);  
    bwrite(POSSB,&SB);
    return EXIT_SUCCESS;
} 

//initAI pone a 0 todos los inodos del array
int initAI(){
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE]; 
    bread(POSSB,&SB);
    int contInodos = SB.posPrimerInodoLibre +1;
    //Si hemos inicializado SB.posPrimerInodoLibre a 0:
    for(int i = SB.posPrimerBloqueAI ; i<= SB.posUltimoBloqueAI; i++){
        for (int j = 0; j < (BLOCKSIZE/INODOSIZE); j++){
            inodos[j].tipo = 'l'; //libre
            if(contInodos<SB.totInodos){
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }
            else{
                inodos[j].punterosDirectos[0]= UINT_MAX;  // como poner a NULL             
            }
        }
        bwrite(i,inodos);
    }
    return EXIT_SUCCESS;
}

//Escribir bit pone el valor indicado por el parametro en un determinado bit
int escribir_bit(unsigned int nbloque, unsigned int bit){
    struct superbloque SB;
    bread(POSSB,&SB);
    int posbyte = nbloque/8;
    int posbit = nbloque%8;
    int nbloqueMB = posbyte/BLOCKSIZE;
    int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;
    unsigned char bufferMB[BLOCKSIZE];
    bread(nbloqueabs,bufferMB);
    posbyte = posbyte % BLOCKSIZE;
    unsigned char mascara = 128;
    mascara >>= posbit;
    if(bit == 0){
        bufferMB[posbyte] &= ~mascara;
    }else{
        bufferMB[posbyte] |= mascara;
    }
    bwrite(nbloqueabs,bufferMB);
    return EXIT_SUCCESS;

}

//Leer bit lee un bit del MB y devuelve el valor
unsigned char leer_bit(unsigned int nbloque){
    struct superbloque SB;
    bread(POSSB,&SB);
    int posbyte = nbloque/8;
    int posbit = nbloque%8;
    int nbloqueMB = posbyte/BLOCKSIZE;
    int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;
    unsigned char bufferMB[BLOCKSIZE];
    bread(nbloqueabs,&bufferMB);
    posbyte = posbyte % BLOCKSIZE;
    unsigned char mascara = 128;
    mascara >>= posbit;
    mascara &= bufferMB[posbyte];
    mascara >>= (7-posbit);
    printf("[leer_bit()-> nbloque = %d, posbyte = %d, posbit = %d, nbloqueMB: %d, nbloqueabs = %d\n",
        nbloque,posbyte,posbit, nbloqueMB, nbloqueabs);
    
    return mascara;   
}

//Funcion que nos permite reservar un bloque
int reservar_bloque(){
    struct superbloque SB;
    unsigned char bufferMB[BLOCKSIZE];
    unsigned char bufferAux[BLOCKSIZE];
    int posBloqueMB;
    bread(POSSB,&SB);
    if(SB.cantBloquesLibres == 0){
        fprintf(stderr, "Error: no hay bloques libres %s\n", strerror(errno)); 
        return EXIT_FAILURE;
    }

    memset(bufferAux,255,BLOCKSIZE);
    posBloqueMB = SB.posPrimerBloqueMB;
	bread(posBloqueMB, bufferMB);
  	while(memcmp(bufferMB, bufferAux, BLOCKSIZE) == 0){
		//incrementamos la posicion del bloque
		posBloqueMB = posBloqueMB + 1;
		bread(posBloqueMB, bufferMB); 
	
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
    bwrite(POSSB,&SB);
    memset(bufferAux,0,BLOCKSIZE);
    bwrite(nbloque,bufferAux);  
    
    
    return nbloque;
}

//Funcion que nos permite liberar un bloque pasado por parametro
int liberar_bloque(unsigned int nbloque){
    struct superbloque SB;
    bread(POSSB,&SB);
    escribir_bit(nbloque,0);
    
    SB.cantBloquesLibres = SB.cantBloquesLibres +1;
   
    bwrite(POSSB,&SB);
    return (int) nbloque;
}

//Funcion que escribe el contenido de una variable de tipo struct inodo en un determinado inodo del array de inodos.
int escribir_inodo(unsigned int ninodo, struct inodo inodo){
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    bread(POSSB,&SB);

    //Localizamos el bloqeu donde se encuentrael inodo:
    int nbloque = ninodo/(BLOCKSIZE/INODOSIZE);
    bread((SB.posPrimerBloqueAI+nbloque),inodos);

    //Escribimos el inodo:
    inodos[ninodo%(BLOCKSIZE/INODOSIZE)] = inodo;
    bwrite(SB.posPrimerBloqueAI+nbloque,inodos); // a la posición del bloque donde se 
    //encuentras el inodo que queremos escribir
    return EXIT_SUCCESS;
}


//Funcion que lee un determinado inodo del array de inodos
int leer_inodo(unsigned int ninodo, struct inodo *inodo){
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    bread(POSSB,&SB);

    //Localizamos el bloqeu donde se encuentrael inodo:
    int nbloque = ninodo/(BLOCKSIZE/INODOSIZE);
    bread(SB.posPrimerBloqueAI+nbloque,inodos);

    //leemos el inodo:
    *inodo = inodos[ninodo%(BLOCKSIZE/INODOSIZE)];
    return 0;   // en que caso no ira bien?????????!!!!!!!!!!!!!!
}

//Funcion que encuentra el primer inodo libre y lo reserva
int reservar_inodo(unsigned char tipo, unsigned char permisos){
    struct superbloque SB;
    bread(POSSB,&SB);
    struct inodo inodo;  
    if(SB.cantInodosLibres == 0){
        fprintf(stderr, "Error: no hay inodos libres %s\n", strerror(errno)); 
        return EXIT_FAILURE;
    }

    int posInodoReservado = SB.posPrimerInodoLibre;
    
    leer_inodo(posInodoReservado,&inodo);
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
    bwrite(POSSB,&SB);
    return posInodoReservado;
}

