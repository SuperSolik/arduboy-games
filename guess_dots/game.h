#pragma once

#include <Arduboy2.h>

template <typename T>
struct Point2D {
  T x;
  T y;
};

using PointU8 = Point2D<uint8_t>;

class Game {
  static constexpr uint8_t DEFAULT_FPS = 24;
  static constexpr uint8_t DOT_RADIUS = 2;
  static constexpr uint8_t TILE_SIZE = 20;

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
    this->player_input();

    if(this->check_win()){
      this->arduboy.digitalWriteRGB(GREEN_LED, RGB_ON);
    }

    this->arduboy.clear();
    this->render();
    this->arduboy.display();
  }

 private:
  uint8_t count_set_bits(uint16_t value) {
    uint8_t count = 0;
    while (value) {
      count += value & 1;
      value >>= 1;
    }
    return count;
  }

  void reset() {
    this->arduboy.digitalWriteRGB(RED_LED, RGB_OFF);
    this->arduboy.digitalWriteRGB(GREEN_LED, RGB_OFF);

    this->level = random(1, 511);
  }

  bool check_win() {
    for (uint8_t i = 0; i < 4; i++) {
      if (count_set_bits(this->level & this->count_masks[i]) != count_set_bits(this->solution & this->count_masks[i])) {
        return false;
      }
    }
    return true;
  }

  void player_input() {
    if (this->arduboy.justPressed(LEFT_BUTTON) && this->player_pos - 1 >= 0) this->player_pos -= 1;
    if (this->arduboy.justPressed(RIGHT_BUTTON) && this->player_pos + 1 < 9) this->player_pos += 1;
    if (this->arduboy.justPressed(DOWN_BUTTON) && this->player_pos + 3 < 9) this->player_pos += 3;
    if (this->arduboy.justPressed(UP_BUTTON) && this->player_pos - 3 >= 0) this->player_pos -= 3;

    if (this->arduboy.justPressed(A_BUTTON)) {
      this->solution = this->solution ^ ((uint16_t)1 << this->player_pos);
    }
  }

  void render() {
    for (uint8_t i = 0; i < 9; i++) {
      if ((this->solution & (1 << i)) >> i == 1) {
        this->arduboy.fillCircle(this->dots[i].x, this->dots[i].y, DOT_RADIUS);
      } else {
        this->arduboy.drawCircle(this->dots[i].x, this->dots[i].y, DOT_RADIUS);
      }
    }

    this->arduboy.drawRoundRect(this->dots[this->player_pos].x - 4, this->dots[this->player_pos].y - 4, 9, 9, 3);

    uint8_t size = 2;

    for (uint8_t i = 0; i < 4; i++) {
      this->arduboy.drawChar(
          this->numbers[i].x - size,
          this->numbers[i].y - size,
          count_set_bits(this->level & this->count_masks[i]) + 48,
          WHITE,
          BLACK,
          1);
    }
  }

  PointU8 dots[9] = {
      {.x = WIDTH / 2 - TILE_SIZE, .y = HEIGHT / 2 - TILE_SIZE},
      {.x = WIDTH / 2, .y = HEIGHT / 2 - TILE_SIZE},
      {.x = WIDTH / 2 + TILE_SIZE, .y = HEIGHT / 2 - TILE_SIZE},
      {.x = WIDTH / 2 - TILE_SIZE, .y = HEIGHT / 2},
      {.x = WIDTH / 2, .y = HEIGHT / 2},
      {.x = WIDTH / 2 + TILE_SIZE, .y = HEIGHT / 2},
      {.x = WIDTH / 2 - TILE_SIZE, .y = HEIGHT / 2 + TILE_SIZE},
      {.x = WIDTH / 2, .y = HEIGHT / 2 + TILE_SIZE},
      {.x = WIDTH / 2 + TILE_SIZE, .y = HEIGHT / 2 + TILE_SIZE},
  };

  PointU8 numbers[4] = {
      {.x = WIDTH / 2 - TILE_SIZE / 2, .y = HEIGHT / 2 - TILE_SIZE / 2},
      {.x = WIDTH / 2 + TILE_SIZE / 2, .y = HEIGHT / 2 - TILE_SIZE / 2},
      {.x = WIDTH / 2 - TILE_SIZE / 2, .y = HEIGHT / 2 + TILE_SIZE / 2},
      {.x = WIDTH / 2 + TILE_SIZE / 2, .y = HEIGHT / 2 + TILE_SIZE / 2},
  };

  uint16_t count_masks[4] = {
      (u8)1 << 0 | (u8)1 << 1 | (u8)1 << 3 | (u8)1 << 4,
      (u8)1 << 1 | (u8)1 << 2 | (u8)1 << 4 | (u8)1 << 5,
      (u8)1 << 3 | (u8)1 << 4 | (u8)1 << 6 | (u8)1 << 7,
      (u8)1 << 4 | (u8)1 << 5 | (u8)1 << 7 | (u8)1 << 8,
  };

  Arduboy2 arduboy;
  uint8_t fps;

  uint16_t level = 0;
  uint16_t solution = 0;
  uint8_t player_pos = 0;
};