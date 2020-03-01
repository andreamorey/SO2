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
    SB.cantBloquesLibres = nbloques;
    SB.cantInodosLibres = ninodos;
    SB.totBloques = nbloques;
    SB.totInodos = ninodos;
    bwrite(POSSB,&SB);
    return EXIT_SUCCESS;
}
int initMB(){
    struct superbloque SB;
    unsigned char buf[BLOCKSIZE];
    memset(buf,0,BLOCKSIZE);
    bread(POSSB,&SB);
    int posInicialMB = SB.posPrimerBloqueMB;
    int posFinalMB = SB.posUltimoBloqueMB;
    //tamMB dnde se usa¿?
    for (int i = posInicialMB; i < posFinalMB; i++){ //que le pasamos al  for¿?
       // bwrite(i*BLOCKSIZE,buf);
       bwrite(posInicialMB +i, buf); //Lubo
    }
    return EXIT_SUCCESS;
}

int initAI(){
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE]; //PORQ DEFINIMOS ESTO ASI?
    bread(POSSB,&SB);
    int contInodos = SB.posPrimerInodoLibre +1;
    //Si hemos inicializado SB.posPrimerInodoLibre a 0:
    for(int i = SB.posPrimerBloqueAI ; i<= SB.posUltimoBloqueAI; i++){
        for (int j = 0; j < BLOCKSIZE/INODOSIZE; j++){
            inodos[j].tipo = 'l'; //libre
            if(contInodos<SB.totInodos){
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }
            else{
                inodos[j].punterosDirectos[0]= UINT_MAX;               
            }
        }
        bwrite(SB.posPrimerBloqueAI+i,&SB);
    }
    return EXIT_SUCCESS;
}