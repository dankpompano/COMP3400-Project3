#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main (void)
{
  // TODO: Print the following message (eliminate the leading spaces on each
  // line):
  //    <html>
  //    <head>
  //      <title>Success!</title>
  //    </head>
  //    <body>
  //      <p>Server has stopped</p>
  //    </body>
  //    </html>
  // Once this program finishes running, the server should shut itself down.
  // (This shutdown is handled in serve.c, not here.)
  printf ("<html>\n");
  printf("<head>\n  <title>Success!</title>\n</head>\n");
  printf ("<body>\n <p>Server has stopped</p>\n</body>\n");
  printf ("</html>\n");
  return 0;
}
