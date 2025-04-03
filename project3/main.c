/*
 * COMP 3400: Template project driver
 *
 * Name: 
 */

#include <assert.h>
#include <getopt.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "base.h"

int cmdline (int, char **, char **, char **);

void
usage (void)
{
  printf ("Usage: cgi [options]\n");
  printf (" Option is:\n");
  printf ("  -p P        Use application protocol P instead of default port\n");
  printf ("  -f PIDFILE  Use PIDFILE to store the server PID (default PID)\n");
  printf ("\n Sample requests are:\n");
  printf ("  \"GET / HTTP/1.0\"\n");
  printf ("  \"GET /bootstrap.html HTTP/1.1\"\n");
  printf ("  \"GET /cgi-bin/show.cgi?db=hashes.txt&first=1&last=3 HTTP/1.0\n");
  printf ("  \"GET /cgi-bin/show.cgi?db=hashes.txt&all=true HTTP/1.1\n");
  printf ("  \"POST /cgi-bin/upload.cgi?db=alternate.txt HTTP/1.0\n");
}

int
main (int argc, char **argv)
{
  char *protocol = "3456";
  char *pidfile = "PID";
  if (cmdline (argc, argv, &protocol, &pidfile) < 0)
    {
      usage ();
      return EXIT_FAILURE;
    }

  // Start the base server distribution using the port number
  int socketfd = start_server (protocol, pidfile);

  if (socketfd < 0)
    return EXIT_FAILURE;

  // Enter into an infinite loop that accepts requests and passes
  // them off to serve_request() for processing.
  while (1)
    {
      struct sockaddr_in address;
      memset (&address, 0, sizeof (address));
      socklen_t addrlen = sizeof (struct sockaddr_in);
      int connfd = accept (socketfd, (struct sockaddr *) &address, &addrlen);

      if (connfd > 0)
        serve_request (connfd);
    }

  return EXIT_SUCCESS;
}

/* DO NOT MODIFY THIS FUNCTION */

int
cmdline (int argc, char **argv, char **protocol, char **pidfile)
{
  int option;

  while ((option = getopt (argc, argv, "p:f:h")) != -1)
    {
      switch (option)
        {
        case 'p':
          *protocol = optarg;
          break;
        case 'f':
          *pidfile = optarg;
          break;
        case 'h':
          return -1;
          break;
        default:
          return -1;
        }
    }

  return 0;
}
