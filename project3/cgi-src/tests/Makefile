#
# Simple Test Makefile
# Mike Lam, James Madison University, August 2016
#
# This version of the Makefile includes support for building a test suite. The
# recommended framework is Check (http://check.sourceforge.net/). To build and
# run the test suite, execute the "test" target. The test suite must be located
# in a module called "testsuite". The MODS, LIBS, and OBJS variables work as
# they do in the main Makefile.
#
# To change the default build target (which executes when you just type
# "make"), change the right-hand side of the definition of the "default"
# target.
#
# By default, this makefile will build the project with debugging symbols and
# without optimization. To change this, edit or remove the "-g" and "-O0"
# options in CFLAGS and LDFLAGS accordingly.
#
# By default, this makefile build the application using the GNU C compiler,
# adhering to the C99 standard with all warnings enabled.


# application-specific settings and run target

include ../Make.src

ITESTOUT=itests.txt

all:
	make -C ../

test: itest
	@echo "========================================"

itest: $(CGI)
	@echo "========================================"
	@echo "        CGI INTEGRATION TESTS"
	@./integration.sh | tee $(ITESTOUT)


# build targets
CC=gcc
CFLAGS=-O0 -Wall --std=gnu99 -pedantic
LDFLAGS=-O0

%.cgi: ../%.c
	$(CC) $(CFLAGS) -o ../../cgi-bin/$@ $<

clean:
	rm -rf $(ITESTOUT) outputs

.PHONY: all clean test itest
