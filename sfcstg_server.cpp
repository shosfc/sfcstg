#include "sfcstg.h"
#include "sfcstg_server.h"
#include <iostream>

int main(int argc, char* argv[]){
  if(argc != 2){
    printf("Usage: %s <PORT>\n", argv[0]);
    return 0;
  }
  printf("sleep: %d   wait_sec:%d   wait_usec: %d\n",
	 SERVER_USLEEP, SERVER_WAIT_SEC, SERVER_WAIT_USEC);
  ServerNetworkManager manager(atoi(argv[1]));
  Game game(WIDTH, HEIGHT);
  Map map;
  Map endmap = game.create_endmap();
  Color n = game.get_available_color();
  while(true){
    game.create_map_data(&map);
    SocketDescriptor new_client = manager.listen_to_join();
    if( new_client != -1) {
      std::cout << "Join a new clinet: " << new_client << std::endl;
      Color color = game.add_new_client(new_client);
      manager.send_data(&color);
    }
    manager.send_data(&map);
    InstructionsWithId insts;
    manager.recv_instruction(&insts);
    if(insts.size() != 0) game.apply_instructions(&insts);
    std::vector<SocketDescriptor> sdlist = game.check_hit();
    manager.send_game_over(sdlist, &endmap);
    game.refresh();
    usleep(SERVER_USLEEP);
  }
}
