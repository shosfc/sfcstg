#include "sfcstg_server.h"
#include <iostream>
#include <vector>
#include <algorithm>

Game::Game(Size width, Size height){
  this->width = width;
  this->height = height;
}
Game::~Game(){
  //delete this->player;
}
void Game::refresh(){
  //Fighters garbage_player;
  Bullets garbage_bullet;
  Bullets::iterator i = this->bullets.begin();
  while( i != this->bullets.end() ) {
    if(!((*i)->is_hit_wall())){
      (*i)->next();
    } else {
      garbage_bullet.push_back(*i);
    }
    i++;
  }
  Bullets::iterator j = garbage_bullet.begin();
  while( j != garbage_bullet.end() ) {
    bullets.erase(std::remove(bullets.begin(), bullets.end(), *j),
		  bullets.end());
    j++;
  }
  //this->check_hit(garbage_player, garbage_bullet);
}

std::vector<SocketDescriptor> Game::check_hit()
{
  std::vector<SocketDescriptor> garbage_player;
  Bullets garbage_bullet;
  Bullets::iterator b = this->bullets.begin();
  Fighters::iterator f = this->players.begin();
  //printf("players size: %d\n" , players.size());
  while( f != this->players.end() ) {
    //    printf("%d:%d\n", (*f).second->get_x(), (*f).second->get_y());
    while( b != this->bullets.end() ) {
      if( (*f).second->is_hit((*b)) ){
	garbage_player.push_back((*f).first);
	garbage_bullet.push_back(*b);
      }
      b++;
    }
    f++;
    b = this->bullets.begin();
  }
  std::unique(garbage_player.begin(), garbage_player.end());
  std::unique(garbage_bullet.begin(), garbage_bullet.end());
  Bullets::iterator j = garbage_bullet.begin();
  while( j != garbage_bullet.end() ) {
    bullets.erase(std::remove(bullets.begin(), bullets.end(), *j),
		  bullets.end());
    j++;
  }
  std::vector<SocketDescriptor>::iterator k = garbage_player.begin();
  while(k != garbage_player.end() ) {
    //players.erase(std::remove(players.begin(), players.end(), *j),
    //		  players.end());
    players.erase(*k);
    //send_game_over(*k);
    k++;
  }
  return garbage_player;
}

Color Game::add_new_client(SocketDescriptor socket){
  Location loc = this->get_available_location();
  Color color = this->get_available_color();
  Fighter* f = new Fighter(this, socket, loc.x, loc.y, color);
  this->add_fighter(socket, f);
  return color;
}
void Game::add_fighter(SocketDescriptor id, Fighter* f){
  //this->players.push_back(f);
  this->players.insert(std::map<SocketDescriptor, Fighter*>::value_type(id, f));
}
void Game::add_bullet(Bullet* b){
  this->bullets.push_back(b);
}

Map Game::create_endmap(){
  Map endmap;
  Cell null_cell = { NULL_COLOR };
  for(int i = 0; i < this->width; i++)
    for(int j = 0; j < this->height; j++)
      endmap.cell[i][j] = null_cell;
  return endmap;
}

Map* Game::create_map_data(Map* map){
  struct Cell white_cell = { WHITE };
  struct Cell wall_cell = { BLACK };
  struct Cell bullet_cell = { BLACK };
  for(int i = 0; i < this->width; i++)
    for(int j = 0; j < this->height; j++)
      map->cell[i][j] = white_cell;
  for(int l = 0; l < this->width; l++)
    map->cell[l][0] = wall_cell;
  for(int l = 0; l < this->width; l++)
    map->cell[l][this->height - 1] = wall_cell;
  for(int l = 0; l < this->height; l++)
    map->cell[0][l] = wall_cell;
  for(int l = 0; l < this->height; l++)
    map->cell[this->width - 1][l] = wall_cell;
  Bullets::iterator b = this->bullets.begin();
  while( b != this->bullets.end() ) {
    map->cell[(*b)->get_x()][(*b)->get_y()] = bullet_cell;
    b++;
  }
  Fighters::iterator f = this->players.begin();
  while( f != this->players.end() ) {
    map->cell[(*f).second->get_x()][(*f).second->get_y()].color 
      = (*f).second->get_color();
    f++;
  }
  return map;
}

Color Game::get_available_color(){
  std::vector<Color> not_used;
  not_used.push_back(BLUE);
  not_used.push_back(GREEN);
  not_used.push_back(CYAN);
  not_used.push_back(RED);
  not_used.push_back(MAGENTA);
  not_used.push_back(YELLOW);
  //printf("size of not_used: %d", not_used.size());
  Fighters::iterator f = this->players.begin();
  while( f != this->players.end() ) {  
    not_used.erase(std::remove(not_used.begin(), not_used.end(),
			       (*f).second->get_color()),
		   not_used.end());
    f++;
  }
  //printf("size of not_used: %d", not_used.size());
  //printf("not[0]: %d\n", not_used[0]);
  return not_used[0];
}

Location Game::get_available_location(){
  srand((unsigned) time(NULL));
  Location loc;
  bool booking;
  do{
    booking = false;
    loc.x = (rand() % ( this->width - 1)) + 1;
    loc.y = (rand() % ( this->height - 1)) + 1;
    Fighters::iterator f = this->players.begin();
    while( f != this->players.end() ) {  
      if( (*f).second->get_x() == loc.x &&
	  (*f).second->get_y() == loc.y )
	booking = true;
      f++;
    }
    Bullets::iterator b = this->bullets.begin();
    while( b != this->bullets.end() ) {
      if( (*b)->get_x() == loc.x &&
	  (*b)->get_y() == loc.y )
	booking = true;
      b++;
    }
  }while(booking);
  return loc;
}

void Game::apply_instructions(InstructionsWithId* insts){
  InstructionsWithId::iterator i = insts->begin();
  while( i != insts->end() ) {
    switch((*i).object){
    case FIGHTER:
      switch((*i).direction){
      case UP:
	if(!players[(*i).id]->is_hit_wall(UP))
	  players[(*i).id]->move_up();
	break;
      case DOWN:
	if(!players[(*i).id]->is_hit_wall(DOWN))
	  players[(*i).id]->move_down();
	break;
      case LEFT:
	if(!players[(*i).id]->is_hit_wall(LEFT))
	  players[(*i).id]->move_left();
	break;
      case RIGHT:
	if(!players[(*i).id]->is_hit_wall(RIGHT))
	  players[(*i).id]->move_right();
	break;
      }
      break;
    case BULLET:
      switch((*i).direction){
      case UP:
	players[(*i).id]->shoot(UP);
	break;
      case DOWN:
	players[(*i).id]->shoot(DOWN);
	break;
      case LEFT:
	players[(*i).id]->shoot(LEFT);
	break;
      case RIGHT:
	players[(*i).id]->shoot(RIGHT);
	break;
      }
    }
    i++;
  }
}

Size Game::get_width(){ return this->width; }
Size Game::get_height(){ return this->height; }
