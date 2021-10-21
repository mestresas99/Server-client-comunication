#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "linkedList_clients.h"
#include "socketio.h"
#include "client_connector.h"
#include "rpc_message_storage.h"

clients_list_t *list = NULL;

pthread_mutex_t mutex_client_info;
pthread_cond_t client_info_condition;
pthread_mutex_t mutex_list;
int client_info_copied = 0;

typedef struct client_info{
    int sc; //socket description
    struct sockaddr_in client_address;
} client_info_t;

CLIENT *clnt;

void handleConnection(client_info_t * client_info_handle) {//method to establish connection with the client process
    //lock when the socket is being copied to memory
    pthread_mutex_lock(&mutex_client_info);
    client_info_t client_info;
    memcpy(&client_info, client_info_handle, sizeof(client_info_t));
    client_info_copied = 1;
    pthread_cond_signal(&client_info_condition);
    pthread_mutex_unlock(&mutex_client_info);
    int sc = client_info.sc;

    char op_code[256];
    readSocket(sc,op_code,256);
    char username [256];
    char port [256];
    char username_dest[256];
    char message[256];

    // Parse operation code.
    if(strcmp(op_code,"REGISTER") == 0){
        char result[5];
        // Retrieve username from socket.
        readSocket(sc,username,256);

        //Send register call to the storage server using rpc.
        enum clnt_stat retval_1;
        int register_result;
        retval_1 = rpc_register_1(username, &register_result, clnt);
        if (retval_1 != RPC_SUCCESS) {
            clnt_perror (clnt, "call failed");
        }
        // Store result as a string and send it.
        sprintf(result, "%d", register_result);
        sendString(sc,result);

        printf("REGISTER <%s> %s\ns>",username, strcmp("0\0",result) ==0 ? "OK":"FAIL");

    }else if(strcmp("UNREGISTER\0",op_code) == 0){
        // Retrieve username
        readSocket(sc,username,256);
        char result[5];
        enum clnt_stat retval_2;
        int unregister_result;

        //Send unregister call to the storage server using rpc.
        retval_2 = rpc_unregister_1(username, &unregister_result, clnt);
        if (retval_2 != RPC_SUCCESS) {
            clnt_perror (clnt, "call failed");
        }

        // Store result as a string and send it.
        sprintf(result, "%d", unregister_result);
        sendString(sc,result);

        printf("UNREGISTER <%s> %s\ns>",username, strcmp("0\0",result) ==0 ? "OK":"FAIL");

    }else if(strcmp("CONNECT\0",op_code) == 0){
        // Retrieve username and port
        readSocket(sc,username,256);
        readSocket(sc,port,256);
        char* result="0\0";

        // initialize the list if it hasn't been initialized.
        pthread_mutex_lock(&mutex_list);
        if(list==NULL){
            list = llc_init();
        }
        pthread_mutex_unlock(&mutex_list);

        enum clnt_stat retval_3;
        int exists_result;
        // Check that the user is registered in the storage server.
        retval_3 = rpc_user_exists_1(username, &exists_result, clnt);
        if (retval_3 != RPC_SUCCESS) {
            clnt_perror (clnt, "call USER EXIST connect failed.");
        }

        pthread_mutex_lock(&mutex_list);
        if(1!=llc_exist(list,username)){ // If the user is not connected and is registered, add the user to the connected list.
            if(exists_result==1){
                if(-1==llc_set_value(list,username,client_info.client_address.sin_addr.s_addr, port))
                    result="3\0"; // Error while setting value
            }else{//not exist
                result="1\0";
            }
        }else{
            result="2\0"; // User already exists
        }
        pthread_mutex_unlock(&mutex_list);
        //Send result to client.
        sendString(sc,result);

        printf("CONNECT <%s> %s\ns>",username, strcmp("0\0",result) ==0 ? "OK":"FAIL");

        // If connection was successful, check for pending messages and send them.
        if(strcmp("0\0",result)==0){
            enum clnt_stat retval_4;
            enum clnt_stat retval_5;
            int last_identifier;
            int num_messages;
            //Get the identifier of the last message received
            retval_4 = rpc_retrieve_last_identifier_1(username, &last_identifier, clnt);
            if (retval_4 != RPC_SUCCESS) {
                clnt_perror (clnt, "call rpc_retrieve_last_identifier_1");
            }
            //Get the number of pending messages.
            retval_5 = rpc_retrieve_message_number_1(username, &num_messages, clnt);
            if (retval_5 != RPC_SUCCESS) {
                clnt_perror (clnt, "call failed rpc_retrieve_message_number_1 while connect");
            }

            //If the user has received messages while he was disconnected, messages will be sent respecting the chronological order
            for(int i = (last_identifier-num_messages)+1; i<=last_identifier; i++){
                enum clnt_stat retval_6;
                user_message message_info;
                //Get the message content
                retval_6 = rpc_retrieve_user_message_1(username, i, &message_info, clnt);
                if (retval_6 != RPC_SUCCESS) {
                    clnt_perror (clnt, "call rpc_retrieve_user_message_1 while connect");
                }
                if (0 == message_info.result) {
                    //Send the message from the server to the client
                    send_message(client_info.client_address.sin_addr.s_addr,port,message_info.sender,username,i,message_info.message,clnt);
                    uint32_t ip_sender;
                    char port_sender[256];
                    pthread_mutex_lock(&mutex_list);
                    // Retrieve ip and port of the message sender.
                    if(0==llc_get_values(list,message_info.sender,&ip_sender,port_sender)){
                        //Send the acknowledgement to the sender
                        send_ack(ip_sender,port_sender,i);
                    }
                    pthread_mutex_unlock(&mutex_list);
                }
            }
        }

    }else if(strcmp("DISCONNECT\0",op_code) == 0){
        readSocket(sc,username,256); // Retrieve username
        char* result="0\0";
        enum clnt_stat retval_3;
        int user_exist;
        //Check if user exist
        retval_3 = rpc_user_exists_1(username, &user_exist, clnt);
        if (retval_3 != RPC_SUCCESS) {
            clnt_perror (clnt, "call failed");
        }
        if(user_exist){ //If the user exists, check status
            uint32_t ip;
            char connect_port[256];
            pthread_mutex_lock(&mutex_list);
            if(0== llc_get_values(list,username, &ip, connect_port)){//take the IP from user
                if (llc_exist(list,username) == 1) { //if the user is connected and has the same IP, change to disconnected
                    if (client_info.client_address.sin_addr.s_addr == ip){
                        if (0!= llc_delete_user(list,username)){//if the user is connect, delete from clients list
                            result = "3\0";
                        }
                    }else { //ips are not equal
                        result = "3\0";
                    }
                }else{ //the user is disconnected
                    result="2\0";
                }
            }
            pthread_mutex_unlock(&mutex_list);
        }else{
            result="1\0"; // User does not exists
        }
        sendString(sc,result);
        printf("DISCONNECT <%s> %s\ns>",username, strcmp("0\0",result) ==0 ? "OK":"FAIL");
    }else if(strcmp("SEND\0",op_code) == 0){
        readSocket(sc,username,256); // Retrieve username.
        readSocket(sc,username_dest,256); // Retrieve destination username.
        readSocket(sc,message,256); // Retrieve message string.
        char* result="0\0";
        char identifier[256];
        enum clnt_stat retval_3;
        int user_exist;

        //Check that the sender user is registered.
        retval_3 = rpc_user_exists_1(username, &user_exist, clnt);
        if (retval_3 != RPC_SUCCESS) {
            clnt_perror (clnt, "call pc_user_exists while send");
        }

        //Check if the destination user is registered
        int user_destination_exist;
        retval_3 = rpc_user_exists_1(username_dest, &user_destination_exist, clnt);
        if (retval_3 != RPC_SUCCESS) {
            clnt_perror (clnt, "pc_user_exists 2 while send");
        }

        if(1==user_exist && 1 == user_destination_exist) { //If both users exist, send message.
            enum clnt_stat retval_7;
            int message_identifier;
            //Store message in the server_store and retrieve the identifier assign from the server in message_identifier.
            retval_7 = store_user_message_1(username_dest, username, message, &message_identifier, clnt);
            if (retval_7 != RPC_SUCCESS) {
                clnt_perror (clnt, "store_user_message_1 while send");
            }
            if(message_identifier!=-1){
                //send the result and if successful send the message_identifier.
                sprintf(identifier,"%d",message_identifier);
                sendString(sc,result);
                if(strcmp("0\0",result) ==0){
                    sendString(sc,identifier);
                }
                pthread_mutex_lock(&mutex_list);
                if(llc_exist(list,username_dest)) { // If the destination user is connected, send the message.
                    uint32_t ip;
                    char connect_port[256];
                    if (0 == llc_get_values(list, username_dest, &ip, connect_port)) {//take the IP from user
                        send_message(ip,connect_port,username,username_dest,message_identifier,message,clnt); //Send message to the client.
                        uint32_t ip_sender;
                        char port_sender[256];
                        if(0==llc_get_values(list,username,&ip_sender,port_sender)){//get ip_sender and the port_sender to send ack
                            send_ack(ip_sender,port_sender,message_identifier);
                            printf("SEND MESSAGE <%s> FROM <%s> TO <%s>\ns>",identifier,username, username_dest);
                            fflush(stdout);
                        }else{//get values not correct
                            result="2\0";
                            sendString(sc,result);
                        }
                    }else{//get values not correct
                        result="2\0";
                        sendString(sc,result);
                    }
                }else{ //destination user is disconnect
                    printf("SEND MESSAGE <%d> FROM <%s> TO <%s> STORED\ns>",message_identifier,username, username_dest);
                    fflush(stdout);
                }
                pthread_mutex_unlock(&mutex_list);
            }else{
                result="2\0"; // Error while storing message
                sendString(sc,result);
            }
        }else{ //the user does not exist, send error
            result="1\0"; // User does not exists
            sendString(sc,result);
        }

    }

    close(sc);
    pthread_exit(0);
}


int main(int argc, char *argv[]){
    if(argc!=5) {
        printf("Usage: server -p <port> -s <hostRPC>\n");
        fflush(stdout);
        exit(0);
    }
    struct sockaddr_in server_addr, client_addr;
    int sd, sc;
    int val;
    uint32_t size;
    int err;

    //socket creation
    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
        perror("Error creating the socket");
        exit(-1);
    }

    val = 1;
    err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));
    if (err < 0) {
        perror("Error in option");
        exit(-1);
    }

    // Retrieve port from arguments.
    char * port_ch = argv[2];
    in_port_t port = htons(atoi(port_ch));

    // Create server_addr
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = port;

    if (bind(sd, (const struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("A error occurred while binding");
        exit(-1);
    }

    printf("s> init server %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(port));
    fflush(stdout);

    listen(sd, 5);
    //get the size of the client buffer
    size = sizeof(client_addr);

    //Create thread and its attributes
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_mutex_init(&mutex_client_info, NULL);
    pthread_mutex_init(&mutex_list, NULL);
    pthread_cond_init(&client_info_condition, NULL);

    // Create rpc client that connects to the given ip
    clnt = clnt_create (argv[4], MESSAGE_STORAGE, MESSAGE_STORAGEVER, "tcp");
    if (clnt == NULL) {
        clnt_pcreateerror (argv[4]);
        exit (1);
    }


    while (1) {//infinite loop waiting for clients

        //Wait for connection and accept incomming connection.
        sc = accept(sd, (struct sockaddr *) &client_addr, &size);//socket initialization
        if (sc < 0) {
            perror("Accept failed\n");//failed socket creation
            exit(-1);
        }

        client_info_t  client_info_main;
        client_info_main.sc=sc;
        client_info_main.client_address = client_addr;

        pthread_t connectionThread;
        //go directly to the handleConnection function
        pthread_create(&connectionThread, &attr, (void *) handleConnection, &client_info_main);
        pthread_mutex_lock(&mutex_client_info);
        while (client_info_copied == 0)
            pthread_cond_wait(&client_info_condition, &mutex_client_info);
        client_info_copied = 0;
        pthread_mutex_unlock(&mutex_client_info);
    }
}

