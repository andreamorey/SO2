#define NUMPROCESOS 3
#define ESCRITURA 10
#include "simulacion.h"

int acabados = 0;



void reaper(){
    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended=waitpid(-1, NULL, WNOHANG))>0) {
        acabados++;
        fprintf(stderr,"[simulación.c → Acabado proceso con PID %d, total acabados: %d]\n", ended , acabados);
    
    //Podemos testear qué procesos van acabando:
    //fprintf(stderr, "acabado: %d total acabados: %d\n", ended, acabados);
    }

}

int main(int argc, char **argv){
    //llamamos al enterrador para eliminar los hijos zombie
    signal(SIGCHLD,reaper);
    char * colores [] = {RED, BLUE, GREEN};
    pid_t pid;
    struct REGISTRO registro;


    if (argc!=2){
        fprintf(stderr,"Sintaxis: ./simulacion <disco>\n");
        return 0;
    }
    
    if (bmount(argv[1]) == -1){
        // no se ha podido abrir
        return -1;
    }
    fprintf(stderr, "*** Simulación de 3 procesos realizando cada uno 10 escrituras ***\n");
    // crear el directorio de simulación simul_aaaammddhhmmss
    char camino[25];
    char tiempo[80];
    time_t actual = time(NULL); //equivalente a time(&actual)
    struct tm *ts;
    ts =localtime(&actual);
    strftime(tiempo, sizeof(tiempo),"%Y%m%d%H%M%S",ts);
    sprintf(camino, "/simul_%s/", tiempo);


    if (mi_creat(camino , 7) == -1){
        fprintf(stderr, "Error en crear directorio simulación %s\n", camino);
        bumount();
        return -1;
    }
    
    
    for (int proceso= 1; proceso <= NUMPROCESOS; proceso++ ){
        pid = fork();
        if (pid == 0){
            //hijo
          //  printf("Soy el hijo\n");
            if (bmount(argv[1]) == -1){
                return -1;
            }

            sprintf(camino, "%sproceso_%d/", camino, getpid()); 
            
            if (mi_creat(camino , 6) == -1){
                // error al crear el directorio al crear el fichero
                bumount();
                return -1;
            }

            sprintf(camino, "%sprueba.dat",camino);
            if (mi_creat(camino , 6) == -1){
                bumount();
                return -1;
            }
            srand(time(NULL)+getpid());
            
            for(int i = 1; i<= ESCRITURA; i++){
                registro.fecha=time(NULL); 
                registro.pid = getpid(); 
                registro.nEscritura = i+1; 
                registro.nRegistro =  rand() % REGMAX;  

                mi_write(camino,&registro, registro.nRegistro * 
                    sizeof(struct REGISTRO), sizeof(struct REGISTRO));
                    fprintf(stderr,"%s""[simulación.c → Escritura %d en %s]\n"RESET, colores[proceso-1],i, camino);
                usleep(50000); 
            }
            fprintf(stderr, "Proceso %d: Completadas %d escrituras en %s\n", proceso, ESCRITURA, camino);
            bumount();
            exit(0); //Necesario para que se emita la señal SIGCHLD 
           
        }
         usleep(200000);
    }    
    
        //Permitir que el padre espere por todos los hijos: 
        while (acabados < NUMPROCESOS){
            pause();
        }
        bumount();
        exit(0);
    
    
}

