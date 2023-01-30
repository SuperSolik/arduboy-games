#include <Arduboy2.h>
#include "bitmaps.h"
#include "point.h"

enum class ObstacleType : uint8_t {
  PLATFORM = 0,
  BLOCK = 1, 
  SPIKE = 2,
  COUNT = 3,
};


class Game {
  static constexpr uint8_t DEFAULT_FPS = 24;
  static constexpr uint8_t STARS_CNT = 55;
  static constexpr uint8_t STARS_SPEED = 1;
  static constexpr uint8_t GROUND_Y = 1;
  static constexpr uint8_t OBJECT_SIZE = 16;
  static constexpr uint8_t PLAYER_X = 32 - OBJECT_SIZE;
  static constexpr uint8_t JUMP_LENGTH = 15;

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
    
    this->arduboy.pollButtons();

    // uint8_t btn_result = 0;

    if (this->arduboy.anyPressed(A_BUTTON | B_BUTTON | UP_BUTTON)) {
      if (!this->is_jumping) this->is_jumping = true;
    }

    this->arduboy.clear();
    this->update_and_draw_backgroud();

    this->arduboy.drawFastHLine(0, this->arduboy.height() - GROUND_Y, this->arduboy.width(), WHITE);

    this->draw_player();
    this->arduboy.display();
  }

  void draw_player() {
    uint8_t base_y = this->arduboy.height() - GROUND_Y - OBJECT_SIZE, offset = 0;
    uint8_t anim_idx = 0;

    if (is_jumping) {
      offset = JUMP_Y[jump_idx++];
      if (jump_idx >= JUMP_LENGTH) {
        this->jump_idx = 0;
        this->is_jumping = false;
      }
      anim_idx = this->animation_idx;
      this->animation_idx = (this->animation_idx + 1) % 6;
    }

    Sprites::drawSelfMasked(PLAYER_X, base_y - offset, cube, anim_idx);

    for(uint8_t i = 0; i < static_cast<uint8_t>(ObstacleType::COUNT); i++) {
      Sprites::drawSelfMasked(PLAYER_X + OBJECT_SIZE * (i + 1) + OBJECT_SIZE, base_y, obstacles, i);
    }
  }

  void update_and_draw_backgroud() {
    for (uint8_t i = 0; i < STARS_CNT; i++) {
      if (stars[i].x < 0) {
        stars[i].x = this->arduboy.width();
        stars[i].y = random(0, this->arduboy.height());
      }
      if (i % 2 == 0) {
        stars[i].x -= STARS_SPEED;
      } else if (i % 3 == 0) {
        stars[i].x -= STARS_SPEED * 2;
      } else {
        stars[i].x -= STARS_SPEED * 3;
      }

      this->arduboy.drawPixel(stars[i].x, stars[i].y, WHITE);
    }
  }

 private:
  void reset() {
    this->animation_idx = 0;
    for(auto& s : stars) {
      s.x = random(0, this->arduboy.width());
      s.y = random(0, this->arduboy.height());
    }
  }

  Arduboy2 arduboy;
  uint8_t fps;
  uint8_t animation_idx;

  bool is_jumping = false;
  uint8_t jump_idx = 0;

  PointS16 stars[STARS_CNT];
  uint8_t JUMP_Y[JUMP_LENGTH] = {
    1, 6, 12, 18, 21, 27, 28, 30, 28, 27, 21, 18, 12, 6, 1 
  };
};