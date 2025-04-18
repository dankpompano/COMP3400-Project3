#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_LENGTH 8192

#define CRLF "\r\n"

//return the amount that we find
int postParser(char** db, char** hash, char** record, char* body, char* boundary)
{
  int count = 0;
  
  if (body == NULL || boundary == NULL) {
    return 0; // If body or boundary is NULL, nothing to parse
  }

  char* location = strstr(body, boundary); // Start looking for the boundary
  while (location != NULL) {
    location += strlen(boundary); // Skip the boundary
    
    // Look for "name=" keyword
    location = strstr(location, "name=\"");
    if (location == NULL) {
        break; // No more fields to parse
    }
    location += 6; // Skip over "name=\""

    // Debug print to see what location holds
    //printf("Debug: location = %s\n", location);

    // Check for each possible field
    if (strncmp(location, "db\"", 3) == 0) {
      location += 3; // Skip over the "db" field and the quote mark
      char* dbEND = strstr(location, "\r\n\r\n");
      if (dbEND == NULL) break; // Invalid data format, stop processing
      dbEND[0] = '\0'; // Null-terminate the string

      location = dbEND + 4; // Move past the \r\n to the value of db

      // Read in the value stopping at the new line.
      //printf("Debug: record Location = %s\n", location); // Debug print
      char* value_end = strstr(location, "\r\n");
      if (value_end == NULL) break; // No value for db, break
      value_end[0] = '\0'; // Null-terminate the value of db

      *db = strdup(location);
      if (*db == NULL) {
        perror("Failed to allocate memory for db");
        return -1; // Return error if strdup fails
      }
      count++;
      //printf("Debug: db = %s\n", *db); // Debug print
    }
    else if (strncmp(location, "hash\"", 5) == 0) {
      location += 5; // Skip over the "hash" field and the quote mark
      char* hashEND = strstr(location, "\r\n\r\n");
      if (hashEND == NULL) break;
      hashEND[0] = '\0'; // Null-terminate the string

      location = hashEND + 4; // Move past the \r\n to the value of hash

      // Read in the value stopping at the new line.
      //printf("Debug: hash Location = %s\n", location); // Debug print
      char* value_end = strstr(location, "\r\n");
      if (value_end == NULL) break; // No value for hash, break
      value_end[0] = '\0'; // Null-terminate the value of hash

      *hash = strdup(location);
      if (*hash == NULL) {
        perror("Failed to allocate memory for hash");
        return -1;
      }
      count++;
      //printf("Debug: hash = %s\n", *hash); // Debug print
    }
    else if (strncmp(location, "record\"", 7) == 0) {
      location += 7; // Skip over the "record" field and the quote mark
      char* recordEND = strstr(location, "\r\n\r\n");
      if (recordEND == NULL) break;
      recordEND[0] = '\0'; // Null-terminate the string

      location = recordEND + 4; // Move past the \r\n to the value of record

      // Read in the value stopping at the new line.
      //printf("Debug: record Location = %s\n", location); // Debug print
      char* value_end = strstr(location, "\r\n");
      if (value_end == NULL) break; // No value for record, break
      value_end[0] = '\0'; // Null-terminate the value of record

      *record = strdup(location);
      if (*record == NULL) {
        perror("Failed to allocate memory for record");
        return -1;
      }
      count++;
      //printf("Debug: record = %s\n", *record); // Debug print
    }

    location++;
    //printf("Debug: location = %s\n", location); // Debug print
  }

  return count;
}




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
  int pipefd[2];

  // Turn pipefd into an actual pipe.
  if (pipe(pipefd) == -1) {
    perror("Pipe failed to create.");
    return NULL;
  }

  pid_t child_pid = fork ();

  // Check if fork failed
  if (child_pid < 0)
  {
    perror("Forking failed within pipe.");
    close(pipefd[0]);
    close(pipefd[1]);
    return NULL;
  }

  // Child process
  if (child_pid == 0)
  {
    // Close unused read end
    close (pipefd[0]); 

    // Redirect stdout to the write end of the pipe
    dup2 (pipefd[1], STDOUT_FILENO);
    
    // Prepare arguments and environmentArguments
    char *arguments[] = { uri, NULL }; // We just need the uri as the argument.
    char *environmentArguments[10]; // We should never have more than 10 arguments.
    int currentArgument = 0;

    // Create the enviroment variable
    char QUERY_STRING[1024];

    if(strcmp(method, "GET") == 0){ // GET method environmentArgument
      snprintf(QUERY_STRING, sizeof(QUERY_STRING), "QUERY_STRING=%s", query); // Copy the query into the environment arguments.
    } else if (strcmp(method, "POST") == 0) // POST method environmentArgument
    {
      // Clone the body so we can safely modify it. strtok inserts \0 into the body.
      char *body_copy = strdup(body);
      if(body_copy != NULL && boundary != NULL)
      {
        //printf("Debug (POST): Body to parse = %s\n", body);

        // Parse POST body with boundary
        char *db = NULL, *hash = NULL, *record = NULL;
        int parsedCount = postParser(&db, &hash, &record, body, boundary); // Parse POST data
        printf("Debug (POST): Parsed %d variables\n", parsedCount);

        // Set environment variables for POST
        if (db) {
          char db_env[1024];
          snprintf(db_env, sizeof(db_env), "db=%s", db);
          environmentArguments[currentArgument++] = strdup(db_env);
          free(db);  // Free after setting the environment variable
        }
        if (record) {
          char record_env[1024];
          snprintf(record_env, sizeof(record_env), "record=%s", record);
          environmentArguments[currentArgument++] = strdup(record_env);
          free(record);  // Free after setting the environment variable
        }
        if (hash) {
          char hash_env[1024];
          snprintf(hash_env, sizeof(hash_env), "hash=%s", hash);
          environmentArguments[currentArgument++] = strdup(hash_env);
          free(hash);  // Free after setting the environment variable
        }
      }
    } else {
      perror("Invalid Method.");
      exit(1);
    }

    // If there is a query to read we add it to the argument array.
    if(query != NULL){
      environmentArguments[currentArgument] = QUERY_STRING;
      currentArgument++;
    }

    // Null at the end of arguments to represent when they end.
    environmentArguments[currentArgument] = NULL;
    currentArgument++;

    // execlp replaces the current running process with a new process. In
    // this case we are replacing it with the cgi uri which refers to the file we are calling
    // The first parameter is the path to the binary that we want to execute.
    // The second parameter are the arguments we are giving to it including the command name.
    // The third parameter are the environment variables being added onto it.
    execve (uri, arguments, environmentArguments);

    // Should never get here as it should be in a new process.
    // Only if execlp fails will you get here.
    perror ("execve failed");

    // Free all the arguments in case we failed.
    for(int i = 0; i < currentArgument; i++) {
      free(environmentArguments[i]);
    }

    return NULL;
  }

  // Parent process starts here.
  close (pipefd[1]); // Parent does not need to write.

  // Read in child response.
  char *response = malloc (sizeof(char) * BUFFER_LENGTH);
  size_t totalBytes = 0;
  size_t currentBytes = 0;

  // Loop through until there is nothing left to read. 
  // Increment response pointer location by totalbytes to get the correct position.
  while ((currentBytes = read(pipefd[0], response + totalBytes, BUFFER_LENGTH - totalBytes - 1)) > 0) {
    totalBytes += currentBytes;
  }

  // Add null character to the end.
  response[totalBytes] = '\0';
  close (pipefd[0]);

  // We have our response (body) from the program but it is missing the headers.
  // Gather the size of the body
  size_t content_length = totalBytes;

  // Calculate the total length of the header and body
  char *final_response = malloc(sizeof(char) * (totalBytes + 1024)); // Extra space for the headers
  if (final_response == NULL) {
      perror("Failed to allocate memory for final response");
      return NULL;
  }

  // If 1.1 then add connection closed.
  char* versionExtra = "";
  if(strcmp(version, "HTTP/1.1") == 0){
    versionExtra = "Connection: close\r\n";
  }

  // Build the response headers assuming max size of 1024
  snprintf(final_response, 1024,
          "%s 200 OK\r\n"
          "Content-Type: text/html; charset=UTF-8\r\n"
          "Content-Length: %zu\r\n"
          "%s"
          "\r\n", version, content_length, versionExtra);
  
  // Append the body content to the final response
  strncat(final_response, response, totalBytes);

  // Free the response buffer
  free(response);

  return final_response;
  
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