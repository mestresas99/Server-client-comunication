#ifndef EXERCISE2_SOCKETIO_H
#define EXERCISE2_SOCKETIO_H
/**
 * Generic function that will be used by the others send methods to send messages between client and server
 * @param socket identifier of the socket
 * @param buffer String where the message is stored
 * @param len length of the item sent
 * @return -1 if the operation is unsuccessful, 0 if successful
 */
int sendMessage(int socket, char *buffer, size_t len);
/**
 * Generic function that will be used to send strings
 * @param socket identifier of the socket
 * @param buffer String where the message is stored
 */

void sendString(int socket, char *buffer);

ssize_t readLine(int fd, void *buffer, size_t n);
void readSocket(int fd,void *buffer, size_t n);

#endif