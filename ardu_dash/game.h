#include <Arduboy2.h>
#include "bitmaps.h"

class Game {
  static constexpr uint8_t DEFAULT_FPS = 24;

 public:
  Game(uint8_t fps = DEFAULT_FPS) {
    this->fps = fps;
  }

  void init() {
    this->arduboy.begin();
    this->arduboy.setFrameRate(this->fps);
    this->arduboy.initRandomSeed();
    this->reset();
  }

  void run() {
    if (!this->arduboy.nextFrame()) return;
    this->arduboy.clear();
    this->arduboy.pollButtons();

    this->animation_idx = (this->animation_idx + 1) % 6;
    Sprites::drawSelfMasked(64 - 8, 32 - 8, cube, this->animation_idx);

    this->arduboy.display();
  }

 private:
  void reset() {
    this->animation_idx = 0;
  }

  Arduboy2 arduboy;
  uint8_t fps;
  uint8_t animation_idx;
};