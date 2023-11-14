/*

test.c - configure function test

*/

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "header.h"

const struct configuration config;

int main()
{
  struct sockaddr_in acc_sin;
  short int sock, acc_sock;
  int acc_len;

  if (configure() < 0)
  {
    fprintf(stderr, "Stepney: (main) unable to configure\n");
    exit(-1);
  }

  if ((sock = open_serversocket()) < 0)
    exit(-1);

  while (1)
  {
    acc_sock = 0;
    bzero((char*)&acc_sin, sizeof(acc_sin));
    acc_len = sizeof(acc_sin);

    acc_sock = accept(sock, (struct sockaddr*)&acc_sin, &acc_len);
    incoming(acc_sock, &acc_sin); /* we should check for errors before this */
  }

    if (close(sock) < 0)
      perror("Stepney: (main) close");
}
