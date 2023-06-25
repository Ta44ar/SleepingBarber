#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "argsFunctions.h"
// numChairs, debugMode

#include "linkedlist.h"

pthread_cond_t clientReadyCondition;
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
    pthread_mutex_lock(&newClient->wasCutMutex);
    while (!currentServedClientId==newClient->id) {
        pthread_cond_wait(&newClient->wasCutCondition, &newClient->wasCutMutex);
    }
    pthread_mutex_unlock(&newClient->wasCutMutex);
    pthread_mutex_destroy(&newClient->wasCutMutex);
}

void doCut(Client *newClient)
{
	int i = rand()%4;
	sleep(i);
	pthread_mutex_lock(&newClient->wasCutMutex);
    pthread_cond_signal(&newClient->wasCutCondition);
    pthread_mutex_unlock(&newClient->wasCutMutex);
}

void *BarberFunction()
{
	while(1){
        pthread_mutex_lock(&waitroomAccess);
        while (!numberOfWaitingClients) {
            pthread_cond_wait(&clientReadyCondition, &waitroomAccess);
        }
		numberOfWaitingClients--;
        Client *currentServedClient = copyFirstElement(waitroomQueue);
		currentServedClientId = currentServedClient->id;
		popClient(&waitroomQueue);
        printInfo();
        pthread_mutex_unlock(&waitroomAccess);

        pthread_mutex_lock(&currentServedClient->turnMutex);
        pthread_cond_signal(&currentServedClient->turnCondition);
        pthread_mutex_unlock(&currentServedClient->turnMutex);

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
        pthread_cond_signal(&clientReadyCondition);
        pthread_mutex_unlock(&waitroomAccess);

        pthread_mutex_lock(&newClient->turnMutex);
        while (!currentServedClientId==newClient->id) {
            pthread_cond_wait(&newClient->turnCondition, &newClient->turnMutex);
        }
        pthread_mutex_unlock(&newClient->turnMutex);
        pthread_mutex_destroy(&newClient->turnMutex);
        getCut(newClient);
        
        
    } else{
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
}
