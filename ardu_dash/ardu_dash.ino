#include <Arduboy2.h>
#include "game.h"

Game game;

void setup () {
  game.init();
}

void loop () {
  game.run();
}