#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_LENGTH 8192

#define CRLF "\r\n"

/* Used to execute a given CGI program in a separate process. Uses the
   version parameter to build the HTTP header. Other parameters are used
   for the FULL implementation only as follows:

     if method is "GET":
       Use query as the value of the QUERY_STRING environment variable.

     if method is "POST":
       Use boundary to split the body of the request (see the project
       page for an example). The size parameter is only used if the POST
       request is for uploading a file, and is the size of the data to
       be uploaded.
 */
char *
cgi_response (char *uri, char *version, char *method, char *query,
              ssize_t size, char *boundary, char *body)
{
  // TODO [PART]: If the URI exists and is executable, run it as a separate
  // process, redirecting its STDOUT back to this process. You can then use
  // that resulting string to determine the Content-Length to send back. As
  // an example, here would be the full response to return for
  // cgi-bin/hello.cgi (assuming HTTP/1.0 and CRLF means "\r\n"). Don't print
  // the quotes:
  //   "HTTP/1.0 200 OK" CRLF
  //   "Content-Type: text/html; charset: UTF-8" CRLF
  //   "Content-Length: 95" CRLF
  //   CRLF
  //   "<html>\n"
  //   "<head>\n"
  //   "  <title>Hello world demo</title>\n"
  //   "</head>\n"
  //   "\n"
  //   "<h2>Hello world!</h2>\n"
  //   "</body>\n"
  //   "</html>\n"
  
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
  char *response = malloc (sizeof (char) * 2056);
  size_t bytes = read (pipe[0], response, 2055);

  // Check if it was successfully read in.
  if(bytes == -1){
    perror("Failed to read in child");
  }

  // Add null character to the end.
  response[bytes] = '\0';
  close (pipe[0]);

  return response;
  
  
  //return strdup ("HTTP/1.0 404 Not Found" CRLF CRLF);

  // TODO [FULL]: Set the environment variables needed for the CGI programs
  // located in cgi-bin. To do this, you will need to use either execve()
  // or execle() when running the CGI program, using an array of string
  // pairs. For example, the following array would set the db and record
  // environment variables:
  //
  //   char *env[] = { "db=foo.txt", "record=2", NULL };
  //
  // If the request is a GET request, you should only set the QUERY_STRING
  // variable to be the query parameter. For POST requests, you will need
  // to look through the body of the HTTP request, splitting based on the
  // boundary values (see the project description for an example).
}
