#include "sfcstg_client.h"
#include <iostream>

Client::Client(Size width, Size height, ClientNetworkManager* manager){
  this->width = width;
  this->height = height;
  this->manager = manager;
  sfcInit(width, height);
  sfcStartReal();
}
Client::~Client(){}
Instruction Client::key_handler(){
  char c = sfcInKey();
  Instruction inst = {NULL_OBJECT, NULL_DIRECTION};
  switch(c){
  case 'i':
    inst.object = FIGHTER;
    inst.direction = UP;
    break;
  case 'k':
    inst.object = FIGHTER;
    inst.direction = DOWN;
    break;
  case 'l':
    inst.object = FIGHTER;
    inst.direction = RIGHT;
    break;
  case 'j':
    inst.object = FIGHTER;
    inst.direction = LEFT;
    break;
  case 'w':
    inst.object = BULLET;
    inst.direction = UP;
    break;
  case 's':
    inst.object = BULLET;
    inst.direction = DOWN;
    break;
  case 'a':
    inst.object = BULLET;
    inst.direction = LEFT;
    break;
  case 'd':
    inst.object = BULLET;
    inst.direction = RIGHT;
    break;
  }
  return inst;
}
void Client::draw_map(Map* map){
  for(int i = 0; i < this->width; i++)
    for(int j = 0; j < this->height; j++)
      sfcPoint(i,
	       j,
	       map->cell[i][j].color);
}
bool Client::check_game_over(Map* map){
  for(int i = 0; i < this->width; i++)
    for(int j = 0; j < this->height; j++)
      if(map->cell[i][j].color != NULL_COLOR) return false;
  return true;
}
