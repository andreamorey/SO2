#include "directorios.h"

int extraer_camino(const char *camino, char *inicial, 
char *final, char *tipo){
    // comprobar que el camino no es nulo
    //fprintf(stderr, "camino para extraer: %s\n", camino);
    if(camino == NULL || strcmp(camino, "") == 0){
        fprintf(stderr, "ERROR en extraer_camino: camino es nulo. \n");
        return -1;
    }
    int x;
    if(camino[0] != '/'){
        return -1;  // camino incorrecto o directorio 
    }
    for ( x=1; (camino[x] != '/') && (camino[x] != 0); x++){// copiamos el primer fichero
    //while((camino[x] != '/') && (camino[x] != 0)){
        inicial[x - 1] = camino [x]; 
       // x++;
    }
    if( x == strlen(camino)){  // es un fichero
        final[0] = 0;       // strcpy(final, **)
        *tipo = 'f';        // devolvemos 1 que es un fichero
    }else{
        final = strcpy(final, &camino[x]); // copiamos el resto
        *tipo = 'd';
    }
    return 0;

/*
    // a lo mejor hay que tratar el error de /
    strncpy(inicial, *(camino+1), strlen(camino)-1);  // inicial = fichero.txt
    final = strchr(inicial, '/');
    if (final == NULL){
        tipo = 'f';
    }else{
        int numCar = strlen(inicial);
        numCar -= strlen(final);
        strncpy(inicial, inicial, numCar);
        tipo = 'd';
    }
    return tipo;
    */
}

int buscar_entrada(const char *camino_parcial, 
unsigned int *p_inodo_dir, unsigned int *p_inodo, 
unsigned int *p_entrada, char reservar, unsigned char permisos){
    if (strcmp(camino_parcial, "/") == 0){ // si es el directorio raiz
        *p_inodo = 0; //0 = SB.posInodoRaiz
        *p_entrada = 0;
        return 0;
    }
    
    struct entrada entrada;
    //Inicializamos variables con el tamaño del campo nombre de la struct entrada
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    
    
    // Limpiamos incial y final
    memset(inicial, 0 , sizeof(entrada.nombre));
    memset(final, 0, sizeof(strlen(camino_parcial)));
    
    char tipo;
    if (extraer_camino(camino_parcial, inicial, final, &tipo) < 0){
        return ERROR_CAMINO_INCORRECTO;
    }
    fprintf(stderr, "[buscar_entrada() -> inicial: %s, final: %s, reservar: %d]\n", inicial, final, reservar);
    
    //buscamos la entrada cuyo nombre se encuentra en inicial

    struct inodo inodo_dir;
    leer_inodo(*p_inodo_dir, &inodo_dir);
    if((inodo_dir.permisos & 4) != 4){
        fprintf(stderr,"[buscar_entrada() -> El inodo %d no tiene permisos de lectura]\n", *p_inodo_dir);
        return ERROR_PERMISO_LECTURA;
    }
    //el buffer de lectura puede ser un struct tipo entrada
    struct entrada buffer_entradas[BLOCKSIZE/sizeof(struct entrada)];
    
    //Calcular cant entradas inodo
    int cant_entradas_inodo, num_entrada_inodo;
    
    memset(entrada.nombre, 0, sizeof(entrada.nombre));
    cant_entradas_inodo = inodo_dir.tamEnBytesLog / sizeof(struct entrada);
    
    //fprintf(stderr, "[buscar_entrada() -> cant_entradas_inodo: %d\n", cant_entradas_inodo);

    num_entrada_inodo = 0; // nº de entrada incial
    if(cant_entradas_inodo > 0 ){
    memset(buffer_entradas, 0, sizeof(buffer_entradas));
   //Leemos la entrada
    if(mi_read_f(*p_inodo_dir, buffer_entradas, num_entrada_inodo * sizeof(struct entrada),
        sizeof(buffer_entradas)) == -1){ // leer entrada
        fprintf(stderr, "[Error-> buscar_entrada(): no se ha podido leer la entrada correcta]\n");
        return -1;
    }
    int cant_entradas_buffer = BLOCKSIZE / sizeof(struct entrada);
    int num_entrada_buffer = num_entrada_inodo % cant_entradas_buffer;

    memcpy(&entrada, &buffer_entradas[num_entrada_buffer],sizeof(struct entrada));
    //mira dentro del inodo si hay la entrada q buscamos ( q la tenemos en "inicial")
    
    while((num_entrada_inodo < cant_entradas_inodo) && strcmp(inicial,entrada.nombre) != 0){
        num_entrada_inodo++;
        num_entrada_buffer = num_entrada_inodo % cant_entradas_buffer;
        if(num_entrada_buffer == 0){
            memset(buffer_entradas, 0, sizeof(buffer_entradas));
            if (mi_read_f(*p_inodo_dir, buffer_entradas, num_entrada_inodo*sizeof(struct entrada),
            sizeof(buffer_entradas))  == -1){  // leer entrada
                fprintf(stderr,"Error: directorios.c --> buscar entrada() mi read f()\n");
                return -1;
                } 
                
            }
            memcpy(&entrada, &buffer_entradas[num_entrada_buffer],sizeof(struct entrada));
        }
       // fprintf(stderr, "[buscar_entrada()-> entrada.nombre: %s, entrada.ninodo: %d\n", entrada.nombre, entrada.ninodo);

    }
    if((num_entrada_inodo == cant_entradas_inodo) && (strcmp(entrada.nombre, inicial) != 0)){ // la entrada no existe
        switch(reservar){
            case 0: // modo consulta. como no existe retornamos error
                return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            case 1:
                if(inodo_dir.tipo == 'f'){ // no podemos crear entradas dentro de un fichero
                    return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
                }
                if ((inodo_dir.permisos & 2) != 2){ // si no tienen permiso de escritura
                    return ERROR_PERMISO_ESCRITURA;
                }else{
                    //creamos la entrada en el directorio referenciado por el *p_inodo_dir
                    strcpy(entrada.nombre, inicial);
                  //  fprintf(stderr,"[buscar_entrada() -> entrada.nombre: %s, ", inicial);
                    if(tipo == 'd'){ // directorio
                        if(strcmp(final,"/") == 0){ // reservamos inodo tipo directorio
                            entrada.ninodo = reservar_inodo('d', permisos);
                            fprintf(stderr,"[buscar_entrada()->  reservado inodo %d tipo %c "
                            "con permisos %u para %s]\n",entrada.ninodo, tipo, permisos, inicial);
                            fprintf(stderr, "[buscar_entrada()-> creada entrada: %s,"
                            " %d]\n", entrada.nombre, entrada.ninodo);
                        }else { // no existe el directorio intermedio
                            //fprintf(stderr)

                            return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                        }
                    }else{// reservamos inodo tipo fichero
                        entrada.ninodo = reservar_inodo('f', permisos);
                        fprintf(stderr,"[buscar_entrada()->  reservado inodo %d tipo %c "
                        "con permisos %u para %s]\n",entrada.ninodo, tipo, permisos, inicial);
                        fprintf(stderr, "[buscar_entrada()-> creada entrada: %s,"
                        " %d]\n", entrada.nombre, entrada.ninodo);
        
                    }   //escribimos el inodo correspondiente a la nueva entrada de directorio
                    if(mi_write_f(*p_inodo_dir, &entrada, num_entrada_inodo*sizeof(struct entrada), sizeof(struct entrada)) == -1){ //entramos si ha habido fallo
                        if(entrada.ninodo != -1 ){ //si la entrada ya tenía un ninodo, lo liberamos porq ya
                            //le hemos reservado otro más arriba
                            liberar_inodo(entrada.ninodo);
                        }
                        fprintf(stderr,"Error: directorios.c-> buscar_entrada() -> mi_write_f\n");
                        return -1;
                    }
                }
        }

    }
    if(strcmp(final, "/") == 0 ||  strcmp(final, "") == 0){ //hemos llegado al final del camino
        if((num_entrada_inodo < cant_entradas_inodo) && (reservar == 1) ){ // modo escritura 
                                                                    //y la entrada ya existe
            return ERROR_ENTRADA_YA_EXISTENTE;
        }
        // cortamos la recursividad
        *p_inodo = entrada.ninodo;
        *p_entrada = num_entrada_inodo;
        return 0;
    }else{
        *p_inodo_dir = entrada.ninodo;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }
}

void mostrar_error_buscar_entrada(int error){
    switch(error){
        case -1: fprintf(stderr, RED"Error: Camino incorrecto.\n"RESET); break;
        case -2: fprintf(stderr, RED"Error: Permiso denegado de lectura.\n"RESET); break;
        case -3: fprintf(stderr, RED"Error: No existe el archivo o el directorio.\n"RESET); break;
        case -4: fprintf(stderr, RED"Error: No existe algún directorio intermedio.\n"RESET); break;
        case -5: fprintf(stderr, RED"Error: Permiso denegado de escritura.\n"RESET); break;
        case -6: fprintf(stderr, RED"Error: El archivo ya existe.\n"RESET); break;
        case -7: fprintf(stderr, RED"Error: No es un directorio.\n"RESET); break;
    }
}

int mi_creat(const char *camino, unsigned char permisos){
    unsigned int p_inodo_dir = 0 , p_inodo,  p_entrada;
    int error = buscar_entrada(camino,&p_inodo_dir, &p_inodo, &p_entrada,1,permisos);
    mostrar_error_buscar_entrada(error);
    return 0;
}

 int mi_dir(const char *camino, char *buffer){
    unsigned int ninodo_dir= 0 , ninodo, nentrada;
    struct inodo inodo;
    int cantEntradas, entradasLeidas=0;

    int error = buscar_entrada(camino,&ninodo_dir, &ninodo, &nentrada,0,7);
    if(error < 0){
        mostrar_error_buscar_entrada(error);
        return .1;
    }

    //leemos el inodo
    leer_inodo(ninodo, &inodo);

    if((inodo.permisos & 4) != 4){ 
        fprintf(stderr,"Error. El directorio no tiene permisos de lectura\n");
        return -1;
    }
    struct entrada entradas[BLOCKSIZE/sizeof(struct entrada)];
    
    if(inodo.tipo == 'd'){
                cantEntradas = inodo.tamEnBytesLog / sizeof(struct entrada);
    }else{
        cantEntradas = 1;
    }

    while(entradasLeidas < cantEntradas){
        mi_read_f(ninodo,&entradas, entradasLeidas*sizeof(struct entrada),sizeof(entradas));
        for(int i = 0; i<16 && entradasLeidas < cantEntradas ; i++){
            if(inodo.tipo == 'd'){
                int inodoentrada= entradas[i].ninodo;
                leer_inodo(inodoentrada,&inodo);
            }
            

            char tmp[100];
            if(inodo.tipo == 'd'){
                sprintf(tmp, YELLOW_S"%c"RESET, inodo.tipo);
            }else{
                sprintf(tmp,BLUE_S"%c"RESET, inodo.tipo);
            }
            strcat(buffer,tmp);
            strcat(buffer,"\t");

            if(inodo.permisos & 4){
                strcat(buffer,"r");
            }else{
                strcat(buffer,"-");
            }
            if(inodo.permisos & 2) {
                strcat(buffer,"w");
            }else{
                strcat(buffer,"-");
            } 
            if(inodo.permisos & 1) {
                strcat(buffer,"x");
            } else{
                strcat(buffer,"-");
            }

            strcat(buffer,"\t\t");

            struct tm *tm;
            tm = localtime(&inodo.mtime);
            sprintf(tmp,"%d-%02d-%02d %02d:%02d:%02d",tm->tm_year+1900,
                            tm->tm_mon+1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
            strcat(buffer,tmp);
            strcat(buffer,"\t");

            //Concatenamos el tamaño
            char tamano[12];
            sprintf(tamano,"%d",inodo.tamEnBytesLog);

            strcat(buffer,tamano);
            strcat(buffer,"\t");

            //Concatenamos el nombre
            strcat(buffer,entradas[0].nombre);
            strcat(buffer,"|");
            entradasLeidas++;
        }

    }

    if(entradasLeidas!=cantEntradas){
        fprintf(stderr,"Error en la cantidad de entradas leídas");
    }

    return entradasLeidas;
}



 
int mi_chmod(const char *camino, unsigned char permisos){
    unsigned int p_inodo_dir = 0, p_inodo, p_entrada;
    int error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,permisos);
    if (error != ERROR_NO_EXISTE_ENTRADA_CONSULTA){
        mi_chmod_f(p_inodo,permisos);
        return 0;
    }
    mostrar_error_buscar_entrada(error);
    return 0;
}

int mi_stat(const char *camino, struct STAT *p_stat){
    unsigned int p_inodo, p_entrada, p_inodo_dir=0;
    int error = buscar_entrada(camino,&p_inodo_dir,&p_inodo,&p_entrada,0,0);
    if (error != ERROR_NO_EXISTE_ENTRADA_CONSULTA){
        mi_stat_f(p_inodo,p_stat);
        fprintf(stderr,"Número del inodo: %d\n", p_inodo);
        return 0;
    }
    return 0;
}
