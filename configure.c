/*

configure.c - configure function

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
/* #include<stdlib.h> */
/* #include<string.h> */
#include"header.h"


short int configure()
{
  extern struct configuration config;
  char **p;

  /*  struct configuration config; */
  config.port = 42013;
  config.htmldocs = "www-root/";  /* must end with '/' */
  config.cgibin = "cgi-bin";    /* relative to htdocs. must not begin or end with '/' */
  config.name = "Stepney";
  config.version = "0.9.0";

  config.logs.accesslog = "accesslog.log";
  config.logs.errorlog = "errorlog.log";

  config.errors.error_403 = "errors/error_403.html";
  config.errors.error_404 = "errors/error_404.html";
  config.errors.error_500 = "errors/error_500.html";
  config.errors.error_501 = "errors/error_501.html";

  config.defaulthtml = malloc(5 * sizeof(char*));
  config.defaulthtml[0] = "index.html";
  config.defaulthtml[1] = "index.htm";
  config.defaulthtml[2] = "welcome.html";
  config.defaulthtml[3] = "welcome.htm";
  config.defaulthtml[4] = NULL;

  return 0;
}
