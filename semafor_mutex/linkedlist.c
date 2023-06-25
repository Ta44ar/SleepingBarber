#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "linkedlist.h"

Client* createClient(int id) {
    Client* newClient = (Client*)malloc(sizeof(Client));
    newClient->id = id;
    newClient->next = NULL;
    return newClient;
}

Client* addClient(Client** head, int id) {
    Client* newClient = createClient(id);

    if (sem_init(&newClient->turn, 0, 0)){
        perror("sem_init turn error");
        exit(EXIT_FAILURE);
    }
    if (sem_init(&newClient->wasCut, 0, 0)){
        perror("sem_init wasCut error");
        exit(EXIT_FAILURE);
    }
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
    copiedClient->turn = head->turn;
    copiedClient->wasCut = head->wasCut;
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