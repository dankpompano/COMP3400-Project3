#include <stdio.h>
#include <stdlib.h>

int
main (void)
{
  printf ("<html>\n");
  printf ("<head>\n  <title>Hello world demo</title>\n</head>\n\n");
  printf ("<body>\n");
  printf ("<h2>Hello world!</h2>\n");
  char *env = getenv ("QUERY_STRING");
  if (env != NULL)
    printf ("<p>Query string: %s</p>\n", env);

  printf ("</body>\n");
  printf ("</html>\n");
  return 0;
}
