#include "pipe_networking.h"
//UPSTREAM = to the server / from the client
//DOWNSTREAM = to the client / from the server

// erno
int err(){
  printf("errno %d\n",errno);
  printf("%s\n",strerror(errno));
  exit(1);
}

// random number
int randnum(int min, int max){
  int rand;
  int file = open("/dev/urandom", O_RDONLY);
  if (file == -1) err();
  if (read(file, &rand, sizeof(int)) == -1) err();
  if (close(file) == -1) err();
  return abs(rand) % (max - min + 1) + min;
}

/*=========================
  server_setup

  creates the WKP and opens it, waiting for a  connection.
  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
  // server making the pipe
  if (mkfifo(WKP, 0644) == -1){
    perror("error creating WKP");
    exit(1);
  }
  printf("Server made WKP\n");

  // server opening the WKP [blocks]
  int from_client = open(WKP, O_RDONLY);
  if (from_client < 0){
    perror("error opening wkp");
    exit(1);
  }
  printf("Server opened WKP\n");

  // server removing the WKP
  if (remove(WKP) == -1){
    perror("error removing wkp");
    exit(1);
  }
  printf("Server removed WKP\n");
  return from_client;
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
  char buffer[HANDSHAKE_BUFFER_SIZE];
  if (read(from_client, buffer, sizeof(buffer)) == -1){
    err();
  }
  printf("Server read SYN with PID: %s\n",buffer);
  // server opening the private pipe (unblock client)
  *to_client = open(buffer, O_WRONLY);
  if (*to_client == -1){
    perror("error opening pp");
    exit(1);
  }
  // server sending SYN_ACK
  // SYN_ACK to acknowledge SYN, sands a rand int to PP
  int rd = randnum(0, 100);
  write(*to_client, &rd, sizeof(rd));
  printf("Server writing random number: %d \n",rd);

  // server reading final ACK
  int ack;
  // read(from_client, &ack, sizeof(ack));
  read(from_client, &rd, sizeof(rd));
  printf("Server reading ACK: %d \n",ack);

  // server recieved ACK, handshake complete
  // printf("ack: %d, rd: %d\n",ack,rd);
  int diff = ack - rd;
  if (diff == -1) {
    printf("handshake incomplete\n");
    exit(1);
  }
  printf("Handshake complete\n");
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
  char PP[HANDSHAKE_BUFFER_SIZE];
  snprintf(PP, sizeof(PP), "%d", getpid());
  mkfifo(PP, 0644);
  printf("Client created private pipe named: %s\n",PP);

  // client opening WKP (unblock server)
  *to_server = open(WKP, O_WRONLY);
  printf("Client opened WKP\n");

  // client writing PP to WKP
  write(*to_server, PP, sizeof(PP));
  printf("Client wrote private pipe to WKP\n");

  // client opening PP (blocks)
  int from_server;
  from_server = open(PP, O_RDONLY);
  printf("Client opened private pipe\n");

  // client deleting PP
  remove(PP);
  printf("Client deleted private pipe\n");

  // client reading SYN_ACK
  int synAck;
  read(from_server, &synAck, sizeof(synAck));
  printf("Client read SYN_ACK\n");

  // client sending ACK
  int ack = synAck + 1;
  write(*to_server, &ack, sizeof(ack));
  printf("Client sent ACK\n");
  return from_server;
}


/*=========================
  server_connect
  args: int from_client

  handles the subserver portion of the 3 way handshake

  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int from_client) {
  // char buffer[HANDSHAKE_BUFFER_SIZE];
  // read(from_client, buffer, sizeof(buffer));
  // int to_client = open(buffer, O_WRONLY);
  int to_client = 0;
  return to_client;
}
