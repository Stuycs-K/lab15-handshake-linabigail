#include "pipe_networking.h"

int main() {

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );

  while (1){
    // repeatedly read the int sent by server + print it
    int num;
    ssize_t bytes = read(from_server, &num, sizeof(num));
    if (bytes <= 0){
      printf("Server disconnect.\n");
      break;
    }
    printf("Recieved Number: %d\n", num);
  }
}
