#include <Arduboy2.h>

#include "game.h"

Arduboy2 a;

Game game(a);

void setup() {
  game.init();
}

void loop () {
  game.run();
}