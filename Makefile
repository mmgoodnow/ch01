# This is a GNU Makefile

EXEC := shell
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
HDRS := $(wildcard *.h)

$(EXEC): $(OBJS)
	gcc -g -o $(EXEC) $(OBJS)

%.o: %.c $(HDRS)
	gcc -c -g -o $@ $<

test:
	perl test.pl

clean:
	rm -rf *.o $(EXEC) *.plist valgrind.out main.out tmp

xfer:
	make clean
	scp -r . systems-vm.local:~/Documents/cs3650/ch01

.PHONY: clean xfer test

