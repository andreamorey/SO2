#include "ficheros_basicos.h"
int main(int argc, char**argv){
    if (argc != 2){
        return EXIT_FAILURE;
    }
    char* nombre_dispositivo = argv[1];s
    bmount(nombre_dispositivo);
    struct superbloque SB;
    bread(POSSB,&SB);
    printf("DATOS DEL SUPERBLOQUE\n");
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
    printf("sizeof struct superbloque is: %lu\n", sizeof(struct superbloque)); //maybe directmnt BLOCKDIZe
    printf("sizeof struct inodo is: %lu\n\n", sizeof(struct inodo));
    
    printf("RECORRIDO LISTA ENLAZADA DE NODOS LIBRES\n");
    for (int i = SB.posPrimerInodoLibre;i<SB.cantInodosLibres; i++){
        printf("%d ", i);
    }
    bumount();
}