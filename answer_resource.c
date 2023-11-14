#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>

#include "header.h"

#define DEBUG 1


char *buf = NULL;


/*  must get a filename (resource).
    tries to send the resource.
    if resource is a dir, tries to look for default file, eg. index.html.
    if resource is a script, runs it and sends output.
    also first sends appropriate headers.
    sends default errorfiles if above fails.
    does not close socket.
*/
int answer_resource(short int sock, struct sockaddr_in *caddr, char *pathname){

    int status_code;       /* 200, 404... */
    char *newpath = (char*) malloc(strlen(pathname) + 42);
    int r;

    strcpy(newpath, pathname);
    if (DEBUG)
        printf(".answer got: %s\n", pathname);

    if (strstr(pathname, "..") != NULL)
        status_code = 403;
    else if (access(pathname, F_OK) == 0)
        status_code = resolve_resource(pathname, newpath);
    else
        status_code = 404;

    if (DEBUG)
        printf(".resolve gave statuscode: %d %s\n", status_code, newpath);

    r = send_resource(sock, caddr, status_code, newpath);
    free(newpath);
    return r;
    }


/*  looks at the filename, and returns what it thinks is appropriate,
    based on suffix.
*/
char* get_type(const char* pathname){
    char* suffix;
    char* type;

    suffix = rindex(pathname, '.');
    if (suffix == '\0')
        type = "text/plain";
    else if (strcmp(suffix, ".html") == 0 || strcmp(suffix, ".htm") == 0)
        type = "text/html";
    else if (strcmp(suffix, ".png") == 0)
        type = "image/png";
    else if (strcmp(suffix, ".jpg") == 0 || strcmp(suffix, ".jpeg") == 0)
        type = "image/jpeg";
    else if (strcmp(suffix, ".gif") == 0)
        type = "image/gif";
    else
        type = "text/plain";    /* as good a default as any... */

    if (DEBUG)
        printf(".content-type: %s\n", type);
    return type;
    }


/*  if path is dir, tries to find a default file (unless its the scriptdir).
    pathname must be an existing file (in unix sense) though.
    newpath will contain the, possibly, updated filename.
    returns the status_code for the, possibly, updated file.
    checks for ".." in request.
*/
int resolve_resource(const char* pathname, char* newpath){

    const extern struct configuration config;
    char **def = config.defaulthtml;    /* point at current default html file */
    char *x;                            /* point at location where directory name ends (right after /) */
    struct stat fs;
    
    if (DEBUG)
        printf(".in resolve_resource with: %s\n", pathname);

    stat(pathname, &fs);

    /* is it a regular file? */
    if (S_ISREG(fs.st_mode))
        if (access(pathname, R_OK) == 0)
            return 200;
        else
            return 403;

    printf(".strcmp old new path: %d\n", strcmp(pathname, newpath) == 0);

    /* here we look for default files */
    if (S_ISDIR(fs.st_mode) && !(match_script(newpath) == 0)){
        if (DEBUG)
            printf(".isdir & not script. looking for defaults!\n");
        /* add directory delimiter if neccessary */
        if (newpath[strlen(newpath) - 1] != '/')
            strcat(newpath, "/");
        x = newpath + strlen(newpath);
        while (*def != NULL){
            strcpy(x, *def);
            if (DEBUG)
                printf(".created/trying newpath: %s\n", newpath);
            stat(newpath, &fs);
            if ((S_ISREG(fs.st_mode) && access(newpath, R_OK) == 0) && (match_script(newpath) != 0))
                return 200;
            strcpy(x, "\0");
            def++;
            }
        return 404;
        }
    else
        return 404;

    if (access(newpath, R_OK) == 0)
        return 200;
    else
        return 404;

    }


/*  checks if the requested file _would be_ in the script directory.
    however, does not check if the file actually exists.
*/
int match_script(char *pathname){

    const extern struct configuration config;
    char *p;
    short f;

    p = pathname;
    p = p + strlen(config.htmldocs); /* skip the html-root */
    f = (strncmp(config.cgibin, p, strlen(config.cgibin)) != 0);  /* does the first part match the scriptroot? */

    if (f)
        return -1;

    if (DEBUG)
        printf(".matches script! %s\n", p);
    return 0;
    }


/*  if status is 200, tries to send filename, else sends errorfile.
    if filename is a script, tries to send its output, or a errorfile if
    that fails.
    calls logging function at the end.
*/
int send_resource(int sock, struct sockaddr_in *caddr, int status_code, char *filename){

    const extern struct configuration config;
    struct stat fs;
    int content_length, fd, r;
    char *errfile = NULL;
    FILE *sf;


    if (DEBUG)
        printf(".in send_reasource() with: %d %s\n", status_code, filename);

    /* read in the correct file/script */
    if (status_code == 200 && match_script(filename) == 0){
        buf = malloc(1024);
        status_code = popen_script(filename, buf);
        content_length = strlen(buf);
        printf(".script returned %d\n", status_code);
        }
    else if (status_code == 200)
        content_length = buffer_static(filename);

    /*  send first header, and buffer the correct file if we are sending
        back an error */
    sf = fdopen(sock, "w");
    if (sf == NULL){
        perror("opening header FILE-wrapper");
        return(-1);
        }
    switch (status_code){
        case 200:   fprintf(sf, "HTTP/1.0 200 OK\r\n");
                    break;
        case 403:   fprintf(sf, "HTTP/1.0 403 Forbidden\r\n");
                    errfile = (char*)malloc(strlen(config.htmldocs) +
                      strlen(config.errors.error_403) + 1);
                    strcpy(errfile, config.htmldocs);
                    strcat(errfile, config.errors.error_403);
                    content_length = buffer_static(errfile);
                    break;
        case 404:   fprintf(sf, "HTTP/1.0 404 Not Found\r\n");
                    errfile = (char*)malloc(strlen(config.htmldocs) +
                      strlen(config.errors.error_404) + 1);
                    strcpy(errfile, config.htmldocs);
                    strcat(errfile, config.errors.error_404);
                    content_length = buffer_static(errfile);
                    break;
        case 500:   fprintf(sf, "HTTP/1.0 500 Internal Server Error\r\n");
                    errfile = (char*)malloc(strlen(config.htmldocs) +
                      strlen(config.errors.error_500) + 1);
                    strcpy(errfile, config.htmldocs);
                    strcat(errfile, config.errors.error_500);
                    content_length = buffer_static(errfile);
                    break;
        case 501:   fprintf(sf, "HTTP/1.0 501 Not Implemented\r\n");
                    errfile = (char*)malloc(strlen(config.htmldocs) +
                      strlen(config.errors.error_501) + 1);
                    strcpy(errfile, config.htmldocs);
                    strcat(errfile, config.errors.error_501);
                    content_length = buffer_static(errfile);
                    break;
        }

    /*  send rest of headers,
        and finally the file */
    fprintf(sf, "Server: %s/%s\r\n", config.name, config.version);
    fprintf(sf, "Content-Length: %d\r\n", content_length);
    if (status_code < 400)
        fprintf(sf, "Content-Type: %s\r\n\r\n", get_type(filename));
    else
        fprintf(sf, "Content-Type: %s\r\n\r\n", get_type(errfile));
    fflush(sf);

    if (DEBUG)
        printf("writing to sock: %d  contentlength: %d\n", sock, content_length);
    r = write(sock, buf, content_length);
    if (r < 0)
        perror("in send_resource(), writing body to sock");
    printf("wrote %d body\n", r);
    if (r == 0 && status_code != 200){
        fprintf(sf, "<html><body><h2 align=center>%d</h2></body></html>\n", status_code);
        fflush(sf);
        }

    write_log(status_code, caddr, filename);
    if (DEBUG)
        printf(".all done. free()ing some core.\n");
    if (buf != NULL)
        free(buf);
    if (errfile != NULL)
        free(errfile);
    if (r != content_length){
        printf(".sending file. only wrote %d of %d bytes\n", r, content_length);
        return(-1);
        }
    return(0);
    }


/*  reads the file to a char buffer.
    returns how much was read, or -1 if not all could be read.
    also returns -1 if file wasn't readable.
    buf must already be malloced.
*/
int buffer_static(char *filename){
    int fd, content_length, r;
    struct stat fs;

    buf = NULL;
    if (DEBUG)
        printf(".in buffer_static with: %s\n", filename);
    if (access(filename, R_OK) != 0)
        return -1;
    stat(filename, &fs);
    content_length = (int)fs.st_size;
    buf = malloc(content_length);
    if (buf == NULL)
        perror("in buffer_static(), mallocing buffer");
    fd = open(filename, O_RDONLY);
    r = read(fd, buf, content_length);
    close(fd);
    if (DEBUG)
        printf(".reading file. read %d of %d bytes\n", r, content_length);
    if (r == content_length)
        return r;
    return(-1);
    }
