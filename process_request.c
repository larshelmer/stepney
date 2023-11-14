/*

process_request.c - find relevant information in request from client

*/

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "header.h"

/*
Returns the path of the requested file if everything goes well. If the
request is a POST or HEAD, which currently isn't implemented, it returns
"\\501". This will make answer resource send back an Error 501. If the
request isn't recognizable it returns NULL. (Beware!)
*/

char* process_request(char* message, short int sock)
{
  extern const struct configuration config;
  char* tokens;
  char* path;

  printf(".PR\n");
  tokens = strtok(message, " ");    /* Get the first part of the request */

  if (strcmp(tokens, "GET") == 0)                        /* Is it a GET? */
  {
    printf("we have a GET!\n");
    tokens = strtok(NULL, " ");                     /* Get the next part */
    if (*tokens == '/')                     /* Does it start with a '/'? */
    {
      /* Allocate memory for the path */
      path = (char*)malloc(strlen(config.htmldocs) + strlen(++tokens) + 1);
      strcpy(path, config.htmldocs);  /* Get predefined part of the path */
      path = strcat(path, tokens);             /* Add this specific path */
      free(--tokens);             /* Free the memory. Is this necessary? */
    }
    else                                  /* It doesn't start with a '/' */
    {
      /* Allocate memory for the path */
      path = (char*)malloc(strlen(config.htmldocs) + strlen(tokens) + 1);
      strcpy(path, config.htmldocs);  /* Get predefined part of the path */
      path = strcat(path, tokens);             /* Add this specific path */
      free(tokens);                                       /* Free memory */
    }
    printf("Path: %s\n", path);
    return path;                                      /* Return the path */
  }
  /* Is it HEAD or POST? */
  else if ((strcmp(tokens, "HEAD") == 0) || (strcmp(tokens, "POST") == 0))
  {
    return "\\501";                   /* HEAD and POST are unimplemented */
  }
  else                         /* Is it something else? Something WRONG? */
  {
    printf(".PR-E\n");

    return NULL;                                     /* Then return NULL */
  }
}
