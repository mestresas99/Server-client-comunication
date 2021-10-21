#ifndef EXERCISE2_LINKEDLIST_CLIENTS_H
#define EXERCISE2_LINKEDLIST_CLIENTS_H

#include <bits/stdint-uintn.h>
#include "linkedList_messages.h"

typedef struct node_users {
    char username[256];
    uint32_t identifier; //Identifier of the last message received
    messages_list_t* pending_messages;// list of pending messages
    struct node_users *next;

} node_users_t;

typedef struct users_list {
    node_users_t *head;
    int size;
} users_list_t;


/**
 * Initializes a new linked list.
 * @return
 */
users_list_t *llu_init();

/**
 * Checks if the provided key exists in the list.
 * @param list Pointer to the list to search.
 * @param username 256 string of characters that will be the key to search.
 * @return -1 if the operation is unsuccessful, 0 if the item was not found, 1 if the item was found.
 */
int llu_exist(users_list_t *list, char *username);


/**
 * Adds and initializes a user to the list if it does not exist.
 * @param list pointer to the list
 * @param username
 * @return -1 if error and 0 if success.
 */
int llu_register(users_list_t *list, char *username);

/**
 * Modifies the last message identifier of the given user
 * @param list Pointer to the list
 * @param username Username of the user
 * @param message_identifier New id
 * @return 0 if success, -1 on error
 */
int llu_set_identifier(users_list_t *list, char *username, uint32_t message_identifier);

/**
 * Retrieves the last message identifier of a user.
 * @param list Pointer to the list.
 * @param username Username of the user
 * @param message_identifier message identifier
 * @return -1 if error, 0 on success
 */
int llu_get_identifier(users_list_t *list, char *username, uint32_t *message_identifier);

/**
 * Method to delete a specific item that contain the given key in the linked list
 * @param list Pointer to the list.
 * @param username char indication the name of the user.
 * @return -1 if the operation was unsuccessful, 1 if the values where deleted correctly.
 */
int llu_unregister(users_list_t *list, char *username);

/**
 * Method to get the number of items in the linked list
 * @param list Pointer to the list.
 * @return -1 if the operation was unsuccessful, >=0 number of items in list.
 */
int llu_num_items(users_list_t *list);

/**
 * Get a pointer to the message_queue of a user.
 * @param list Pointer to the user list.
 * @param username Username of an User
 * @return The pointer of the message queue, NULL on error.
 */
messages_list_t* llu_get_message_queue(users_list_t *list, char * username);


#endif //EXERCISE2_LINKEDLIST_CLIENTS_H
