#include "pipe_networking.h"
#define PIPE "pipe"
//UPSTREAM = to the server / from the client
//DOWNSTREAM = to the client / from the server
/*=========================
  server_setup

  creates the WKP and opens it, waiting for a  connection.
  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
  // server making the pipe
  mkfifo(PIPE, 0666);
  // server opening the WKP [blocks]
  int from_client = open(WKP, O_RDONLY); // write too?
  // server removing the WKP
  remove(WKP);
  return from_client;
}

int err(){
  printf("errno %d\n",errno);
  printf("%s\n",strerror(errno));
  exit(1);
}

int randnum(int min, int max){
  int rand;
  int file = open("/dev/urandom", O_RDONLY);
  if (file == -1) err();
  if (read(file, &rand, sizeof(int)) == -1) err();
  if (close(file) == -1) err();
  return abs(rand) % (max - min + 1) + min;
}

/*=========================
  server_handshake 
  args: int * to_client

  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe (Client's private pipe).

  returns the file descriptor for the upstream pipe (see server setup).
  =========================*/
int server_handshake(int *to_client) {
  // server reading SYN (the pid)
  int from_client = server_setup();
  char buffer[100];
  read(from_client, buffer, sizeof(buffer));
  // server opening the private pipe (unblock client)
  *to_client = open(buffer, O_WRONLY);
  // server sending SYN_ACK
  // SYN_ACK to acknowledge SYN, sands a rand int to PP
  int rd = randnum(0, 999999);
  printf("%d\n",rd);
  write(*to_client, &rd, sizeof(rd));

  // server reading final ACK
  // server recieved ACK
  // server recieved ACK, handshake complete
  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  // client creates private pipe using PID
  char pipe[100];
  snprintf(pipe, sizeof(pipe), "%d", getpid());
  mkfifo(pipe, 0666);
  int from_server;
  return from_server;
}


/*=========================
  server_connect
  args: int from_client

  handles the subserver portion of the 3 way handshake

  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int from_client) {
  int to_client  = 0;
  return to_client;
}


