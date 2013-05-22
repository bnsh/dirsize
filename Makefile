CC=g++
CFLAGS=-g3 -O2 -Wall -Werror

SRCS=\
	dirsize.C \
	newfiles.C \
	mystat.C \

OBJS=$(SRCS:C=o)

BINS=\
	dirsize \
	newfiles \
	mystat \

all: $(OBJS) $(BINS)

install: all
	/bin/cp -p dirsize /usr/local/bin/dirsize
	/bin/chmod 755 /usr/local/bin/dirsize
	/bin/cp -p newfiles /usr/local/bin/newfiles
	/bin/chmod 755 /usr/local/bin/newfiles
	/bin/cp -p mystat /usr/local/bin/mystat
	/bin/chmod 755 /usr/local/bin/mystat

clean:
	/bin/rm -f $(BINS) $(OBJS)

dirsize: dirsize.o
	$(CC) $(CFLAGS) $(^) -o $(@)

newfiles: newfiles.o
	$(CC) $(CFLAGS) $(^) -o $(@)

mystat: mystat.o
	$(CC) $(CFLAGS) $(^) -o $(@)

%.o: %.C
	$(CC) -c $(CFLAGS) $(^) -o $(@)
