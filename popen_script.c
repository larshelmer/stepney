/*

popen_script.c - execute a script and return its output

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* The output parameter must be allocated in advance! */

int popen_script(char* path, char* output)                /* Must get complete path */
{
  FILE* fp;

  if (access(path, F_OK) < 0)                                /* Test if file exists */
  {
    perror("Stepney: (popen_script) access - exists");
    return 404;                                       /* Return error code - exists */
  }
  if (access(path, X_OK) < 0)                        /* Test for execute permission */
  {
    perror("Stepney: (popen_script) access - executable");
    return 403;                                   /* Return error code - permission */
  }
  if ((fp = popen(path, "r")) == NULL)  /* Open a pipe to script output and execute */
  {
    perror("Stepney: (popen_script) popen");
    return 500;                                 /* Return error code - server error */
  }
  bzero(output, 1024);       /* Clear memory, so it will guaranteed be 0-terminated */
  fscanf(fp, "%1023c", output);     /* Put output in a string. Max 1023 characters! */
  pclose(fp);                                                         /* Close pipe */

  return 200;                                                          /* Return ok */
}
