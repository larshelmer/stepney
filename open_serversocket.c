/*

open_serversocket.c - setting up a serversocket

*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include "header.h"


#define MAX_PENDING 5


short int open_serversocket()
{
  extern const struct configuration config;

  struct sockaddr_in sin;
  short int sock;
  int enable = 1;

  bzero((char*)&sin, sizeof(sin));    /* Clear memory where sin resides */
  sin.sin_family = AF_INET;           /* Internet protocol family */
  sin.sin_addr.s_addr = INADDR_ANY;   /* Local computer */
  sin.sin_port = htons(config.port);  /* Set port number - network order */

  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) /* Create socket descriptor */
  {
    perror("Stepney: (open_serversocket) socket");
    return -1;
  }

  /* Make socket instantly reusable */
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
    perror("Stepney: (open_serversocket) setsockopt");

  if ((bind(sock, (struct sockaddr*)&sin, sizeof(sin))) < 0) /* Assign name to socket */
  {
    perror("Stepney: (open_serversocket) bind");
    return -1;
  }

  listen(sock, MAX_PENDING);  /* Listen for incoming traffic */


  return sock;  /* Return the socket */
}
