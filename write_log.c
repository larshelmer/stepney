/*

write_log.c - writes access and error logs

*/

/*

needs status (eg 200, 404 etc), address and file from the function calling it
address might demand some slight reorganizing.
write in file:
<status> <address>        <date>         <file>
xxx      xxx.xxx.xxx.xxx  ctime(time())  filename, variable length

The address field could have different length as well which will be compensated for
to get even columns. Access log will contain _every_ attempt while error log only
will contain those who fails (eg file not found, unimplemented etc).

*/

#include <time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "header.h"

void write_log(int status, struct sockaddr_in* sin, char* filename)
{
  extern const struct configuration config;

  FILE* fp;
  char* timestring;
  char* ipstring;
  time_t t;
  int i;

  printf("in writelog\n");

  t = time(NULL);                                  /* Get time in seconds since 1970 */
  timestring = ctime(&t);           /* Convert to a readable format (\n-terminated!) */
  *(timestring + strlen(timestring) - 1) = '\0';            /* Remove \n att the end */

  ipstring =  inet_ntoa(sin->sin_addr);                   /* Get ip in a nice format */

  if ((fp = fopen(config.logs.accesslog, "a")) != NULL)              /* Open logfile */
  {
    fprintf(fp, "%d   %s", status, ipstring);         /* Write status and ip to file */
    for (i = 0; i < (18 - strlen(ipstring)); i++)    /* Some spaces for even columns */
      fprintf(fp, " ");
    fprintf(fp, "%s   %s\n", timestring, filename);       /* Write time and filename */
    if (fflush(fp) < 0)                                          /* Flush the stream */
      perror("Stepney: (write_log) fflush - access");
    if (fclose(fp) < 0)                                              /* and close it */
      perror("Stepney: (write_log) fclose - access");
  }
  else
    perror("Stepney: (write_log) fopen - access");
  if (status >= 400)                                       /* Is it an error status? */
  {
    if ((fp = fopen(config.logs.errorlog, "a")) != NULL)   /* Open the error logfile */
    {
      fprintf(fp, "%d   %s", status, ipstring);       /* Write status and ip to file */
      for (i = 0; i < (18 - strlen(ipstring)); i++)  /* Some spaces for even columns */
	fprintf(fp, " ");
      fprintf(fp, "%s   %s\n", timestring, filename);     /* Write time and filename */
      if (fflush(fp) < 0)                                        /* Flush the stream */
	perror("Stepney: (write_log) fflush - error");
      if (fclose(fp) < 0)                                            /* and close it */
	perror("Stepney: (write_log) fclose - error");
    }
    else
      perror("Stepney: (write_log) fopen - error");
  }
}
