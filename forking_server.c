#include "pipe_networking.h"
int from_client = 0;
int to_client = 0;

void sighandler(int signo){
  if (signo == SIGPIPE){
    printf("Client disconnected\n");
  }
  else if (signo == SIGINT){
    printf("\nServer closing...\n");
    close(from_client);
    close(to_client);
    remove(WKP);
    printf("Removed WKP\n");
    exit(0);
  }
}

int server_handshake_half(int from_client){
  printf("Server waiting for SYN\n");
  char syn[HANDSHAKE_BUFFER_SIZE];
  read(from_client, syn, sizeof(syn));
  printf("Server recieved SYN: %s\n",syn);

  int to_client = open(syn, O_WRONLY);
  
  // int rd = randnum(0, 100);
  int rd = rand() % 100;
  write(to_client, &rd, sizeof(rd));
  printf("Server writing random number: %d \n",rd);

  int ack;
  read(from_client, &ack, sizeof(ack));
  printf("Server reading ACK: %d \n",ack);
  
  int diff = ack - rd;
  if (diff == -1) {
      printf("handshake incomplete\n");
      exit(1);
  }
  return to_client;
}

void subserver(int from_client){
    int to_client = server_handshake_half(from_client);
    char buffer[100];
    while (1){
      int bytesread = read(from_client, buffer, sizeof(buffer));
      if (bytesread <= 0){
        if (bytesread == 0){
          printf("Subserver %d stopped. Client disconnected\n", getpid());
        }
        else {
          perror("could not read from client");
        }
        break;
      }
      printf("Subserver %d recieved: %s\n", getpid(), buffer);

      char message[150];
      snprintf(message, sizeof(message), "Subserver %d processed: %s", getpid(), buffer);
      write(to_client, message, sizeof(message));
      sleep(1);
    }
    close(to_client);
    close(from_client);
    exit(0);
}

int main() {

  // SIGINT/SIGPIPE
  signal(SIGINT,sighandler);
  signal(SIGPIPE,sighandler);
  
  // multiple clients
  printf("Waiting for a new client...\n");

  while (1){
    srand(time(NULL));

    from_client = server_setup();
    printf("Connected\n");

    pid_t f = fork();
    if (f < 0){
        perror("fork failed\n");
        exit(1);
    }
    else if (f == 0){
        subserver(from_client);
    }
    printf("Client disconnect\n");
    close(from_client);
  }
  return 0;
}
