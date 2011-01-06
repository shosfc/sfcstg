#ifndef __SFCSTG_CLIENT__
#define __SFCSTG_CLIENT__
#include "sfcstg.h"
#include "sfcColor.h"
#include "sfcLibR.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sys/select.h>

class ClientNetworkManager {
 private:
  Port port;
  SocketDescriptor sockfd;
  struct sockaddr_in server_addr;
  struct hostent *server;
  fd_set read_set;
  fd_set write_set;
  void error(const char*);
  void set_server_addr();
 public:
  ClientNetworkManager(HostName, Port);
  ~ClientNetworkManager();
  std::string read_sock();
  Color read_user_data();
  void read_map(Map*);
  void write_sock(void*);
  int get_readable(Map*);
  int get_writable(void*);
  void close_connection();
};

class Client {
 private:
  Size width;
  Size height;
  ClientNetworkManager* manager;
 public:
  Client(Size, Size, ClientNetworkManager*);
  ~Client();
  void refresh();
  Instruction key_handler();
  void draw_map(Map*);
  bool check_game_over(Map*);
};
#endif
