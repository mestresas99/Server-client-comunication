#ifndef EXERCISE2_LINKEDLIST_CLIENTS_H
#define EXERCISE2_LINKEDLIST_CLIENTS_H

#include <bits/stdint-uintn.h>
#include "linkedList_messages.h"

typedef struct node_client {
    char username[256];
    uint32_t ip;
    char port[256];
    struct node_client *next;

} node_client_t;

typedef struct clients_list {
    node_client_t *head;
    int size;
} clients_list_t;


/**
 * Initializes a new linked list.
 * @return
 */
clients_list_t *llc_init();

/**
 * Checks if the provided key exists in the list.
 * @param list Pointer to the list to search.
 * @param username 256 string of characters that will be the key to search.
 * @return -1 if the operation is unsuccessful, 0 if the item was not found, 1 if the item was found.
 */
int llc_exist(clients_list_t *list, char *username);

/**
 * Adds the given values to the list if they don't exist.
 * @param list Pointer to the list.
 * @param username 256 string of characters that will be the key.
 * @param status 256 string of characters that represents value1.
 * @param ip Int value2.
 * @param port Float value3.
 * @return -1 if the operation was unsuccessful, 1 if the values where added correctly.
 */
int llc_set_value(clients_list_t *list, char *username, uint32_t ip, char *port);

/**
 * Modifies the values of the given key if the item exists in the list.
 * @param list Pointer to the list.
 * @param key 256 string of characters that will be the key.
 * @param value1 256 string of characters that represents value1.
 * @param value2 Int value2.
 * @param value3 Float value3.
 * @return -1 if the operation was unsuccessful, 0 if the values where modified correctly.
 */
int llc_modify_values(clients_list_t *list, char *username, uint32_t ip, char *port);

/**
 * Get the values of a specific element in the linked list
 * @param list Pointer to the list.
 * @param key 256 string of characters that will be the key.
 * @param value1 (output) 256 string of characters that represents value1.
 * @param value2 (output) Int value2.
 * @param value3 (output) Float value3.
 * @return -1 if the operation was unsuccessful, 1 if the values where retrieved correctly.
 */
int llc_get_values(clients_list_t *list, char *username, uint32_t *ip, char *port);

/**
 * Method to delete a specific item that contain the given key in the linked list
 * @param list Pointer to the list.
 * @param key 256 string of characters that will be the key.
 * @return -1 if the operation was unsuccessful, 1 if the values where deleted correctly.
 */
int llc_delete_user(clients_list_t *list, char *username);


#endif //EXERCISE2_LINKEDLIST_CLIENTS_H
