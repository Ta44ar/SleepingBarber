#include <stdio.h>
#include <stdlib.h> 
#include <semaphore.h>
#include <pthread.h>
#include "linkedlist.h"

Client* createClient(int id) {
    Client* newClient = (Client*)malloc(sizeof(Client));
    newClient->id = id;
    newClient->next = NULL;
    return newClient;
}

Client* addClient(Client** head, int id) {
    Client* newClient = createClient(id);

    if (*head == NULL) {
        *head = newClient;
        return newClient;
    }

    Client* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newClient;

    return newClient;
}

Client* copyFirstElement(Client* head) {
    if (head == NULL) {
        return NULL;
    }

    Client* copiedClient = (Client*)malloc(sizeof(Client));
    if (copiedClient == NULL) {
        return NULL;
    }

    copiedClient->id = head->id;
    copiedClient->turnCondition = head->turnCondition;
    copiedClient->turnMutex = head->turnMutex;
    copiedClient->wasCutCondition = head->wasCutCondition;
    copiedClient->wasCutMutex = head->wasCutMutex;
    copiedClient->next = NULL;

    return copiedClient;
}

void popClient(Client** head) {
    if (*head == NULL) {
        return;
    }

    Client* temp = *head;
    *head = (*head)->next;
    free(temp);
}

void printClientList(Client* head) {
    if (head == NULL) {
        printf("[ ]\n");
        return;
    }

    printf("[");
    Client* current = head;
    while (current != NULL) {
        printf("%d", current->id);
        if (current->next != NULL) {
            printf(", ");
        }
        current = current->next;
    }
    printf("]\n");
}