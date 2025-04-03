#include <arpa/inet.h>
#include <assert.h>
#include <check.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../cgi_response.h"

START_TEST (PART_cgi_response)
{
  printf ("======================================\n");
  printf ("PUBLIC\n");
  printf ("  PART cgi response\n");
  printf ("  Given uri=cgi-bin/hello.cgi and version=HTTP/1.0,\n");
  printf ("  build the response (contents ignored):\n\n");
  printf ("    HTTP/1.0 200 OK\\r\\n\n");
  printf ("    Content-Type: text/html; charset=UTF-8\\r\\n\n");
  printf ("    Content-Length: 102\\r\\n\n");
  printf ("    \\r\\n\n");
  printf ("    <html>\n");
  printf ("    <head>\n");
  printf ("      <title>Hello world demo</title>\n");
  printf ("    </head>\n");
  printf ("    \n");
  printf ("    <body>\n");
  printf ("    <h2>Hello world!</h2>\n");
  printf ("    </body>\n");
  printf ("    </html>\n\n");
  char *header = cgi_response ("../cgi-bin/hello.cgi", "HTTP/1.0", "GET",
                               NULL, 0, NULL, NULL);
  char *expected = "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: 102\r\n"
    "\r\n"
    "<html>\n<head>\n  <title>Hello world demo</title>\n</head>\n\n"
    "<body>\n<h2>Hello world!</h2>\n</body>\n</html>\n";
  ck_assert_str_eq (header, expected);
  free (header);
}
END_TEST

START_TEST (PART_cgi_response_shutdown)
{
  printf ("======================================\n");
  printf ("PUBLIC\n");
  printf ("  PART cgi response shutdown\n");
  printf ("  Given uri=cgi-bin/shutdown.cgi and version=HTTP/1.1,\n");
  printf ("  build the response (contents ignored):\n\n");
  printf ("    HTTP/1.1 200 OK\\r\\n\n");
  printf ("    Content-Type: text/html; charset=UTF-8\\r\\n\n");
  printf ("    Content-Length: 102\\r\\n\n");
  printf ("    Connection: close\\r\\n\n");
  printf ("    \\r\\n\n");
  printf ("    <html>\n");
  printf ("    <head>\n");
  printf ("      <title>Success!</title>\n");
  printf ("    </head>\n");
  printf ("    \n");
  printf ("    <body>\n");
  printf ("      <p>Server has stopped</p>\n");
  printf ("    </body>\n");
  printf ("    </html>\n\n");
  char *header = cgi_response ("../cgi-bin/shutdown.cgi", "HTTP/1.1", "GET",
                               NULL, 0, NULL, NULL);
  char *expected = "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: 99\r\n"
    "Connection: close\r\n"
    "\r\n"
    "<html>\n<head>\n  <title>Success!</title>\n</head>\n"
    "<body>\n  <p>Server has stopped</p>\n</body>\n</html>\n";
  ck_assert_str_eq (header, expected);
  free (header);
}
END_TEST

void public_tests (Suite *s)
{
  TCase *tc_public = tcase_create ("Public");
  tcase_add_test (tc_public, PART_cgi_response);
  tcase_add_test (tc_public, PART_cgi_response_shutdown);
  suite_add_tcase (s, tc_public);
}

