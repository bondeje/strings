.POSIX:
.OBJDIR: ./
CC = gcc

STDC = -std=c99
CFLAGS = -Wall -Wextra -Wno-unused -Wno-unused-parameter -pedantic $(STDC)
DBG_CFLAGS = $(CFLAGS) -O0 -g3

OBJS = strings.o
TEST_OBJS = strings.do test_utils.do test_strings.do

all: $(OBJS) test

.MAIN: all

.SUFFIXES: .c .o .do
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
.c.do:
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="" ; fi ; \
	$(CC) $(DBG_CFLAGS) $$DBGOPT -c $< -o $@

test: $(TEST_OBJS)
	if [ -n "$(SANITIZE)" ] ; then export DBGOPT="-fsanitize=address,undefined"; else export DBGOPT="" ; fi ; \
	$(CC) $(DBG_CFLAGS) $$DBGOPT $(TEST_OBJS) -o $@

check: test
	./test --verbose
	
clean:
	rm -f test *.o *.do
