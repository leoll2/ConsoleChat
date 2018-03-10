CC = gcc
CFLAGS = -Wall -std=c99
INC = -Iinclude/
INCL = -Iinclude/list/

.PHONY: all clean

all: msg_client msg_server

build/cl_console.o: src/cl_console.c src/client.h
	$(CC) $(CFLAGS) $(INC) -c src/cl_console.c -o build/cl_console.o

build/cl_exec.o: src/cl_exec.c src/client.h
	$(CC) $(CFLAGS) $(INC) -c src/cl_exec.c -o build/cl_exec.o

build/cl_main.o: src/cl_main.c src/client.h
	$(CC) $(CFLAGS) $(INC) -c src/cl_main.c -o build/cl_main.o
	
build/cl_network.o: src/cl_network.c src/client.h
	$(CC) $(CFLAGS) $(INC) -c src/cl_network.c -o build/cl_network.o

build/commands.o: src/commands.c src/commands.h
	$(CC) $(CFLAGS) $(INC) -c src/commands.c -o build/commands.o
	
build/liblist.a: \
	include/list/list.o\
	include/list/list_chat_user.o \
	include/list/list_message.o \
	include/list/list_int.o \
	include/list/list_string.o
	ar rcs build/liblist.a include/list/*.o
	
build/sv_main.o: src/sv_main.c src/server.h
	$(CC) $(CFLAGS) $(INC) -c src/sv_main.c -o build/sv_main.o
	
build/sv_exec.o: src/sv_exec.c src/server.h
	$(CC) $(CFLAGS) $(INC) -c src/sv_exec.c -o build/sv_exec.o

build/sv_console.o: src/sv_console.c src/server.h
	$(CC) $(CFLAGS) $(INC) -c src/sv_console.c -o build/sv_console.o

build/sv_network.o: src/sv_network.c src/server.h
	$(CC) $(CFLAGS) $(INC) -c src/sv_network.c -o build/sv_network.o
	
include/list/list_chat_user.o: include/list/list.o include/list/list_chat_user.c include/list/list_chat_user.h
	$(CC) $(CFLAGS) $(INC) -c include/list/list_chat_user.c -o include/list/list_chat_user.o
	
include/list/list_message.o: include/list/list.o include/list/list_message.c include/list/list_message.h
	$(CC) $(CFLAGS) $(INC) -c include/list/list_message.c -o include/list/list_message.o

include/list/list_int.o: include/list/list.o include/list/list_int.c include/list/list_int.h
	$(CC) $(CFLAGS) $(INC) -c include/list/list_int.c -o include/list/list_int.o

include/list/list_string.o: include/list/list.o include/list/list_string.c include/list/list_string.h
	$(CC) $(CFLAGS) $(INC) -c include/list/list_string.c -o include/list/list_string.o

include/list/list.o: include/list/list.c include/list/list.h include/types.h
	$(CC) $(CFLAGS) $(INC) -c include/list/list.c -o include/list/list.o
	
include/inout_util/inout_util.o: include/inout_util/inout_util.c include/inout_util/inout_util.h
	$(CC) $(CFLAGS) -c include/inout_util/inout_util.c -o include/inout_util/inout_util.o
	
include/struct_util/struct_util.o: include/struct_util/struct_util.c include/struct_util/struct_util.h
	$(CC) $(CFLAGS) $(INC) -c include/struct_util/struct_util.c -o include/struct_util/struct_util.o

msg_client:	build/cl_main.o	\
	build/cl_exec.o	\
	build/cl_network.o \
	build/cl_console.o \
	build/commands.o \
	build/liblist.a \
	include/inout_util/inout_util.o \
	include/struct_util/struct_util.o
	$(CC) build/cl_main.o \
	build/cl_exec.o \
	build/cl_network.o \
	build/cl_console.o \
	include/inout_util/inout_util.o \
	include/struct_util/struct_util.o \
	build/commands.o -lpthread -Lbuild -llist -o msg_client
	
msg_server: build/sv_main.o \
	build/sv_exec.o \
	build/sv_console.o \
	build/sv_network.o \
	build/commands.o \
	build/liblist.a \
	include/inout_util/inout_util.o \
	include/struct_util/struct_util.o
	$(CC) build/sv_main.o \
	build/sv_exec.o \
	build/sv_console.o \
	build/sv_network.o \
	include/inout_util/inout_util.o \
	include/struct_util/struct_util.o \
	build/commands.o -Lbuild -llist -o msg_server
	
clean:
	rm -f build/*
	rm -f include/list/*.o
	rm -f include/inout_util/*.o
	rm -f include/struct_util/*.o
	rm -f msg_client
	rm -f msg_server

