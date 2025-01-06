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
    char syn[HANDSHAKE_BUFFER_SIZE];
    read(from_client, syn, sizeof(syn));

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
    snprintf(buffer, sizeof(buffer), "Subserver %d", getpid());
    while (read(from_client, buffer, sizeof(buffer)) > 0){
        printf("%s\n",buffer);
    }
    close(to_client);
    close(from_client);
    exit(0);
}

int main() {
  srand(getpid());

  // SIGINT/SIGPIPE
  signal(SIGINT,sighandler);
  signal(SIGPIPE,sighandler);
  
  // multiple clients
  printf("Waiting for a new client...\n");

  while (1){

    from_client = server_setup();
    printf("Connected\n");
    pid_t f = fork();
    if (f < 0){
        perror("fork failed\n");
        exit(1);
    }
    else if (f == 0){
        remove(WKP);
        subserver(from_client);
    }
    printf("Client disconnect\n");
    close(to_client);
    close(from_client);
    exit(0);
  }
}
