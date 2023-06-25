#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "linkedlist.h"

typedef struct Client {
    int id;
    pthread_cond_t turnCondition;
    pthread_mutex_t turnMutex;
	sem_t wasCut;
    struct Client* next;
} Client;

Client* createClient(int id);
Client* addClient(Client** head, int id);
Client* copyFirstElement(Client* head);
void popClient(Client** head);
void printClientList(Client* head);

#endif
