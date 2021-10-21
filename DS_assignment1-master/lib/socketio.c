#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "socketio.h"
#include <errno.h>
#include <string.h>

/**
 * Generic function that will be used by the others send methods to send messages between client and server
 * @param socket identifier of the socket
 * @param buffer String where the message is stored
 * @param len length of the item sent
 * @return -1 if the operation is unsuccessful, 0 if successful
 */
int sendMessage(int socket, char *buffer, size_t len) {
    size_t r;
    size_t l = len;

    do {
        r = write(socket, buffer, l);
        l = l - r;
        buffer = buffer + r;
    } while ((l > 0) && (r >= 0));

    if (r < 0)
        return (-1);   /* fail */
    else
        return (0);    /* full length has been sent */
}

void sendString(int socket, char *buffer){
    size_t length = strlen(buffer)+1; //Get string length with the null character included.
    int return_val = sendMessage(socket,buffer,length);
    if(return_val==-1){
        perror("Error trying to send string:");
        close(socket);
        exit(EXIT_FAILURE);
    }
}

void readSocket(int fd,void *buffer, size_t n){
    if(readLine(fd,buffer,n)==-1){  //If readline returns an error, readSocket ends

        perror("Error reading from socket");
        exit(-1);
    }

}

ssize_t readLine(int fd, void *buffer, size_t n)
{
    ssize_t numRead;  /* num of bytes fetched by last read() */
    ssize_t totRead;	  /* total bytes read so far */
    char *buf;
    char ch;


    if (n <= 0 || buffer == NULL) {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;
    totRead = 0;

    for (;;) {
        numRead = read(fd, &ch, 1);	/* read a byte */

        if (numRead == -1) {
            if (errno == EINTR)	/* interrupted -> restart read() */
                continue;
            else
                return -1;		/* some other error */
        } else if (numRead == 0) {	/* EOF */
            if (totRead == 0)	/* no byres read; return 0 */
                return 0;
            else
                break;
        } else {			/* numRead must be 1 if we get here*/
            if (ch == '\n')
                break;
            if (ch == '\0')
                break;
            if (totRead < n - 1) {		/* discard > (n-1) bytes */
                totRead++;
                *buf++ = ch;
            }
        }
    }

    *buf = '\0';
    return totRead;
}