#include <Arduboy2.h>

#include "game.h"

Arduboy2 a;

Game<a.width(), a.height()> game(a, 24);

void setup() {
  game.init();
}

void loop () {
  game.run();
}