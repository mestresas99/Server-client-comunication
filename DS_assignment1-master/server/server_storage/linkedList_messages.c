#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "linkedList_messages.h"


messages_list_t *llm_init() {
    messages_list_t *list = (messages_list_t *) malloc(sizeof(messages_list_t));
    if (list == NULL) {
        return NULL;
    }
    list->head = NULL;
    list->size = 0;

    fflush(stdout);
    return list;
}

int llm_exist(messages_list_t *list, u_int32_t id) {

    if (list == NULL || list->head == NULL) {
        return -1;
    }
    node_message_t *current = list->head;
    while (id != current->id) {
        if (current->next == NULL) {
            //If we get to this point the key we are looking for doesnt exixts so it returns -1
            return 0;
        }
        current = current->next;
    }
    return 1;
}

int
llm_set_value(messages_list_t *list, u_int32_t id, char *sender_name, char *message) { /*Add elements at the end of the list*/
    if (list->size == 0) {
        node_message_t *head = (node_message_t *) malloc(sizeof(node_message_t));
        if (head == NULL) {
            return -1;
        }
        //Scenario list is empty
        head->id=id;

        strcpy(head->sender_name, sender_name);
        strcpy(head->message, message);

        head->next = NULL;
        list->head = head;
        list->size = 1;
        return 0;
    }

    //we check if the key is already in the list
    if (llm_exist(list, id) == 1) {
        return -1;
    }
    node_message_t *current = list->head;
    while (current->next != NULL) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = (node_message_t *) malloc(sizeof(node_message_t));
    current->next->id=id;
    strcpy(current->next->sender_name, sender_name);
    strcpy(current->next->message, message);
    current->next->next = NULL;
    list->size++;
    return 0;
}


int llm_get_values(messages_list_t *list, u_int32_t* id, char *sender_name, char *message) {


    node_message_t *current = list->head;
    while (current->id != *id) {
        if (current->next == NULL) {
            return -1;
        }
        current = current->next;
    }
    *id = current->id;
    strcpy(sender_name, current->sender_name);
    strcpy(message, current->message);

    return 0;

}

int llm_modify_values(messages_list_t *list, u_int32_t id, char *sender_name, char *message) {

    node_message_t *current = list->head;
    while (current->id != id) {
        if (current->next == NULL) {
            return -1;
        }
        current = current->next;
    }
    //If we we to this point, we have found the key we were looking for we we modify the values with the new ones
    current->id = id;
    strcpy(current->sender_name, sender_name);
    strcpy(current->message, message);
    return 0;

}

int llm_delete_key(messages_list_t *list, uint32_t id) {
    //we also need the previous node as it is the one
    // that will be linked with the node after the one that will be deleted
    node_message_t *previous = NULL;
    node_message_t *current = list->head;
    while (current->id != id) {
        if (current->next == NULL) {
            return -1;
        }
        previous = current;
        current = current->next;
    }
    node_message_t *temp_node = current->next;
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

int llm_num_items(messages_list_t *list) {
    //we just return the list's size
    if (list != NULL) {
        return list->size;
    } else {
        return -1;
    }
}

int llm_delete_all(messages_list_t *list) {
    if (list == NULL) {
        return -1;
    }
    node_message_t *current = list->head;
    node_message_t *previous;
    while (current != NULL) {
        previous = current;
        current = current->next;
        free(previous);
    }
    return 0;
}