#include "sfcstg_client.h"
#include <iostream>
#include <unistd.h>
#include <string>

int main(int argc, char* argv[]){
  if(argc != 3){
    printf("Usage: %s <HOST> <PORT>\n", argv[0]);
    return 0;
  }
  ClientNetworkManager manager(argv[1], atoi(argv[2]));
  Color c = manager.read_user_data();
  Client client(WIDTH, HEIGHT, &manager);
  Map map;
  while(true){
    manager.get_readable(&map);
    if(client.check_game_over(&map)) {
      manager.close_connection();
      return 0;
    }
    client.draw_map(&map);
    Instruction inst = client.key_handler();
    printf("press 'I', 'J', 'K', 'L' to move\n");
    printf("press 'W', 'A', 'S', 'D' to shoot\n");
    if(inst.object != NULL_OBJECT){
      manager.write_sock((void *)&inst);
    }
    usleep(CLIENT_USLEEP);
  }
}
