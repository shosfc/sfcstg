#ifndef __SFCSTG_SERVER__
#define __SFCSTG_SERVER__
#include "sfcstg.h"
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
#include <vector>
#include <map>
struct Location {
  Loc x;
  Loc y;
};
struct InstructionWithId {
  SocketDescriptor id;
  Object object;
  Direction direction;
};
typedef std::vector<InstructionWithId> InstructionsWithId;
class Unit;
class Game;
class Unit {
 protected:
  Loc x;
  Loc y;
  Color color;
  Game* game;
 public:
  Unit();
  Unit(Loc, Loc, Color);
  void move(Loc, Loc);
  void move_up();
  void move_down();
  void move_right();
  void move_left();
  int get_x();
  int get_y();
  bool is_hit(Unit*);
  bool is_hit_wall();
};
class Bullet : public Unit {
 private:
  enum Direction vec;
 public:
  Bullet(Game*, Loc, Loc, Color, Direction);
  void next();
};
class Fighter : public Unit {
 private:
  SocketDescriptor id;
  void complement_direction(int&, int&, Direction);
 public:
  Fighter(Game*, SocketDescriptor, Loc, Loc, Color);
  void shoot(Direction);
  bool is_hit_wall(Direction);
  Color get_color();
};
typedef std::vector<Bullet*> Bullets;
typedef std::vector<Fighter*> FightersB;
typedef std::map<SocketDescriptor, Fighter*> Fighters;
class Game {
 private:
  Size width;
  Size height;
  Map* map;
  Fighters players;
  Bullets bullets;
  void draw_wall();
 public:
  Game(Size, Size);
  ~Game();
  Size get_width();
  Size get_height();
  void next();
  void refresh();
  Color add_new_client(SocketDescriptor);
  void add_fighter(SocketDescriptor, Fighter*);
  void add_bullet(Bullet*);
  Map create_endmap();
  Map* create_map_data(Map*);
  Color get_available_color();
  Location get_available_location();
  void apply_instructions(InstructionsWithId*);
  std::vector<SocketDescriptor> check_hit();
};

#define BUF_LEN 256
struct CLIENT_INFO {
  char hostname[BUF_LEN];
  char ipaddr[BUF_LEN];
  int port; 
  time_t last_access;  
};
class ServerNetworkManager{
 private:
  char buff[256];
  Port port;
  //  fd_set read_set;
  //fd_set write_set;
  struct timeval waitval;
  struct CLIENT_INFO client_info[FD_SETSIZE];
  int    listening_socket;
  struct sockaddr_in sin;  
  fd_set target_fds;
  fd_set org_target_fds;
 public:
  ServerNetworkManager(Port);
  ~ServerNetworkManager();
  SocketDescriptor accept_new_client(int);
  SocketDescriptor listen_to_join();
  void send_data(void*);
  void send_game_over(std::vector<SocketDescriptor>, Map*);
  void recv_data();
  void recv_instruction(InstructionsWithId*);
  void check_timeout();
};

#endif
