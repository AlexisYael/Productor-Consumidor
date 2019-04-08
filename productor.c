#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>

int crearMemoria(int **num);
void eliminarMemoria(int *num,int shmid);
sem_t *crearSemaforo(char *name);
void eliminarSemaforo(sem_t *sem,char *name);

int main(int argc, char const *argv[])
{
    sem_t *sem,*sem2;
    int *num=NULL;
    int shmid;
    sem=crearSemaforo("/semP");
    sem2=crearSemaforo("/semH");
    shmid=crearMemoria(&num);
    /* Empezamos a producir */
    for(int i=0;i<10;i++)
    {
        sem_post(sem);
        sem_wait(sem2);
        *num+=i;
        printf("Los datos escritos son: %d.\n",*num);
        sleep(1);
    }
    
    return 0;
}
/* FUNCIÓN QUE CREA LA MEMORIA COMPARTIDA */
int crearMemoria(int **num)
{
    int shmid;
    key_t key=ftok("mem",'P');
    if ((shmid=shmget(key,sizeof(int),IPC_CREAT | 0600))==-1)
    {
        if(errno==EEXIST)
        {
            printf("La memoria existe.\n");
            if (shmid=shmget(key,sizeof(int),IPC_EXCL)==-1) 
            {
                printf("Error al conectar a la memoria.\n");
                exit(-1);
            }
            else
            {
                printf("Se conecto a la memoria: %d.\n",shmid);
            }    
        }
        else 
        {
            printf("Error al crear la memoria.\n");
            exit(-1);
        }
    }
    else
    {
        printf("Se creo la memoria: %d.\n",shmid);
    }
    // Atamos la memoria creada 
    if ((*num=shmat(shmid,0,0))==NULL) 
    {
        printf("Error al atar la memoria.\n");
        exit(-1);
    }
    return shmid;   
}
/* FUNCIÓN QUE CREA UN SEMAFORO CON BASE A SU NOMBRE */
sem_t *crearSemaforo(char *name)
{
    sem_t *sem; 
    if ((sem=sem_open(name,O_CREAT | O_EXCL,S_IRUSR | S_IWUSR,1)) == SEM_FAILED) 
    {
        printf("Error al crear el semaforo.\n");
        if (errno==EEXIST) 
        {
            printf("El semaforo existe.\n");
            if ((sem=sem_open(name,S_IRUSR | S_IWUSR)) == SEM_FAILED) 
            {
                printf("Error al conectar semaforo.\n");
                exit(-1);
            }
            else
            {
                printf("Se ligo al semaforo: %d.\n",*sem);
            } 
        }
        else
        {
            exit(-1);
        }
    }
    else
    {
        printf("Se creo el semaforo: %d.\n",*sem);
    }
    return sem;
}
/* FUNCIÓN QUE PERMITE ELIINAR LA MEMORIA COMPARTIDA DADO SU DIRECCIÓN */
void eliminarMemoria(int *num,int shmid)
{
    shmdt(num);
    if (shmctl(shmid,IPC_RMID,0)==0) 
    {
        printf("Segmento de memoria eliminado correctamente.\n");
    }
    else
    {
        printf("Error al eliminar el segmento de memoria.\n");
        exit(-1);
    }
}
/* FUNCIÓN QUE PERMITE ELIMINAR UN SEMAFORO DADO EL SEMAFORO Y SU NOMBRE */
void eliminarSemaforo(sem_t *sem,char *name)
{
    if(sem_close(sem)==0)
    {
        printf("Se cerro correctamente el semaforo.\n");
        if(sem_unlink(name)==0)
        {
            printf("Se borro corectamente el combre del semaforo.\n");
        }
        else
        {
            printf("Error al eliminar el nombre del semaforo.\n");
            exit(-1);
        }
    }
    else
    {
        printf("Error al cerrar el semaforo.\n");
        exit(-1);
    }
    
}
