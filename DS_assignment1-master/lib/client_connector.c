//
// Created by jc on 11/5/21.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <socketio.h>
#include "client_connector.h"


int establish_connection(uint32_t ip, char* connect_port) { //method to establish communication with the server
    //initialization of sockets
    int sd, connectResult;
    struct sockaddr_in server_addr;
    struct hostent *hp;
    // the socket is create
    sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sd < 0) {
        perror("Error in socket");
        exit(1);
    }

    int val = 1;
    int err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));
    if (err < 0) {
        perror("Error in option");
        exit(-1);
    }

    // Obtain Server address
    bzero((char *) &server_addr, sizeof(server_addr));

    server_addr.sin_addr.s_addr=ip;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(connect_port));

    //send connection to the server process, sending the connection numbrer result
    connectResult = connect(sd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (connectResult < 0) {
        perror("Error connecting:");
        close(sd);
        exit(EXIT_FAILURE);
    }
    return sd;
}

int send_message(uint32_t ip, char* connect_port, char* username, char* username_dest, uint32_t message_identifier, char* message, CLIENT *clnt){
    int sc_client = establish_connection(ip, connect_port);//establish connection server client destination
    sendString(sc_client,"SEND_MESSAGE\0");
    sendString(sc_client, username);
    char identifier[256];
    sprintf(identifier,"%d",message_identifier);
    sendString(sc_client,identifier);
    sendString(sc_client,message);
    close(sc_client);
    enum clnt_stat retval_8;
    int result_8;
    // Once the message is sent, remove the message from the server_storage
    retval_8 = delete_user_message_1(username_dest, message_identifier, &result_8, clnt);
    if (retval_8 != RPC_SUCCESS) {
        clnt_perror (clnt, "call delete user message while send message");
    }
    return result_8;

}

void send_ack(uint32_t ip, char* connect_port, uint32_t message_identifier){
    int sc_client = establish_connection(ip, connect_port);//establish connection server client destination
    sendString(sc_client,"SEND_MESS_ACK\0");
    char identifier[256];
    sprintf(identifier,"%d",message_identifier);
    sendString(sc_client,identifier);
    close(sc_client);
}