#ifndef __SFCSTG__
#define __SFCSTG__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <vector>
const int WIDTH = 20;
const int HEIGHT = 10;
const int SERVER_USLEEP = 200000;
const int CLIENT_USLEEP = 100000;
const int SERVER_WAIT_SEC = 0;
const int CLIENT_WAIT_SEC = 1;
const int SERVER_WAIT_USEC = 0;
const int CLIENT_WAIT_USEC = 0;

typedef int Loc;
typedef int Size;
typedef int ID;
typedef int Port;
typedef int SocketDescriptor;
typedef char* HostName;
enum Direction { NULL_DIRECTION, UP, DOWN, RIGHT, LEFT};
enum Object { NULL_OBJECT, FIGHTER, BULLET, WALL};
enum Color { BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, YELLOW, WHITE, NULL_COLOR};
struct Cell{
  enum Color color;
};
struct Map {
  struct Cell cell[WIDTH][HEIGHT];
};
struct Instruction{
  Object object;
  Direction direction;
};
typedef std::vector<Instruction> Instructions;
#endif
