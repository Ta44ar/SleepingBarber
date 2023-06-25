#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>

#include "argsFunctions.h"
// numChairs, debugMode

#include "linkedlist.h"

sem_t clientReady;
pthread_mutex_t waitroomAccess = PTHREAD_MUTEX_INITIALIZER;

int numberOfWaitingClients = 0;
int numberOfResignedClients = 0;
int totalNumberOfClients = 0;
int currentServedClientId = 0;

Client *waitroomQueue = NULL;
Client *resignedClients = NULL;

void printDebug()
{
	printf("Waiting: ");	
	printClientList(waitroomQueue);
	printf("Resigned: ");	
	printClientList(resignedClients);
	printf("\n\n");
}

void printInfo()
{
	if (currentServedClientId != 0)
	{
		printf("Res: %d  WRoom: %d/%d  [in: %d]\n", numberOfResignedClients, numberOfWaitingClients, numChairs, currentServedClientId);
	}
	else
	{
		printf("Res: %d  WRoom: %d/%d  [in: -]\n", numberOfResignedClients, numberOfWaitingClients, numChairs);
	}
	if (debugMode)
	{
		printDebug();
	}
}

void getCut(Client *newClient)
{
	if (sem_wait(&newClient->wasCut) == -1) perror("sem_wait error");
    sem_destroy(&newClient->wasCut);
}

void doCut(Client *newClient)
{
	int i = rand()%4;
	sleep(i);
	sem_post(&newClient->wasCut);

}

void *BarberFunction()
{
	while(1){
        if (sem_wait(&clientReady) == -1) perror("sem_wait error");
        
        pthread_mutex_lock(&waitroomAccess);
		numberOfWaitingClients--;
        Client *currentServedClient = copyFirstElement(waitroomQueue);
		currentServedClientId = currentServedClient->id;
		popClient(&waitroomQueue);
        printInfo();
        pthread_mutex_unlock(&waitroomAccess);
        sem_post(&currentServedClient->turn);
        doCut(currentServedClient);

        currentServedClientId = 0;
	}				
}

void ClientFunction()
{
    pthread_mutex_lock(&waitroomAccess);
    totalNumberOfClients++;
    int id = totalNumberOfClients;

    if (numberOfWaitingClients < numChairs){
        numberOfWaitingClients++;
        Client* newClient = addClient(&waitroomQueue, id);
        printInfo();
        sem_post(&clientReady);
        pthread_mutex_unlock(&waitroomAccess);
        if (sem_wait(&newClient->turn) == -1) perror("sem_wait error");;
        sem_destroy(&newClient->turn);
        getCut(newClient);
    } else {
        addClient(&resignedClients, id);
        numberOfResignedClients++;
        printInfo();
        pthread_mutex_unlock(&waitroomAccess);
    }
}

void *ClientGenerator()
{
    int tmpStatus;
    int i = 0;
    while (1)
    {
        pthread_t client;
        tmpStatus = pthread_create(&client, NULL, (void *)ClientFunction, &i);
        if (tmpStatus){
            fprintf(stderr, "Client thread with id [%d] couldn't start!'\n", i);
        }
        
        i += 1;

        sleep(rand()%3);
    }
}

int main(int argc, char *argv[]){
    int tmpStatus;
    srand(time(NULL));
    argsHandler(argc, argv);

    pthread_t barber, clientGenerator;

    if (sem_init(&clientReady, 0, 0)){
        perror("sem_init clientReady error");
        exit(EXIT_FAILURE);
    }

    tmpStatus = pthread_create(&barber, NULL, (void*)BarberFunction, NULL);
	if (tmpStatus !=0 )
	{
		fprintf(stderr, "Failed to create BarberFunction thread;\n ERROR: %s\n", strerror(tmpStatus));
	}
    tmpStatus = pthread_create(&clientGenerator, NULL, (void*)ClientGenerator, NULL);
	if (tmpStatus !=0 )
	{
		fprintf(stderr, "Failed to create Clientenerator thread;\n ERROR: %s\n", strerror(tmpStatus));
	}


    pthread_join(barber, NULL);
    pthread_join(clientGenerator, NULL);

    pthread_mutex_destroy(&waitroomAccess);
    sem_destroy(&clientReady);
}
