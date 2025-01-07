#include "pipe_networking.h"
int from_client, from_server;

void sighandler(int signo){
  if (signo == SIGPIPE){
    printf("Client disconnected\n");
  }
  else if (signo == SIGINT){
    printf("\nClient shut down.\n");
    exit(0);
  }
}

// erno
int err2(){
  printf("errno %d\n",errno);
  printf("%s\n",strerror(errno));
  exit(1);
}

int randNumber(int min, int max){
  int rand;
  int file = open("/dev/urandom", O_RDONLY);
  if (file == -1) err2();
  if (read(file, &rand, sizeof(int)) == -1) err2();
  if (close(file) == -1) err2();
  return abs(rand) % (max - min + 1) + min;
}

int main() {
  signal(SIGINT, sighandler);
  signal(SIGPIPE, sighandler);

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );
  
  // send message
  char buffer[100];
  while (1){
    snprintf(buffer, sizeof(buffer), "Number %d\n", randNumber(0,100));
    ssize_t bytes = write(to_server, &buffer, sizeof(buffer));
    if (bytes <= 0){
      break;
    }
    printf("Client sent: %s",buffer);
    sleep(1);
  }

  //recieve message
  char buff[100];
  while (1){
    ssize_t bytesrecieved = read(from_server, &buff, sizeof(buff));
    if (bytesrecieved <= 0){
      break;
    }
    // else if (buff[0] != '\0'){
    //   printf("Client recieved: %s\n",buff);
    // }
  }
}
