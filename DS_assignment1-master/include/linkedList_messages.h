#ifndef EXERCISE2_LINKEDLIST_H
#define EXERCISE2_LINKEDLIST_H

#include <bits/stdint-uintn.h>

typedef struct node_message {
    u_int32_t id;
    char sender_name[256];
    char message[256];
    struct node_message *next;

} node_message_t;

typedef struct messages_list {
    node_message_t *head;
    int size;
} messages_list_t;


/**
 * Initializes a new linked list.
 * @return
 */
messages_list_t *llm_init();

/**
 * Checks if the provided key exists in the list.
 * @param list Pointer to the list to search.
 * @param key 256 string of characters that will be the key to search.
 * @return -1 if the operation is unsuccessful, 0 if the item was not found, 1 if the item was found.
 */
int llm_exist(messages_list_t *list, u_int32_t id);

/**
 * Adds the given values to the list if they don't exist.
 * @param list Pointer to the list.
 * @param key 256 string of characters that will be the key.
 * @param value1 256 string of characters that represents value1.
 * @param value2 Int value2.
 * @param value3 Float value3.
 * @return -1 if the operation was unsuccessful, 1 if the values where added correctly.
 */
int llm_set_value(messages_list_t *list, u_int32_t id, char *sender_name, char *message);

/**
 * Modifies the values of the given key if the item exists in the list.
 * @param list Pointer to the list.
 * @param key 256 string of characters that will be the key.
 * @param value1 256 string of characters that represents value1.
 * @param value2 Int value2.
 * @param value3 Float value3.
 * @return -1 if the operation was unsuccessful, 1 if the values where modified correctly.
 */
int llm_modify_values(messages_list_t *list, u_int32_t id, char *sender_name, char *message);

/**
 * Get the values of a specific element in the linked list
 * @param list Pointer to the list.
 * @param key 256 string of characters that will be the key.
 * @param value1 (output) 256 string of characters that represents value1.
 * @param value2 (output) Int value2.
 * @param value3 (output) Float value3.
 * @return -1 if the operation was unsuccessful, 1 if the values where retrieved correctly.
 */
int llm_get_values(messages_list_t *list, u_int32_t* id, char *sender_name, char *message);

/**
 * Method to delete a specific item that contain the given key in the linked list
 * @param list Pointer to the list.
 * @param key 256 string of characters that will be the key.
 * @return -1 if the operation was unsuccessful, 1 if the values where deleted correctly.
 */
int llm_delete_key(messages_list_t *list, u_int32_t id);

/**
 * Method to get the number of items in the linked list
 * @param list Pointer to the list.
 * @return -1 if the operation was unsuccessful, >=0 number of items in list.
 */
int llm_num_items(messages_list_t *list);

/**
 * Method to delete all the items in the linked list
 * @param list Pointer to the list.
 * @return -1 if the operation was unsuccessful, 1 if the values where deleted correctly.
 */
int llm_delete_all(messages_list_t *list);

#endif //EXERCISE2_LINKEDLIST_H
