/*

 Header.h - function declarations

*/

struct errorfiles
{
  char* error_400;  /* Bad request */
  char* error_401;  /* Unauthorized */
  char* error_403;  /* Forbidden */
  char* error_404;  /* Not found */
  char* error_500;  /* Internal server error */
  char* error_501;  /* Not implemented */
  char* error_502;  /* Bad gateway */
  char* error_503;  /* Service unavailable */
};

struct logfiles
{
  char* accesslog;  /* Should be complete filename */
  char* errorlog;   /* Should be complete filename */
};

struct configuration
{
  unsigned short int port;
  char* htmldocs;     /* Must end with '/' */
  char* cgibin;       /* Must end with '/', relative to htmldocs without first '/' */
  struct logfiles logs;
  struct errorfiles errors;
  char** defaulthtml;
  char* name;
  char* version;

  /* more to come... */

};
/*
struct cinfo
{
  short int sock;
  struct sockaddr_in sin;
};
*/

/*
short int configure();

short int open_serversocket(struct sockaddr_in* sin);

void incoming(short int acc_sock, struct sockaddr_in* acc_sin);
*/

char* socket_reader(short int sock);
char* process_request(char* message, short int sock);
int popen_script(char* path, char* output);
char* get_type(const char* pathname);
/*
void incoming(short int sock_acc, struct sockaddr_in* acc_sin);
*/
