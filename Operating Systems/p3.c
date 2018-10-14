//GARRETT MONCRIEF
//NOTE ABOUT RUNNING: not sure if I put the sleep function in the right place
//but program takes a while to run for larger inputs due to the 1 sec sleep 
//between writes


#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdlib.h>

//key number
#define SHMKEY ((key_t) 1497)
#define BUFFSIZE 15
#define MAXCHAR 150

/*
	Source for semaphore tutorial http://www.csc.villanova.edu/~mdamian/threads/posixsem.html
*/
sem_t mutex;
sem_t empty;
sem_t full;

//create shared memory for use with cooperating processes
typedef struct
{
	char c[BUFFSIZE];
} shared_mem;

shared_mem *c; //init shared_mem total

/*
	Source for pthread tutorial: http://www.csc.villanova.edu/~mdamian/threads/posixthreads.html
*/
void *thread1(void* data);
void *thread2(void* data);

int main()
{	
	printf("\n");
	
	//semaphores
	sem_init (&mutex, 0, 1);
	sem_init (&empty, 0, BUFFSIZE);
	sem_init (&full, 0, 0);
	
	//shared mem variables
	int shmid;
	char *shmadd;
	shmadd = (char *) 0;
	
	//process creates shared memory segment
	if ((shmid = shmget (SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
	{
		perror("scmget");
		exit(1);
	}
	
	//process attaches to shared memory segment 
	if ((c = (shared_mem *) shmat (shmid, shmadd, 0)) == (shared_mem *) -1)
	{
		perror ("shmat");
		exit (0);
	}
	
	pthread_t	tid1[1];     /* process id for thread 1 */
	pthread_t	tid2[1];     /* process id for thread 2 */
	pthread_attr_t	attr[1];     /* attribute pointer array */
        	    
	fflush(stdout);
	/* Required to schedule thread independently.*/
	pthread_attr_init(&attr[0]);
	pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);  
	/* end to schedule thread independently */

   /* Create the threads */
	pthread_create(&tid1[0], &attr[0], thread1, NULL);
	pthread_create(&tid2[0], &attr[0], thread2, NULL);

   /* Wait for the threads to finish */
	pthread_join(tid1[0], NULL);
	pthread_join(tid2[0], NULL);
	
	//detaches from shared memory 
	if (shmctl(shmid, IPC_RMID, NULL)==-1)
	{
		perror ("shmdt");
		exit(-1);
	}
	//removes from system 
	shmctl (shmid, IPC_RMID, NULL);
	
	//deallocate semaphore
	sem_destroy(&mutex);
	sem_destroy(&full);
	sem_destroy(&empty);
	
	printf("\n\n");
}

void* thread1(void* data) //producer
{
	int i = 0;
	shared_mem *buffer = (shared_mem*)data;
	char newChar;
	FILE* fp;
	fp = fopen("mytest.dat", "r");
	while (fscanf(fp, "%c", &newChar) != EOF)
	{
		//if in excess of max chars will break loop
		if (i == MAXCHAR || i > MAXCHAR)
		{
			printf("\nMax char size exceeded.\n");
			break;
		}
		i++;
		
		//lock sems
		sem_wait(&empty);
		sem_wait(&mutex);
		
		c->c[i % BUFFSIZE] = newChar;
		
		//unlock sems
		sem_post(&mutex);
		sem_post(&full);
	}
	
	//producer informs consumer has placed the last char
	//lock sems
	sem_wait(&empty);
	sem_wait(&mutex);
	
	c->c[i % BUFFSIZE] = -1;
	
	//unlock sems
	sem_post(&mutex);
	sem_post(&full);
		
	pthread_exit(NULL);
}

void *thread2(void* data)
{
	int i = 0;
	shared_mem *buffer = (shared_mem*)data;
	char writeChar;
	
	
	while (writeChar != -1){
		
		sleep(1); //consumer runs slower, advised to add 1 second sleep
		
		//lock sems
		sem_wait(&full);
		sem_wait(&mutex);
		
		writeChar = c->c[i % BUFFSIZE];
		i++;
		
		if (writeChar != -1)
			printf("%c", writeChar);
		
		//unlock sems
		sem_post(&mutex);
		sem_post(&empty);
		
	}
	pthread_exit(NULL);
}