/*

socket_reader.c - reads request from socket

*/

#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "header.h"

/*
Returns the message if there is one, otherwise NULL.
*/

char* socket_reader(short int sock)
{
  int count;
  char buf[1024];
  char* message;

  if ((count = read(sock, &buf, sizeof(buf))) < 1)     /* Try to read from socket */
  {
    perror("Stepney: (socket_reader) read");
    return NULL;
  }
  if (count > 0)  /* hmm.... this could be very _wrong_! */
  {
    message = (char*)malloc(strlen(buf) + 1);  /* Allocate memory for the message */
    strcpy(message, (char*)&buf);              /* Copy the buffer into the memory */
  }
  return message;  /* Return the message */
}
