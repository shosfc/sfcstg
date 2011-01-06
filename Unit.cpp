#include "sfcstg_server.h"

Unit::Unit(){}
Unit::Unit(Loc x, Loc y, Color c){
  this->x = x;
  this->y = y;
  this->color = c;
}
void Unit::move(Loc x, Loc y){
  this->x = x;
  this->y = y;
}
void Unit::move_up(){ this->move(this->x, this->y - 1); }
void Unit::move_down(){ this->move(this->x, this->y + 1); }
void Unit::move_right(){ this->move(this->x + 1, this->y); }
void Unit::move_left(){ this->move(this->x - 1, this->y); }
int Unit::get_x(){ return this->x; }
int Unit::get_y(){ return this->y; }
bool Unit::is_hit(Unit* u){
  if(u->get_x() == this->get_x() && u->get_y() == this->get_y()) return true;
  return false;
}
bool Unit::is_hit_wall(){
  if(0 >= this->x ||
     0 >= this->y ||
     game->get_width() - 1 <= this->x ||
     game->get_height() -1 <= this->y)
    return true;
  return false;
}
Bullet::Bullet(Game* g,Loc x, Loc y, Color c, Direction vec){
  this->game = g;
  this->x = x;
  this->y = y;
  this->color = c;
  this->vec = vec;
}
void Bullet::next(){
  switch(this->vec){
  case UP:
    this->move_up();
    break;
  case DOWN:
    this->move_down();
    break;
  case RIGHT:
    this->move_right();
    break;
  case LEFT:
    this->move_left();
    break;
  }
}

Fighter::Fighter(Game* g, SocketDescriptor id, Loc x, Loc y, Color c){
  this->game = g;
  this->id = id;
  this->x = x;
  this->y = y;
  this->color = c;
  this->move(this->x, this->y);
}
void Fighter::shoot(Direction d){
  int init_x;
  int init_y;
  complement_direction(init_x, init_y, d);
  game->add_bullet(new Bullet(this->game,
			      this->x + init_x,
			      this->y + init_y,
			      BLACK,
			      d));
}
bool Fighter::is_hit_wall(Direction d){
  int x;
  int y;
  complement_direction(x, y, d);
  if(0 >= this->x + x||
     0 >= this->y + y||
     this->game->get_width() - 1 <= this->x + x||
     this->game->get_height() -1 <= this->y + y)
    return true;
  return false;
}
void Fighter::complement_direction(int& x, int& y, Direction d){
  switch(d){
  case UP:
    x = 0; y = -1;
    break;
  case DOWN:
    x = 0; y = 1;
    break;
  case RIGHT:
    x = 1; y = 0;
    break;
  case LEFT:
    x = -1; y = 0;
    break;
  }
}
Color Fighter::get_color(){
  return this->color;
}


