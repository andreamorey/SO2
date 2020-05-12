#include "ficheros_basico.h"

// Devuelve el tamaño en bloques que ocupa el mapa de bits
int tamMB(unsigned int nbloques){
    int tamMB = (nbloques/8)/BLOCKSIZE; //numBytes 
    if ((nbloques/8)%BLOCKSIZE != 0){
        tamMB++;
    }
    return tamMB;
}

// Devuelve el tamaño en bloques del array de inodos
int tamAI(unsigned int ninodos){ 
    int tamAI = (ninodos*INODOSIZE)/BLOCKSIZE;
    if ((ninodos*INODOSIZE)%BLOCKSIZE != 0){
        //Hay que añadir un bloque adicional para el resto.
        tamAI++;
    }
    return tamAI;
}

// Inicializa el superbloque con los datos básicos
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
        fprintf(stderr, "[initSB()-> no se ha podido inicializar los datos del "
        "superbloque: %s]\n", strerror(errno));
        return -1;
    }
    return  0;
}

//initMB inicializa los bits del mapa de bits, teniendo en cuenta los
//bloques ocupados por metadatos (SB, MB y AI)
int initMB(){
	unsigned char buffer[BLOCKSIZE];
	memset(buffer, 0, sizeof(buffer)); // inicializamos el buffer a 0
	struct superbloque SB;
	if (bread(POSSB, &SB) == -1){ //leemos el superbloque
        fprintf(stderr,"[initMB()-> no se ha podido leer el "
        "superbloque: %s]\n", strerror(errno));
        return -1;
    }
	//llamamos a la función bwrite hasta llenar el mapa de bits
	int posicion = SB.posPrimerBloqueMB;
	while (posicion <= SB.posUltimoBloqueMB) {
        if(bwrite(posicion, buffer)==-1) { //poner 0 en todo el MB
            fprintf(stderr,"[initMB()-> no se ha podido guardar los datos en el "
            "superbloque: %s]\n", strerror(errno));
            return -1;
        } 
        posicion++;
    }
    // ponemos a 1 los bits que representan los metadatos
	for (int i=POSSB; i <= SB.posUltimoBloqueAI; i++) {
        escribir_bit(i, 1);
    }
	if(bwrite(POSSB,&SB)==-1){ // guardamos el todo en el superbloque
        fprintf(stderr,"[initMB()-> no se ha podido guardar los datos en el "
        "superbloque: %s]\n", strerror(errno));
        return -1;
    }
	return 0;
}

// Función para calcular potencias
int elevar(int num, int elevado){
    int resultado = 1;
    for(int i = 1 ; i<= elevado; i++){  
        resultado = resultado * num;
    }
    return resultado;
}

//Crea inicialmente la lista enlazada de los inodos libres
int initAI(){
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE]; //buffer tamaño BLOCKSIZE
    if (bread(POSSB,&SB) == -1){
        fprintf(stderr,"[initAI()-> no se ha podido leer el "
        "superbloque: %s]\n", strerror(errno));
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
                inodos[j].punterosDirectos[0]= UINT_MAX; //le asignamos
                //el máximo valor para un unsigned int
            }
        }
        if(bwrite(i,inodos) == -1){
            fprintf(stderr,"[initAI()-> no se han podido guardar los "
            "inodos: %s]\n", strerror(errno));
            return -1;
        }
    }
    return 0;
}

// Pone el valor indicado por el parámetro en un determinado bit
//del MB representado por el nbloque que también nos pasan.
int escribir_bit(unsigned int nbloque, unsigned int bit){
    struct superbloque SB;
    if(bread(POSSB,&SB) == -1){
        fprintf(stderr,"[escribir_bit()->no se ha podido leer el "
        "superbloque: %s]\n", strerror(errno));
        return -1;
    }
    
    /*
    Una vez leído el SB, dado el nbloque debemos buscar dónde
    se ubica su bit correspondiente en el MB y luego el nº de
    bloque físico para grabarlo despúes de modificarlo.
    */
    int posbyte = nbloque/8; //posición del byte que contiene el bit
                            //que representa el nbloqueen el MB
    int posbit = nbloque%8; // la posición del bit dentro de ese byte
    int nbloqueMB = posbyte/BLOCKSIZE; //bloque del MB dónde está ese bit
    int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB; //posición del bloque
    //en el dispositivo virtual para leer/escribir el bit
    unsigned char bufferMB[BLOCKSIZE];
    if(bread(nbloqueabs,bufferMB) == -1){
        fprintf(stderr,"[escribir_bit()-> no se ha podido leer "
        "el mapa de bits: %s]\n", strerror(errno));
        return -1;
    }
    posbyte = posbyte % BLOCKSIZE;
    //ahora con una máscara ya podemos poner el bit a 0 o a 1
    unsigned char mascara = 128;
    mascara >>= posbit; //desplazamiento a la derecha de bits
    if(bit == 0){
        bufferMB[posbyte] &= ~mascara;
    }else{
        bufferMB[posbyte] |= mascara;
    }
    if(bwrite(nbloqueabs,bufferMB) == -1){
        fprintf(stderr,"[escribir_bit()-> no se ha podido escribir en "
        "el mapa de bits: %s]\n", strerror(errno));
        return -1;
    }
    return 0;
}

// Esta función lee un bit del MB y devuelve el valor leído
unsigned char leer_bit(unsigned int nbloque){
    struct superbloque SB;
    if(bread(POSSB,&SB) == -1){
        fprintf(stderr,"[leer_bit()-> no se ha podido leer del "
        "superbloque: %s]\n", strerror(errno));
        return -1;
    }
    //se hacen los siguientes cálculos igual que en escribir_bit:
    int posbyte = nbloque/8, posbit = nbloque%8;  
    int nbloqueMB = posbyte/BLOCKSIZE;
    int nbloqueabs = nbloqueMB + SB.posPrimerBloqueMB;
    unsigned char bufferMB[BLOCKSIZE];
    if(bread(nbloqueabs,&bufferMB) == -1){
        fprintf(stderr,"[leer_bit()-> no se ha podido leer el "
        "mapa de bits: %s]\n", strerror(errno));
        return -1;
    }
    //lee el bit correspondiente usando un desplazamiento de bits a la derecha
    posbyte = posbyte % BLOCKSIZE;
    unsigned char mascara = 128;
    mascara >>= posbit;
    mascara &= bufferMB[posbyte];
    mascara >>= (7-posbit); // movemos el bit a la primera posición ..000b
    return mascara;   // devolvemos el bit leido
}

// Función que nos permite reservar un bloque libre del MB
int reservar_bloque(){
    struct superbloque SB;
    unsigned char bufferMB[BLOCKSIZE], bufferAux[BLOCKSIZE];
    int posBloqueMB;
    if (bread(POSSB,&SB) == -1){
        fprintf(stderr,"[reservar_bloque()-> no se ha podido leer el "
        "superbloque: %s]\n", strerror(errno));
        return -1;
    }
    if(SB.cantBloquesLibres == 0){ //Comprobamos que quedan bloques libres
        fprintf(stderr, "reservar_bloque()-> no hay bloques libres:"
        " %s]\n", strerror(errno));
        return -1;
    }
    //Como aún quedan, localizamos el 1er bloque libre del dispositivo virtual
    //consultando cuál es el primer bit a 0 del MB.
    memset(bufferAux,255,BLOCKSIZE); // ponemos todo el buffer a 1 
    posBloqueMB = SB.posPrimerBloqueMB;
	if(bread(posBloqueMB, bufferMB) == -1){
        fprintf(stderr,"[reservar_bloque()-> no se ha podido leer del "
        "mapa de bits: %s]\n", strerror(errno));
        return -1;
    }
    //localizamos la posición del primer bloque del MB que tenga algún bit a 0
  	while(memcmp(bufferMB, bufferAux, BLOCKSIZE) == 0){
		posBloqueMB = posBloqueMB + 1; //incrementamos la posicion del bloque
		if(bread(posBloqueMB, bufferMB) == -1){
            fprintf(stderr,"[reservar_bloque()-> no se ha podido leer del "
            "mapa de bits: %s]\n", strerror(errno));
            return -1;
        } 
    }
    //Ahora localizamos qué byte dentro de ese bloque tiene algún 0
    int posbyte;
    for (int i =0; i< BLOCKSIZE && bufferMB[i] == 255 ; i++){
        posbyte = i;  
    }
    posbyte++;
    //Finalmente localizamos el primer bit dentro de ese byte que vale 0
    unsigned char mascara = 128;
    int posbit = 0;
    while (bufferMB[posbyte] & mascara){ 
        posbit++;
        bufferMB[posbyte] <<=1;
    }
    // cálculo para determinar cuál es el nº de bloque que reservaremos
    int nbloque = ((posBloqueMB - SB.posPrimerBloqueMB)*BLOCKSIZE + posbyte)*8+posbit;
    escribir_bit(nbloque,1); //le pasamos un 1 para indicar que el bloque está reservado
    SB.cantBloquesLibres = SB.cantBloquesLibres - 1;
    if (bwrite(POSSB,&SB) == 1){
        fprintf(stderr,"[reservar_bloque()-> no se han podido guardar datos "
        "en el superbloque: %s]\n", strerror(errno));
        return -1;
    }
    memset(bufferAux,0,BLOCKSIZE);
    if(bwrite(nbloque,bufferAux) == -1){
        fprintf(stderr,"[reservar_bloque()-> no se han podido guardar los 0s "
        "en el bloque: %s]\n", strerror(errno));
        return -1;
    } 
    return nbloque;
}

// Función que nos permite liberar el bloque pasado por parámetro
int liberar_bloque(unsigned int nbloque){
    struct superbloque SB;
    if (bread(POSSB,&SB) == -1){
        fprintf(stderr,"[liberar_bloque()-> no se ha podido leer el "
        "superbloque: %s]\n", strerror(errno));
        return -1;
    }
    escribir_bit(nbloque,0); //ponemos a 0 el bit del MB correspondiente al bloque nbloque
    SB.cantBloquesLibres = SB.cantBloquesLibres +1;
    if(bwrite(POSSB,&SB) == -1){
        fprintf(stderr,"[liberar_bloque()-> no se han podido guardar los datos "
        "del superbloque: %s]\n", strerror(errno));
        return -1;
    }
    return (int) nbloque;
}

// Función que escribe el contenido de una variable de tipo
// struct inodo en un determinado inodo del array de inodos.
int escribir_inodo(unsigned int ninodo, struct inodo inodo){
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    if (bread(POSSB,&SB)== -1){
        fprintf(stderr,"[escribir_inodo()-> no se ha podido leer el "
        "superbloque: %s]\n", strerror(errno));
        return -1;
    }
    //Localizamos el bloque donde se encuentra el inodo:
    int nbloque = ninodo/(BLOCKSIZE/INODOSIZE);
    if(bread((SB.posPrimerBloqueAI+nbloque),inodos) == -1){
        fprintf(stderr,"[escribir_inodo()-> no se ha podido leer los "
        "inodos del array de inodos: %s]\n", strerror(errno));
        return -1;
    }
    //Escribimos el inodo:
    inodos[ninodo%(BLOCKSIZE/INODOSIZE)] = inodo;
    // a la posición del bloque donde se encuentra el inodo 
    // que queremos escribir
    if(bwrite(SB.posPrimerBloqueAI+nbloque,inodos)==-1){
        fprintf(stderr,"[escribir_inodo()-> no se han podido guardar los "
        "inodos: %s]\n", strerror(errno));
        return -1;
    }   
    return 0;
}

// Función que lee un determinado inodo del array de inodos
int leer_inodo(unsigned int ninodo, struct inodo *inodo){
    struct superbloque SB;
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    if (bread(POSSB,&SB) == -1){
        fprintf(stderr,"[leer_inodo()-> no se ha podido leer del "
        "superbloque: %s]\n", strerror(errno));
        return -1;
    }
    //Localizamos el bloque donde se encuentra el inodo:
    int nbloque = ninodo/(BLOCKSIZE/INODOSIZE);
    if(bread(SB.posPrimerBloqueAI+nbloque,inodos) == -1){
        fprintf(stderr,"[leer_inodo() no se ha podido leer los "
        "inodos del array de inodos: %s]\n", strerror(errno));
        return -1;
    }
    //leemos el inodo:
    *inodo = inodos[ninodo%(BLOCKSIZE/INODOSIZE)];
    return 0;  
}

// Función que encuentra el primer inodo libre y lo reserva
int reservar_inodo(unsigned char tipo, unsigned char permisos){
    struct superbloque SB;
    if(bread(POSSB,&SB) == -1){
        fprintf(stderr,"[reservar_inodo()-> no se ha podido leer el "
        "superbloque: %s]\n", strerror(errno));
        return -1;
    }
    struct inodo inodo;
   // comprobamos si quedan inodos libres
    if(SB.cantInodosLibres == 0){ 
        fprintf(stderr, "[reservar_inodo()-> no hay inodos "
        "libres %s]\n", strerror(errno)); 
        return -1;
    }
    //guardamos el que antes era el primer inodo libre
    int posInodoReservado = SB.posPrimerInodoLibre;
    leer_inodo(posInodoReservado,&inodo);
    //reservamos el inodo, actualizando la lista enlazada de inodos libres
    SB.posPrimerInodoLibre = inodo.punterosDirectos[0] ;
    //inicializamos todos los campos del inodo
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
        fprintf(stderr,"[reservar_inodo()-> no se han podido guardar los datos"
        "en el superbloque: %s]\n", strerror(errno));
        return -1;
    }
    return posInodoReservado;
}
// Función que asocia un nivel a cada rango de bloques lógicos,
// devolviendo el nivel del bloque lógico indicado.
int obtener_nrangoBL(struct inodo inodo, unsigned int nblogico, 
unsigned int *ptr){
    //actualizamos el puntero para que apunte donde lo hace el puntero
    //correspondiente del inodo
    if(nblogico < DIRECTOS){
        *ptr = inodo.punterosDirectos[nblogico];
        return 0; //[0 , 11]
    }else if (nblogico < INDIRECTOS0 ){
        *ptr = inodo.punterosIndirectos[0];
        return 1; //[12 , 267]
    }else if( nblogico < INDIRECTOS1){
        *ptr = inodo.punterosIndirectos[1];
        return 2; //[268 , 65.803]
    }else if(nblogico < INDIRECTOS2){
        *ptr = inodo.punterosIndirectos[2];
        return 3; //[65.804 , 16.843.019]
    }else{
        *ptr = 0;
        fprintf(stderr, "obtener_nRangoBL()-> error al obtener el rango "
        "%s]\n", strerror(errno)); 
        return -1;
    }
}       

// Función para la obtención de los índices de los bloques de punteros
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
    return -1;  //en caso de error
}    
 
// Función encargada de obtener el nº de bloque físico correspondiente a un
// bloque lógico determinado del inodo indicado
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, 
char reservar){
    //si reservar = 0, utilizaremos traducir_bloque_inodo() para consultar
    //si reservar = 1, utilizaremos traducir_bloque_inodo() para consultar y,
    //si no existe bloque físico, también para reservar.
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
                    //printf(stderr,BLUE "[traducir_bloque_inodo() --> inodo.punterosIndirectos[%d] = %d "
                    //"(reservar BF %d para punteros_nivel%d)]\n"RESET,nRangoBL-1, ptr, 
                    //ptr,nivel_punteros );  // punteros indirectos azul
                }else{      //el bloque cuelga de otro bloque de punteros
                    buffer[indice] = ptr;  
                    //fprintf(stderr, BLUE"[traducir_bloque_inodo() --> punteros_nivel%d[%d] = %d (reservado "
                    //"BF %d para punteros_nivel%d)]\n"RESET,nivel_punteros+1,indice, ptr,ptr, nivel_punteros);
                    if(bwrite(ptr_ant, buffer) == -1){
                        //fprintf(stderr,"[traducir_bloque_inodo() --> no se han podido guardar los "
                        //"datos: %s]\n", strerror(errno));
                        return -1;
                    }
                }
            }
        }
        if (bread(ptr, buffer) == -1){
            //fprintf(stderr,"[traducir_bloque_inodo() --> no se han podido leer "
            //"el bloque físico: %s]\n", strerror(errno));
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
                //fprintf(stderr, RED "[traducir_bloque_inodo() --> inodo.punterosDirectos[%d] = %d "
                //"(reservado BF %d para BL %d)]\n"RESET,nblogico, ptr, ptr, nblogico);  //solo en punteros directos
            }else {
                buffer[indice] = ptr;   
                //fprintf(stderr,RED  "[traducir_bloque_inodo() --> punteros_nivel%d[%d] = %d "
                //"(reservar BF %d para BL %d)]\n"RESET, nivel_punteros+1,indice, ptr, ptr,nblogico);  // punteros indirectos azul
                if (bwrite(ptr_ant, buffer) == -1){
                    //fprintf(stderr,"[traducir_bloque_inodo() --> no se han podido guardar los "
                    //"datos: %s]\n", strerror(errno));
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

// Funicón para librerar todos los bloques del inodo
int liberar_inodo(unsigned int ninodo){
    struct superbloque SB;
    struct inodo inodo;
    int liberados;
    bread(POSSB,&SB);
    leer_inodo(ninodo,&inodo);
    int primerBL = 0;
    liberados = liberar_bloques_inodo(primerBL,&inodo); //llamar a la función auxiliar
    //fprintf(stderr, "[total bloques liberados: %d]\n",liberados);
    //liberar_bloques_inodo() para liberar todos los bloques del inodo
    SB.cantBloquesLibres += liberados; 
    inodo.numBloquesOcupados -= liberados; 
    if(inodo.numBloquesOcupados != 0){
        fprintf(stderr,"[liberar_inodo() → No se han liberado todos los bloques]\n");
        return -1;
    }
    inodo.tamEnBytesLog = 0;
    inodo.tipo = 'l';
    //actualizamos la lista enlazada de inodos libres
    int posSiguienteInodo = SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre =  ninodo; 
    inodo.punterosDirectos[0] = posSiguienteInodo;
    SB.cantInodosLibres ++;
    
    escribir_inodo(ninodo,inodo);
    if (bwrite(POSSB,&SB) == -1){
        fprintf(stderr,"[liberar_inodo() → no se han podido guardar los datos"
        "en el superbloque: %s]\n", strerror(errno));
        return -1;
    }
    return ninodo;
}

// Función para liberar todos los bloques ocupados
int liberar_bloques_inodo(unsigned int nblogico, struct inodo *inodo){
    unsigned int nivel_punteros, indice, ptr, ultimoBL ; 
    int bloques_punteros [3] [NPUNTEROS];  //array de bloques de punteros
    int ptr_nivel [3] ;  //punteros a bloques de punteros de cada nivel
    int indices[3]; //indices de cada nivel
    int liberados =0; //salvar_inodo ;  // nº de bloques liberados
    unsigned char bufAux_punteros[BLOCKSIZE]; 
    int nRangoBL;
    memset (bufAux_punteros, 0, BLOCKSIZE);

    if(inodo->tamEnBytesLog == 0){ // el fichero está vacío
        return 0;
    }
    //obtenemos el último bloque lógico del fichero
    if((inodo->tamEnBytesLog % BLOCKSIZE) == 0){
        ultimoBL = inodo->tamEnBytesLog/BLOCKSIZE - 1;
    }else{
        ultimoBL = inodo->tamEnBytesLog/BLOCKSIZE; 
    }
    //fprintf(stderr,"[liberar_bloques_inodo()-->primer BL: %d, último"
    //    " BL %d]\n",nblogico,ultimoBL);
    ptr = 0;
    for (unsigned int nblog = nblogico; nblog <= ultimoBL; nblog++){  //recorrido BLs 
        nRangoBL = obtener_nrangoBL(*inodo,nblog,&ptr); //0:D, 1:I0, 2:I1, 3:I2
        if(nRangoBL < 0 ){
            fprintf(stderr,"[liberar_bloques_inodo -->  error al obtener rango Bl]\n");
            return -1;
        }
        nivel_punteros = nRangoBL;   //el nivel_punteros +alto cuelga del inodo
        while(ptr > 0 && nivel_punteros > 0){    //cuelgan bloques de punteros
            indice = obtener_indice(nblog, nivel_punteros);
            if(indice == 0 || nblog == nblogico){
                //solo leemos del dispositivo si no está ya cargado en un buffer 
                if(bread(ptr,bloques_punteros[nivel_punteros-1]) < 0){ // error de lectura
                    fprintf(stderr,"[liberar_bloques_inodo --> no se ha podido leer "
                    "del bloque físico: %s]\n",strerror(errno));
                    return -1;
                }
            }
            ptr_nivel[nivel_punteros-1] = ptr;
            indices[nivel_punteros-1] = indice;
            ptr = bloques_punteros[nivel_punteros-1][indice];
            nivel_punteros--;
        }
        if (ptr > 0){  //si existe bloque de datos
            liberar_bloque(ptr);
            liberados++;
            //fprintf(stderr,"[liberar_bloques_inodo()-->liberamos BF: %d: datos del"
            //" BL %d]\n",ptr,nblog);
            if (nRangoBL == 0){  //es un puntero Directo
                inodo->punterosDirectos[nblog] = 0;
            }else{
                while( nivel_punteros < nRangoBL){
                    indice = indices[nivel_punteros];
                    bloques_punteros[nivel_punteros][indice] = 0;
                    ptr = ptr_nivel[nivel_punteros];
                    if(memcmp (bloques_punteros[nivel_punteros], bufAux_punteros, BLOCKSIZE)==0) {
                        //No cuelgan bloques ocupados, hay que 
                        //liberar el bloque de punteros
                        liberar_bloque(ptr) ;
                        liberados++;
                        nivel_punteros++;
                        if (nivel_punteros == nRangoBL) {
                            inodo->punterosIndirectos[nRangoBL-1] = 0;
                        }
                        //fprintf(stderr,"[liberar_bloques_inodo()-->liberamos BF: %d: "
                        //"bloque de punteros_nivel%d correspondiente al BL %d]\n",ptr,
                        //nivel_punteros, nblog);
                    }else{   //escribimos en el dispositivo el bloque de punteros
                    // modificado
                        if(bwrite(ptr,bloques_punteros[nivel_punteros]) == -1){
                            //fprintf(stderr,"[liberar_bloques_inodo --> no se ha podido"
                            //" escribir en el bloque físico: %s]\n",strerror(errno));
                        }
                        nivel_punteros = nRangoBL;  // para salir del bucle
                    }
                }
            }
        }
    }  
   
    return liberados;
    
}
