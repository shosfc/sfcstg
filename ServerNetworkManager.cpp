#include "sfcstg_server.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <netdb.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

ServerNetworkManager::ServerNetworkManager(Port port){
  this->waitval.tv_sec  = SERVER_WAIT_SEC;
  this->waitval.tv_usec = SERVER_WAIT_USEC;
  int sock_optval = 1;

  listening_socket = socket(AF_INET, SOCK_STREAM, 0);
  if ( setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR,
                  &sock_optval, sizeof(sock_optval)) == -1 ){
    perror("setsockopt");
    exit(1);
  }
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  if ( bind(listening_socket, (struct sockaddr *)&sin, sizeof(sin)) < 0 ){
    perror("bind");
    exit(1);
  }
  if ( listen(listening_socket, SOMAXCONN) == -1 ){
    perror("listen");
    exit(1);
  }
  printf("listening Port:%d\n", port);
  FD_ZERO(&org_target_fds);
  FD_SET(listening_socket, &org_target_fds);
}

ServerNetworkManager::~ServerNetworkManager(){
  close(listening_socket);
}

SocketDescriptor ServerNetworkManager::listen_to_join(){
    int i;
    int new_sock = -1;
    memcpy(&target_fds, &org_target_fds, sizeof(org_target_fds));
    select(FD_SETSIZE, &target_fds, NULL, NULL, &waitval);
    for ( i=0 ; i<FD_SETSIZE ; i++ ){
      if ( FD_ISSET(i, &target_fds) ){
        if ( i == listening_socket ){
          new_sock = accept_new_client(i);
          if ( new_sock != -1 ){
            FD_SET(new_sock, &org_target_fds);
	    return new_sock;
          }
        }
      }
    }
    return new_sock;
 }

void ServerNetworkManager::check_timeout(){
  time_t now_time;
  time(&now_time); 
  for (int i=0 ; i<FD_SETSIZE ; i++ ){
    if ( ! FD_ISSET(i, &org_target_fds) )  continue;
    if ( i == listening_socket ) continue;
    if ( now_time-30 > client_info[i].last_access ){
      printf("Timeout: %s (%s) Port:%d FD: %d no response in 30 sec\n",
	     client_info[i].hostname,
	     client_info[i].ipaddr,
	     client_info[i].port,
	     i);
      close(i);
      FD_CLR(i, &org_target_fds);
    }
  }
}

void ServerNetworkManager::recv_data(){
  select(FD_SETSIZE, &target_fds, NULL, NULL, &waitval);
  for (int i=0 ; i<FD_SETSIZE ; i++ ){
  if ( !i == listening_socket ){
    int read_size;
    char buff[16];
    read_size = read(i, buff, sizeof(buff) -1);
    buff[read_size] = '\0';
    printf("%s", buff);
    if ( read_size == -1 || read_size == 0 ){
      FD_CLR(i, &org_target_fds);
    }
  }
  }
}

void ServerNetworkManager::recv_instruction(InstructionsWithId* insts){
  memcpy(&target_fds, &org_target_fds, sizeof(org_target_fds));
  if(select(FD_SETSIZE,
	    &target_fds,
	    (fd_set *)NULL,
	    (fd_set *)NULL,
	    &waitval) == -1){
    perror("select failed");
  }
  for (int i=0 ; i<FD_SETSIZE ; i++ ){
    if ( !i == listening_socket ) continue;
    if(FD_ISSET(i, &target_fds)){
      int read_size;
      Instruction inst;
      InstructionWithId instwithid;
      read_size = read(i, &inst, sizeof(inst));
      instwithid.id = i;
      instwithid.object = inst.object;
      instwithid.direction = inst.direction;
      insts->push_back(instwithid);
      //printf("%d, %d, %d", instwithid.id, instwithid.object, instwithid.direction);
      if ( read_size == -1 || read_size == 0 ){
	FD_CLR(i, &org_target_fds);
      }
    }
  }
}

void ServerNetworkManager::send_game_over(std::vector<SocketDescriptor> sdlist, Map* endmap){
  std::vector<SocketDescriptor>::iterator i = sdlist.begin();
  while ( i != sdlist.end() ){
    int n = write(*i, endmap, sizeof(struct Map));
    printf("FD %d closed\n", *i);
    close(*i);
    FD_CLR(*i, &org_target_fds);
    FD_CLR(*i, &target_fds);
    i++;
  }
}

void ServerNetworkManager::send_data(void* map){
  memcpy(&target_fds, &org_target_fds, sizeof(org_target_fds));
  if(select(FD_SETSIZE,
	    (fd_set *)NULL,
	    &target_fds,
	    (fd_set *)NULL,
	    &waitval) == -1){
    perror("select failed");
    //exit(1);
  }
  for ( int i = 0 ; i < FD_SETSIZE ; i++ ){
    if ( i == listening_socket ) continue;
    if(FD_ISSET(i, &target_fds)){
      //printf("%d write\n", i);
      int n = write(i, map, sizeof(struct Map));
      if (n == -1 || n == 0) {
	//close(i);
	FD_CLR(i, &org_target_fds);
      }
    }
  }
}

int ServerNetworkManager::accept_new_client(int sock){
  int len;
  int new_socket;
  struct hostent *peer_host;
  struct sockaddr_in peer_sin;
  int listning_socket;
  len = sizeof(sin);
  new_socket = accept(listening_socket, (struct sockaddr *)&sin, (socklen_t*)&len);
  if ( new_socket == -1 ){
    perror("accept");
    exit(1);
  }
  if ( new_socket > FD_SETSIZE-1 ){
    return -1;
  }
  // debug
  len = sizeof(peer_sin);
  getpeername(new_socket,
              (struct sockaddr *)&peer_sin, (socklen_t*)&len);
  
  peer_host = gethostbyaddr((char *)&peer_sin.sin_addr.s_addr,
                            sizeof(peer_sin.sin_addr), AF_INET);
  // host
  strncpy(client_info[new_socket].hostname, peer_host->h_name,
          sizeof client_info[new_socket].hostname);
  // IP
  strncpy(client_info[new_socket].ipaddr, inet_ntoa(peer_sin.sin_addr),
          sizeof client_info[new_socket].ipaddr);
  // port
  client_info[new_socket].port = ntohs(peer_sin.sin_port);
  // time
  time(&client_info[new_socket].last_access);
  printf("Connected: %s (%s) Port:%d  FD: %d\n",
         client_info[new_socket].hostname,
         client_info[new_socket].ipaddr,
         client_info[new_socket].port,
         new_socket);
  return new_socket;
}
