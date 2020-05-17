#include "semaforo_mutex_posix.h"
#include <fcntl.h> 

sem_t *initSem(){
    sem_open(SEM_NAME,O_CREAT);
}
void deleteSem(){
    sem_unlink(SEM_NAME);

}
void signalSem(sem_t *sem){
    sem_post(sem);
}
void waitSem(sem_t *sem){
    sem_wait(sem);
}