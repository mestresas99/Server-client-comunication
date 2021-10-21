// Created by jc on 11/5/21.
//

#ifndef CLIENT_JAVA_CLIENT_CONNECTOR_H
#define CLIENT_JAVA_CLIENT_CONNECTOR_H
#include "rpc_message_storage.h"

int establish_connection(uint32_t ip, char* connect_port);
int send_message(uint32_t ip, char* connect_port, char* username, char* username_dest, uint32_t message_identifier, char* message, CLIENT* client);
void send_ack(uint32_t ip, char* connect_port, uint32_t message_identifier);//


#endif //CLIENT_JAVA_CLIENT_CONNECTOR_H
