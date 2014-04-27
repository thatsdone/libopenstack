.PHONY : clean

IFLAGS=-I../include
LIB_CFLAGS=-c -g -O0 --std=gnu99 -D_GNU_SOURCE -fpic
LIB_LDFLAGS=-shared -lcurl -ljson
#
CFLAGS=-c -g -O0 --std=gnu99 -D_GNU_SOURCE 
LDFLAGS=-lcurl -ljson

LIB_OBJS=keystone.o swift.o

LIB_TARGET=libopenstack.so
COPST_TARGET=test

all: lib copst

copst: lib
	gcc $(CFLAGS) $(IFLAGS) main.c -o main.o
	gcc main.o $(LDFLAGS) -L/root/dev $(LIB_TARGET) -o $(COPST_TARGET)

lib:
	gcc $(LIB_CFLAGS) $(IFLAGS) keystone.c -o keystone.o
	gcc $(LIB_CFLAGS) $(IFLAGS) swift.c -o swift.o
	gcc $(LIB_OBJS) $(LIB_LDFLAGS) -o $(LIB_TARGET)

clean:
	/bin/rm -f *~ include/openstack/*~ *.o *.so core.* $(COPST_TARGET)
