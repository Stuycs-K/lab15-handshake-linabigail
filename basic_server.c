#include "pipe_networking.h"

int main() {
  int to_client;
  int from_client;
  char num;
  srand(getpid());

  while (1){
    from_client = server_handshake( &to_client );
    int rd = rand() % 10;
    printf("Message Sent to Client: %d\n",rd);
    write(to_client, &rd, sizeof(rd));
    sleep(1);
    close(to_client);
    close(from_client);
  }
}