#include "ficheros_basicos.h"

struct inodo inodo;
int ninodo;

int main(int argc, char**argv){
    if (argc != 2){
        return EXIT_FAILURE;
    }
    char* nombre_dispositivo = argv[1];
    bmount(nombre_dispositivo);
    struct superbloque SB;
    bread(POSSB,&SB);
    printf("\nDATOS DEL SUPERBLOQUE\n");
    printf("Pos primer bloque MB %d\n", SB.posPrimerBloqueMB);
    printf("Pos último bloque MB %d\n", SB.posUltimoBloqueMB);
    printf("Pos primer bloque AI %d\n", SB.posPrimerBloqueAI);
    printf("Pos último bloque AI %d\n", SB.posUltimoBloqueAI);
    printf("Pos primer bloque datos %d\n", SB.posPrimerBloqueDatos);
    printf("Pos último bloque datos %d\n", SB.posUltimoBloqueDatos);
    printf("Pos inodo raiz %d\n", SB.posInodoRaiz);

    printf("Pos primer inodo libre %d\n", SB.posPrimerInodoLibre);
    printf("cantidad Bloques Libres %d\n", SB.cantBloquesLibres);
    printf("cantidad Inodos Libres %d\n", SB.cantInodosLibres);    
    printf("tot bloques %d \n", SB.totBloques);
    printf("tot inodos %d\n\n", SB.totInodos);

    //printf("sizeof struct superbloque is: %lu\n", sizeof(struct superbloque)); //maybe directmnt BLOCKDIZe
    //printf("sizeof struct inodo is: %lu\n\n", sizeof(struct inodo));
    /*
    printf("\nRESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS\n");
    int bloqueReservado = reservar_bloque();
    printf("Se ha reservado el bloque físico nº %i que era ", bloqueReservado);
    printf("el 1º libre indicado por el MB\n");
    bread(POSSB, &SB);
    printf("SB.cantBloquesLibres = %i \n", SB.cantBloquesLibres);
    liberar_bloque(bloqueReservado);
    bread(POSSB, &SB);
    printf("Liberamos ese bloque y después SB.cantBloquesLibres = %i\n",
        SB.cantBloquesLibres);

    int posicion = POSSB;
    printf("\nMAPA DE BITS CON BLOQUES DE METADATOS OCUPADOS\n");
    unsigned char bit = leer_bit(0); //0
    printf("valor de bit de posSB (BF nº%i) = %i \n\n", posicion,bit);
    posicion = SB.posPrimerBloqueMB;
    bit = leer_bit(SB.posPrimerBloqueMB); //1
    printf("valor de bit de posPrimerBloqueMB (BF nº%i) = %i \n\n",posicion, bit);
    posicion = SB.posUltimoBloqueMB;
    bit = leer_bit(SB.posUltimoBloqueMB); //13
    printf("valor de bit de posUltimoBloqueMB (BF nº%i) = %i \n\n",posicion, bit);
    posicion = SB.posPrimerBloqueAI;
    bit = leer_bit(SB.posPrimerBloqueAI); //14
    printf("valor de bit de posPrimerBloqueAI (BF nº%i) = %i \n\n",posicion, bit);
    posicion = SB.posUltimoBloqueAI;
    bit = leer_bit(SB.posUltimoBloqueAI); //3138
    printf("valor de bit de posUltimoBloqueAI (BF nº%i) = %i \n\n",posicion, bit);
    posicion = SB.posPrimerBloqueDatos;
    bit = leer_bit(SB.posPrimerBloqueDatos); //3139
    printf("valor de bit de posPrimerBloqueDatos (BF nº%i) = %i \n\n",posicion, bit);
    posicion = SB.posUltimoBloqueDatos;
    bit = leer_bit(SB.posUltimoBloqueDatos); //99999
    printf("valor de bit de posUltimoBloqueDatos (BF nº%i) = %i \n\n",posicion, bit);

    printf("DATOS DEL DIRECTORIO RAIZ\n");
    struct inodo inodoRaiz;
    leer_inodo(SB.posInodoRaiz, &inodoRaiz);
    printf("tipo: %c\n", inodoRaiz.tipo);
    printf("permisos: %i\n", inodoRaiz.permisos);
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];
    ts = localtime(&inodoRaiz.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodoRaiz.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodoRaiz.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    printf("atime: %s \nmtime: %s \nctime: %s\n",atime,mtime,ctime);
    printf("nlinks: %i\n", inodoRaiz.nlinks);
    printf("tamEnBytesLog: %i\n", inodoRaiz.tamEnBytesLog);
    printf("numBloquesOcupados: %i\n", inodoRaiz.numBloquesOcupados);
    */
   /* printf("INODO 1. TRADUCCIÓN DE LOS BLOQUES LOGICOS 8, 204 30.004, 400.004, 16.843.019\n\n");
    int posInodoReservado = reservar_inodo('f',6);
    traducir_bloque_inodo(posInodoReservado,8,'1');

    traducir_bloque_inodo(posInodoReservado,204,'1');

    traducir_bloque_inodo(posInodoReservado,30004,'1');
    traducir_bloque_inodo(posInodoReservado,400004,'1');
    traducir_bloque_inodo(posInodoReservado,16843019,'1');

*//*
    printf("\n\nDATOS DEL INODO RESEVADO 1\n\n");
    struct inodo inodoReservado;
    leer_inodo(posInodoReservado, &inodoReservado);
    printf("tipo: %c\n", inodoReservado.tipo);
    printf("permisos: %i\n", inodoReservado.permisos);
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];
    ts = localtime(&inodoReservado.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodoReservado.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodoReservado.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    printf("atime: %s \nmtime: %s \nctime: %s\n",atime,mtime,ctime);
    printf("nlinks: %i\n", inodoReservado.nlinks);
    printf("tamEnBytesLog: %i\n", inodoReservado.tamEnBytesLog);
    printf("numBloquesOcupados: %i\n", inodoReservado.numBloquesOcupados);
*/

    bumount();
}