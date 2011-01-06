#include "sfcstg.h"
#include "sfcstg_client.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

ClientNetworkManager::ClientNetworkManager(HostName hostname, Port port)
{
  addrinfo hints, *res;
  char service[8];
  sprintf(service, "%d", port);
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  getaddrinfo(hostname, service, &hints, &res);
  this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    this->error("Error: cannot open a socket");
  this->server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    this->set_server_addr();
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) 
      error("ERROR: connection failed");

    FD_ZERO(&read_set);
    FD_SET(sockfd, &read_set);
    FD_ZERO(&write_set);
    FD_SET(sockfd, &write_set);
}
ClientNetworkManager::~ClientNetworkManager(){
  close(sockfd);
}
void ClientNetworkManager::error(const char* msg)
{
  perror(msg);
  exit(0);
}
void ClientNetworkManager::set_server_addr(){
  bzero((char *) &server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, 
	(char *)&server_addr.sin_addr.s_addr,
	server->h_length);
  server_addr.sin_port = htons(port);
}
std::string ClientNetworkManager::read_sock(){
  char buff[256];
  bzero(buff,256);
  int n = read(sockfd, buff,255);
  if(n < 0) error("Error: read failed");
  std::cout << buff << std::endl;
	      //return (std::string)buff;
  std::string s = "ss";
  return s;
}
Color ClientNetworkManager::read_user_data(){
  Color c;
  int n = read(sockfd, &c, sizeof(Color));
  if(n < 0) error("Error: read failed");
  return c;
}

void ClientNetworkManager::read_map(Map* map){
  int n = read(sockfd, map, sizeof(struct Map));
  //std::cout << map.c[0][0].id << std::endl;  
  if(n < 0) error("Error: read failed");
}
void ClientNetworkManager::write_sock(void* data){
  // bzero(buff,256);
  int n = write(sockfd, data, sizeof(data));
  if(n < 0) error("Error: read failed");
}
int ClientNetworkManager::get_readable(Map* map){
  struct timeval timeout;
  timeout.tv_sec = CLIENT_WAIT_SEC;
  timeout.tv_usec = CLIENT_WAIT_USEC;
  if(select(FD_SETSIZE,
	    &read_set,
	    (fd_set *)NULL,
	    (fd_set *)NULL,
	    &timeout) == -1){
    perror("select failed");
    return -1;
  }
  if(FD_ISSET(sockfd, &read_set)){
    this->read_map(map);
  }
  //if(FD_ISSET(sockfd, &write_set)){
  //  this->write_sock("abc");
  //}
}
int ClientNetworkManager::get_writable(void* data){
  struct timeval timeout;
  timeout.tv_sec = CLIENT_WAIT_SEC;
  timeout.tv_usec = CLIENT_WAIT_USEC;
  if(select(FD_SETSIZE,
	    (fd_set *)NULL,
	    &write_set,
	    (fd_set *)NULL,
	    &timeout) == -1){
    perror("select failed");
    return -1;
  }
  // if(FD_ISSET(sockfd, &read_set)){
  //this->read_map(map);
  //  }
  if(FD_ISSET(sockfd, &write_set)){
    this->write_sock(&data);
  }
}

void ClientNetworkManager::close_connection(){
  close(sockfd);
}
