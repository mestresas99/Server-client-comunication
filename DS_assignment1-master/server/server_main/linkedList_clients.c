#include <string.h>
#include <stdlib.h>
#include "linkedList_clients.h"
#include <stdio.h>


clients_list_t *llc_init() {
    clients_list_t *list = (clients_list_t *) malloc(sizeof(clients_list_t));
    if (list == NULL) {
        return NULL;
    }
    list->head = NULL;
    list->size = 0;

    return list;
}

int llc_exist(clients_list_t *list, char *username) {

    if (list == NULL || list->head == NULL) {
        return -1;
    }
    node_client_t *current = list->head;
    while (strcmp(current->username, username) != 0) {
        if (current->next == NULL) {
            //If we get to this point the key we are looking for doesnt exists so it returns -1
            return 0;
        }
        current = current->next;
    }
    return 1;
}

int llc_set_value(clients_list_t *list, char *username, uint32_t ip, char *port) { /*Add elements at the end of the list*/
    if (list->size == 0) {
        node_client_t *head = (node_client_t *) malloc(sizeof(node_client_t));
        if (head == NULL) {
            return -1;
        }
        //Scenario list is empty
        strcpy(head->username, username);
        head->ip = ip;
        strcpy(head->port, port);
        head->next = NULL;
        list->head = head;
        list->size = 1;
        return 0;
    }

    //we check if the key is already in the list
    if (llc_exist(list, username) == 1) {
        return -1;
    }
    node_client_t *current = list->head;
    while (current->next != NULL) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = (node_client_t *) malloc(sizeof(node_client_t));

    strcpy(current->next->username, username);
    current->next->ip = ip;
    strcpy(current->next->port, port);
    current->next->next = NULL;
    list->size++;
    return 0;
}


int llc_get_values(clients_list_t *list, char *username, uint32_t *ip, char *port) {


    node_client_t *current = list->head;
    while (strcmp(current->username, username) != 0) {
        if (current->next == NULL) {
            return -1;
        }
        current = current->next;
    }
    strcpy(username, current->username);
    *ip = current->ip;
    strcpy(port, current->port);
    return 0;

}

int llc_modify_values(clients_list_t *list, char *username, uint32_t ip, char *port) {

    node_client_t *current = list->head;
    while (strcmp(current->username, username) != 0) {
        if (current->next == NULL) {
            return -1;
        }
        current = current->next;
    }

    current->ip = ip;
    strcpy(current->port, port);
    return 0;

}

int llc_delete_user(clients_list_t *list, char *username) {
    //we also need the previous node as it is the one
    // that will be linked with the node after the one that will be deleted
    node_client_t *previous = NULL;
    node_client_t *current = list->head;
    while (strcmp(current->username, username) != 0) {
        if (current->next == NULL) {
            return -1;
        }

        previous = current;
        current = current->next;
    }

    node_client_t *temp_node = current->next;
    if (previous == NULL) {
        //If the previous is null, the node that will be deleted will be the header
        //so the node after the header will be the new header
        list->head = temp_node;
    } else {
        if (current->next == NULL) {
            //Scenario node removed is the last one
            previous->next = NULL;
        } else {
            //Else the rest of the cases
            previous->next = temp_node;
        }
    }
    free(current);
    list->size--;
    return 0;

}
