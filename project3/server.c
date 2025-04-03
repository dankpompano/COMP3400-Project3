#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "base.h"
#include "cgi_response.h"
#include "server.h"

#define CRLF "\r\n"

void
serve_request (int connfd)
{
  char *method = NULL;
  char *uri = NULL;
  char *query = NULL;
  char *version = NULL;
  char *boundary = NULL;
  char *body = NULL;
  ssize_t size = retrieve_request (connfd, &method, &uri, &query, &version,
                                  &boundary, &body);
  if (size < 0)
    {
      // Ignore certain types of request (such as favicon.ico)
      shutdown (connfd, SHUT_RDWR);
      close (connfd);
      return;
    }

  // The following lines are useful for observing what request you get.
  // Specifically, consider the following HTTP request:
  //    GET /cgi-bin/show.cgi?db=hashes.txt&all=true HTTP/1.1\r\n
  //    Hostname: localhost\r\n
  //    Connection: close\r\n
  //    \r\n
  // The variables would have the following values:
  //    method = "GET"                    [dynamically allocated, needs freed]
  //    uri = "cgi-bin/show.cgi"          [dynamically allocated, needs freed]
  //    query = "db=hashes.txt&all=true"  [dynamically allocated, needs freed]
  //    size = 0                        [only meaningful in POST file uploads]
  //    boundary = NULL                 [only meaningful in POST file uploads]
  //    body = NULL                     [only meaningful in POST file uploads]
  printf ("+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
  printf ("METHOD: %s\n", method);
  printf ("URI: %s\n", uri);
  printf ("QUERY: %s\n", query);
  printf ("VERSION: %s\n", version);
  printf ("SIZE: %zd\n", size);
  printf ("BOUNDARY: %s\n", boundary);
  printf ("BODY [each line ends in \\r\\n]:\n%s\n", body);
  printf ("+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

  // TODO [PART]: Replace this hard-coded HTTP response with the contents
  // of an HTML file (if the URI begins with "srv_root") or with the output
  // of executing a CGI program (if the URI begins with "cgi-bin"). For CGI
  // files, you will need to pipe/fork/dup2/exec the program. Also, note that
  // the query string will need to be passed using an environment variable
  // called "QUERY_STRING".
  char *response = html_response (uri, version);
  write (connfd, response, strlen (response));

  shutdown (connfd, SHUT_RDWR);
  close (connfd);

  // Clean up variables and close the connection
  if (method != NULL)
    free (method);
  if (query != NULL)
    free (query);
  if (boundary != NULL)
    free (boundary);
  if (body != NULL)
    free (body);

  // TODO [PART]: If the URI is for the shutdown.cgi file, kill the current
  // process with the SIGUSR1 signal.
  if (uri != NULL)
    free (uri);
    
  if(uri == "cgi-bin/shutdown.cgi")
    kill(connfd,"SIGUSR1");
    
  return;
}
