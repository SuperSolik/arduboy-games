#include <Arduboy2.h>

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

    this->arduboy.print("hello world");

    this->arduboy.display();
  }

 private:
  void reset() {}

  Arduboy2 arduboy;
  uint8_t fps;
};