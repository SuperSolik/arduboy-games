#include <Arduboy2.h>
#include "game.h"

Game game(28);

void setup () {
  game.init();
}

void loop () {
  game.run();
}