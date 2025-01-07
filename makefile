server: runserver
	@./runserver

client: runclient
	@./runclient

compile: basic_client.o pipe_networking.o forking_server.o pipe_networking.o
	@gcc -o runclient basic_client.o pipe_networking.o
	@gcc -o runserver forking_server.o pipe_networking.o

basic_client.o: basic_client.c pipe_networking.h
	@gcc -c basic_client.c

forking_server.o: forking_server.c pipe_networking.h
	@gcc -c forking_server.c

pipe_networking.o: pipe_networking.c pipe_networking.h
	@gcc -c pipe_networking.c

clean:
	@rm -f *.o
	@rm -f *~
	@rm -f runclient runserver
