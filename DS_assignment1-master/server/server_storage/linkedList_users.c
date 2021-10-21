#include <string.h>
#include <stdlib.h>
#include "linkedList_users.h"
#include "linkedList_messages.h"
#include <stdio.h>

users_list_t *llu_init() {
    //We allocate the momery required
    users_list_t *list = (users_list_t *) malloc(sizeof(users_list_t));
    if (list == NULL) {
        return NULL;
    }
    //set the head as null and size 0 because is empty
    list->head = NULL;
    list->size = 0;

    return list;
}

int llu_exist(users_list_t *list, char *username){
    //First we check if there are elements in the list
    if (list == NULL || list->head == NULL) {
        return -1;
    }

    node_users_t *current = list->head;
    //Then we iterate through the list until we find the username or we get to the end
    while (strcmp(current->username, username) != 0) {
        if (current->next == NULL) {
            //If we get to this point the key we are looking for doesnt exists so it returns -1
            return 0;
        }
        current = current->next;
    }
    //If we actually get to this point, the username is not in the list
    return 1;
}


int llu_register(users_list_t *list, char *username){ /*Add new users at the end of the list*/
    if (list->size == 0) {
        node_users_t *head = (node_users_t *) malloc(sizeof(node_users_t));
        if (head == NULL) {
            return -1;
        }
        //Scenario list is empty
        strcpy(head->username, username);
        head->identifier=0;
        messages_list_t* message_queue = llm_init();
        head->pending_messages=message_queue;
        head->next = NULL;
        list->head = head;
        list->size = 1;
        return 0;
    }

    //we check if the username is already in the list
    if (llu_exist(list, username) == 1) {
        return -1;
    }
    node_users_t *current = list->head;
    while (current->next != NULL) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = (node_users_t *) malloc(sizeof(node_users_t));

    strcpy(current->next->username, username);

    current->next->identifier=0;
    current->next->pending_messages=llm_init();
    current->next->next = NULL;
    list->size++;
    return 0;
}


int llu_get_identifier(users_list_t *list, char *username, uint32_t *message_identifier){
    //method that gets the identifier of a username

    node_users_t *current = list->head;
    while (strcmp(current->username, username) != 0) {
        if (current->next == NULL) {
            return -1;
        }
        current = current->next;
    }


    *message_identifier = current->identifier;

    return 0;

}




int llu_set_identifier(users_list_t *list, char *username, uint32_t message_identifier){
    //Function that modify the user's identifier
    node_users_t *current = list->head;
    while (strcmp(current->username, username) != 0) {
        if (current->next == NULL) {
            return -1;
        }
        current = current->next;
    }
    current->identifier=message_identifier;
    return 0;

}


int llu_unregister(users_list_t *list, char *username) {
    //This method deletes a user from a linked list
    //we also need the previous node as it is the one
    // that will be linked with the node after the one that will be deleted
    node_users_t *previous = NULL;
    node_users_t *current = list->head;
    while (strcmp(current->username, username) != 0) {
        if (current->next == NULL) {
            return -1;
        }

        previous = current;
        current = current->next;
    }

    node_users_t *temp_node = current->next;
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




messages_list_t * llu_get_message_queue(users_list_t *list, char * username){
    //Function that returns a user's message queue
    node_users_t *current = list->head;
    while (strcmp(current->username, username) != 0) {
        if (current->next == NULL) {
            return NULL;
        }
        current = current->next;
    }

    return current->pending_messages;
}


