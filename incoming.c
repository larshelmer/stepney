/*

incoming.c - handles communication with clients

*/

extern struct configuration config;

#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include "header.h"

#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include <netdb.h>

void closesocket(short int sock)
{
  if (close(sock) < 0)                   /* Close socket */
    perror("Stepney: (incoming) close");
  printf("Stepney: (incoming) closed client socket\n");
}

void incoming(short int sock_acc, struct sockaddr_in* acc_sin)
{
  char* incoming_message;
  char* path;

  printf(".incoming\n");

  if ((incoming_message = socket_reader(sock_acc)) == NULL)  /* Read from socket */
  {
    closesocket(sock_acc);
    return;
  }

  if ((path = process_request(incoming_message, sock_acc)) == NULL)  /* Parse the request */
  {
    closesocket(sock_acc);
    return;
  }

  printf(".spawning answer\n");

  answer_resource(sock_acc, acc_sin, path);  /* Answer the request */

  if (path != NULL)           /* Has path been set? */
    free(path);               /* Free its memory */
  if (incoming_message != NULL)  /* Has incoming_message been set? */ 
    free(incoming_message);   /* Free its memory */
  closesocket(sock_acc);    /* Close socket */
  printf("slut\n");
}
