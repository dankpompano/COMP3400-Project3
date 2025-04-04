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

  //char *response = html_response (uri, version);
  char *response;

  if(strstr(uri, "srv_root") != NULL){
    response = body;
  } else if(strstr(uri, "cgi-bin"))
  {
    // Create child
    int pipe[2];
    pid_t child_pid = fork ();

    if (child_pid < 0)
    { // Fork Failed
      printf ("Fork Failed\n");
      close (pipe[0]);
      close (pipe[1]);
      return EXIT_FAILURE;
    }

    if (child_pid == 0)
    { // Child process
      close (pipe[0]); // Close unused read end

      // Redirect stdout to the write end of the pipe
      dup2 (pipe[1], STDOUT_FILENO);
      close (pipe[1]); // Close original write end

      // Create the enviroment variable
      char *QUERY_STRING[] = {query ,NULL};

      // execlp replaces the current running process with a new process. In
      // this case we are replacing it with the cgi uri which refers to the file we are calling
      // Method is going to be an http request such as "GET" and will act as the argument.
      // QUERY_STRING is the query being requested and combined with NULL so it will end.
      execve (uri, method, QUERY_STRING);

      // Should never get here as it should be in a new process.
      // Only if execlp fails will you get here.
      printf ("execlp failed\n");
      exit (EXIT_FAILURE);
    }
    close (pipe[1]); // Parent does not need to write.

    // Wait for the child to respond.
    waitpid (child_pid, NULL, 0); // wait for child

    // Read in child response.
    response = malloc (sizeof (char) * 2056);
    size_t bytes = read (pipe[0], response, 2055);

    // Check if it was successfully read in.
    if(bytes == -1){
      perror("Failed to read in child");
    }

    // Add null character to the end.
    response[bytes] = '\0';
    close (pipe[0]);
  }

  write (connfd, response, strlen (response));
  free(response);

  // TODO [PART]: If the URI is for the shutdown.cgi file, kill the current
  // process with the SIGUSR1 signal.
    
  if(uri == "cgi-bin/shutdown.cgi")
    kill(connfd,"SIGUSR1");

  // Close the connection.
  shutdown (connfd, SHUT_RDWR);
  close (connfd);

  // Clean up variables.
  if (method != NULL)
    free (method);
  if (uri != NULL)
    free (uri);
  if (query != NULL)
    free (query);
  if (boundary != NULL)
    free (boundary);
  if (body != NULL)
    free (body);

  return;
}
