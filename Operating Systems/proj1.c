//GARRETT MONCRIEF
//ASSIGNMENT 1

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

//key number
#define SHMKEY ((key_t) 1497)

//create shared memory for use with cooperating processes
typedef struct
{
	int value;
} shared_mem;

shared_mem *total;

void process1(shared_mem *total);
void process2(shared_mem *total);
void process3(shared_mem *total);
void process4(shared_mem *total);

int main()
{
	int shmid, pid1, pid2, pid3, pid4, ID, status;
	char *shmadd;
	shmadd = (char *) 0;
	
	//process creates shared memory segment
	if ((shmid = shmget (SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
	{
		perror("scmget");
		exit(1);
	}
	
	//process attaches to shared memory segment 
	if ((total = (shared_mem *) shmat (shmid, shmadd, 0)) == (shared_mem *) -1)
	{
		perror ("shmat");
		exit (0);
	}
	
	//shared memory init to 0
	total->value = 0;
	
	//fork child 1
	if ((pid1 = fork()) == 0){
		process1(total);
		exit(0);
	}
	
	//fork child 2
	else if ((pid2 = fork()) == 0){
		process2(total);
		exit(0);
	}

	//fork child 3
	else if ((pid3 = fork()) == 0){
		process3(total);
		exit(0);
	}

	//fork child 4
	else if ((pid4 = fork()) == 0){
		process4(total);
		exit(0);
	}
	
	//parent wait for children to end 
	waitpid (pid1, NULL, 0);
	waitpid (pid2, NULL, 0);
	waitpid (pid3, NULL, 0);
	waitpid (pid4, NULL, 0);
	
	printf("\n\n");
	printf("Child with ID: %d has just exited.\n", pid1);
	printf("Child with ID: %d has just exited.\n", pid2);
	printf("Child with ID: %d has just exited.\n", pid3);
	printf("Child with ID: %d has just exited.\n", pid4);
	printf("\n\nEnd of Program\n\n");	
	
	//detaches from shared memory 
	if (shmdt(total)==-1)
	{
		perror ("shmdt");
		exit(-1);
	}
	
	//removes from system 
	shmctl (shmid, IPC_RMID, NULL);

}

void process1(shared_mem *total)
{
	int i;
	for (i = 0; i < 100000; i++)
		total->value += 1;
	printf("From Process 1: counter = %d\n", total->value);
}

void process2(shared_mem *total)
{
	int i;
	for (i = 0; i < 200000; i++)
		total->value += 1;
	printf("From Process 2: counter = %d\n", total->value);
}

void process3(shared_mem *total)
{
	int i;
	for (i = 0; i < 300000; i++)
		total->value += 1;
	printf("From Process 3: counter = %d\n", total->value);
}

void process4(shared_mem *total)
{
	int i;
	for (i = 0; i < 500000; i++)
		total->value += 1;
	printf("From Process 4: counter = %d\n", total->value);
}