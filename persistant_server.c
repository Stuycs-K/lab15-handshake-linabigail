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

int main() {
  srand(getpid());

  // SIGINT
  signal(SIGINT,sighandler);
  signal(SIGPIPE,sighandler);

  while (1){

    // multiple clients
    printf("Waiting for a new client...\n");

    from_client = server_handshake( &to_client );
    
    while (1){
      int rd = rand() % 100;
      printf("Message Sent to Client: %d\n",rd);
      if ( write(to_client, &rd, sizeof(rd)) == -1){
        perror("client disconnected");
        break;
      }
      sleep(1);
    }
    printf("Client disconnect\n");
    close(to_client);
    close(from_client);
    to_client = 0;
    from_client = 0;
  }
  return 0;
}
