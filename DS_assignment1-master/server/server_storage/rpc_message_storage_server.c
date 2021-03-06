/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "rpc_message_storage.h"
#include "linkedList_users.h"
#include "linkedList_messages.h"

users_list_t *list = NULL;
pthread_mutex_t mutex_list;
int mutex_initialized=0;

bool_t rpc_register_1_svc(char *username, int *result,  struct svc_req *rqstp) //method to register an user into the rpc server
{
    if(mutex_initialized==0){
        pthread_mutex_init(&mutex_list, NULL);
        mutex_initialized=1;
    }

    bool_t retval;
    *result=0;
    pthread_mutex_lock(&mutex_list);
    if(list==NULL) {
        list = llu_init();
    }
    if(1!=llu_exist(list,username)){
        if(-1==llu_register(list,username))
            *result=2; // Error while setting value
    }else{
        *result=1; // User already exists
    }
    pthread_mutex_unlock(&mutex_list);

    retval = TRUE;
    return retval;
}

bool_t
rpc_unregister_1_svc(char *username, int *result,  struct svc_req *rqstp) //method to unregister an user into the rpc server
{
    if(mutex_initialized==0){
        pthread_mutex_init(&mutex_list, NULL);
        mutex_initialized=1;
    }
    bool_t retval;
    *result=0;
    pthread_mutex_lock(&mutex_list);
    if(1==llu_exist(list,username)){ //If the user exists, try to delete the user and its messages.
        messages_list_t *message_queue = llu_get_message_queue(list,username);
        if(NULL== message_queue){
            *result=2;
        }
        if(-1==llm_delete_all(message_queue)){
            *result=2;
        }
        if(-1== llu_unregister(list,username)){
            *result=2;
        }
    }else{
        *result=1; // User does not exists
    }
    pthread_mutex_unlock(&mutex_list);
    retval = TRUE;
    return retval;
}

bool_t
rpc_user_exists_1_svc(char *username, int *result,  struct svc_req *rqstp) //method to check that an user exists in the rpc server
{
    if(mutex_initialized==0){
        pthread_mutex_init(&mutex_list, NULL);
        mutex_initialized=1;
    }
    bool_t retval;
    pthread_mutex_lock(&mutex_list);
    *result=llu_exist(list, username);
    pthread_mutex_unlock(&mutex_list);
    retval = TRUE;
    return retval;
}

bool_t
rpc_retrieve_last_identifier_1_svc(char *username, int *result,  struct svc_req *rqstp) //get the id from the last user
{
    if(mutex_initialized==0){
        pthread_mutex_init(&mutex_list, NULL);
        mutex_initialized=1;
    }
    bool_t retval;
    uint32_t message_identifier;
    *result=0;
    pthread_mutex_lock(&mutex_list);
    if(-1==llu_get_identifier(list,username,&message_identifier)){ //get the id method of the las user in the list
        *result=-1;
    }else{
        *result= message_identifier;
    }
    pthread_mutex_unlock(&mutex_list);
    retval = TRUE;
    return retval;
}

bool_t
rpc_retrieve_message_number_1_svc(char *username, int *result,  struct svc_req *rqstp)//return the number of messages of an user
{
    if(mutex_initialized==0){
        pthread_mutex_init(&mutex_list, NULL);
        mutex_initialized=1;
    }
    bool_t retval;
    *result=0;
    pthread_mutex_lock(&mutex_list);
    messages_list_t* message_queue = llu_get_message_queue(list,username);
    if(NULL != message_queue) { //get the message queue from an user
        *result=llm_num_items(message_queue); //return the num of messages from a queue
    }else{
        *result=-1;
    }
    pthread_mutex_unlock(&mutex_list);
    retval = TRUE;
    return retval;
}

bool_t
rpc_retrieve_user_message_1_svc(char *username, int message_identifier, user_message *result,  struct svc_req *rqstp)// get an specific message with the user id
{
    if(mutex_initialized==0){
        pthread_mutex_init(&mutex_list, NULL);
        mutex_initialized=1;
    }
    bool_t retval;
    pthread_mutex_lock(&mutex_list);
    messages_list_t* message_queue = llu_get_message_queue(list,username);
    char message[256];
    char sender[256];
    if(NULL != message_queue) { //get the message queue from an user
        if(-1!=llm_get_values(message_queue, &message_identifier, sender, message)){ //get the message with an specific id
            strcpy(result->message, message);
            strcpy(result->sender, sender);


            result->result=0;
        }else{
            result->result=-1;
        }

    }else{
        result->result=-1;
    }
    pthread_mutex_unlock(&mutex_list);
    retval = TRUE;
    return retval;
}

bool_t
store_user_message_1_svc(char *username, char *username_sender, char *message, int *result,  struct svc_req *rqstp)
{
    if(mutex_initialized==0){
        pthread_mutex_init(&mutex_list, NULL);
        mutex_initialized=1;
    }
    bool_t retval;
    pthread_mutex_lock(&mutex_list);
    messages_list_t* message_queue = llu_get_message_queue(list,username);
    uint32_t message_identifier;
    if(NULL != message_queue) { //get the message queue from an user

        if(-1!=llu_get_identifier(list,username,&message_identifier)){
            message_identifier++;
            if(-1!=llm_set_value(message_queue,message_identifier,username_sender,message)){
                if(-1!=llu_set_identifier(list,username,message_identifier)){
                    *result=message_identifier;
                }else
                    *result=-1;

            }else
                *result=-1;
        }else
            *result=-1;
    }else
        *result=-1;
    pthread_mutex_unlock(&mutex_list);
    retval = TRUE;
    return retval;
}

bool_t
delete_user_message_1_svc(char *username, int message_identifier, int *result,  struct svc_req *rqstp)
{
    if(mutex_initialized==0){
        pthread_mutex_init(&mutex_list, NULL);
        mutex_initialized=1;
    }
    pthread_mutex_lock(&mutex_list);
    bool_t retval;
    messages_list_t* message_queue = llu_get_message_queue(list,username);
    if(NULL != message_queue) { //get the message queue from an user
        *result=llm_delete_key(message_queue, message_identifier);
    }else{
        *result=-1;
    }
    pthread_mutex_unlock(&mutex_list);
    retval = TRUE;
    return retval;
}

int
message_storage_1_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
    xdr_free (xdr_result, result);

    /*
     * Insert additional freeing code here, if needed
     */

    return 1;
}
