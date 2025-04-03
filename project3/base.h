#ifndef __COMP3400_base_h__
#define __COMP3400_base_h__

#include <stdbool.h>

#define BUFFER_SIZE 4096
int start_server (char *, char *);
ssize_t retrieve_request (int, char **, char **, char **, char **, char **,
                          char **);
char *html_response (char *, char *);

#endif
