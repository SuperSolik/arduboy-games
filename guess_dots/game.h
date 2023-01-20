#pragma once

#include <Arduboy2.h>

// #include "point.h"
#include "content.h"


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

    // this->level = random(1, 511);
    this->set_level(1);
  }

  bool check_win() {
    uint32_t cnt_mask = 0;
    for (uint8_t i = 0; i < dots_content::lm[lm_idx].numbers_cnt; i++) {
      cnt_mask = dots_content::lm[lm_idx].cnt_masks[i];
      if (count_set_bits(this->level & cnt_mask) != count_set_bits(this->solution & cnt_mask)) {
        return false;
      }
    }
    return true;
  }

  void set_level(uint8_t index) {
    if (index < 0 || index > 2) return;
    this->lm_idx = index;
  }

  void player_input() {
    if (this->arduboy.justPressed(LEFT_BUTTON) && this->player_pos - 1 >= 0) this->player_pos -= 1;
    if (this->arduboy.justPressed(RIGHT_BUTTON) && this->player_pos + 1 < dots_content::lm[lm_idx].dots_cnt) this->player_pos += 1;
    if (this->arduboy.justPressed(DOWN_BUTTON) && this->player_pos + dots_content::lm[lm_idx].tile_size < dots_content::lm[lm_idx].dots_cnt) this->player_pos += dots_content::lm[lm_idx].tile_size;
    if (this->arduboy.justPressed(UP_BUTTON) && this->player_pos - dots_content::lm[lm_idx].tile_size >= 0) this->player_pos -= dots_content::lm[lm_idx].tile_size;

    if (this->arduboy.justPressed(A_BUTTON)) {
      this->solution = this->solution ^ ((uint32_t)1 << this->player_pos);
    }
  }

  void render() {
    for (uint8_t i = 0; i < dots_content::lm[lm_idx].dots_cnt; i++) {
      if ((this->solution & (1 << i)) >> i == 1) {
        this->arduboy.fillCircle(
          dots_content::lm[lm_idx].dots[i].x,
          dots_content::lm[lm_idx].dots[i].y, 
          DOT_RADIUS
        );
      } else {
        this->arduboy.drawCircle(
          dots_content::lm[lm_idx].dots[i].x,
          dots_content::lm[lm_idx].dots[i].y, 
          DOT_RADIUS
        );
      }
    }

    this->arduboy.drawRoundRect(
      dots_content::lm[lm_idx].dots[this->player_pos].x - 4, 
      dots_content::lm[lm_idx].dots[this->player_pos].y - 4, 
      9, 
      9, 
      3
    );

    for (uint8_t i = 0; i < dots_content::lm[lm_idx].numbers_cnt; i++) {
      this->arduboy.drawChar(
          dots_content::lm[lm_idx].numbers[i].x - 2,
          dots_content::lm[lm_idx].numbers[i].y - 3,
          // '0',
          count_set_bits(this->level & dots_content::lm[lm_idx].cnt_masks[i]) + 48,
          WHITE,
          BLACK,
          1);
    }
  }

  // PointU8 dots[9] = {
  //     {.x = WIDTH / 2 - TILE_SIZE, .y = HEIGHT / 2 - TILE_SIZE},
  //     {.x = WIDTH / 2, .y = HEIGHT / 2 - TILE_SIZE},
  //     {.x = WIDTH / 2 + TILE_SIZE, .y = HEIGHT / 2 - TILE_SIZE},
  //     {.x = WIDTH / 2 - TILE_SIZE, .y = HEIGHT / 2},
  //     {.x = WIDTH / 2, .y = HEIGHT / 2},
  //     {.x = WIDTH / 2 + TILE_SIZE, .y = HEIGHT / 2},
  //     {.x = WIDTH / 2 - TILE_SIZE, .y = HEIGHT / 2 + TILE_SIZE},
  //     {.x = WIDTH / 2, .y = HEIGHT / 2 + TILE_SIZE},
  //     {.x = WIDTH / 2 + TILE_SIZE, .y = HEIGHT / 2 + TILE_SIZE},
  // };

  // PointU8 numbers[4] = {
  //     {.x = WIDTH / 2 - TILE_SIZE / 2, .y = HEIGHT / 2 - TILE_SIZE / 2},
  //     {.x = WIDTH / 2 + TILE_SIZE / 2, .y = HEIGHT / 2 - TILE_SIZE / 2},
  //     {.x = WIDTH / 2 - TILE_SIZE / 2, .y = HEIGHT / 2 + TILE_SIZE / 2},
  //     {.x = WIDTH / 2 + TILE_SIZE / 2, .y = HEIGHT / 2 + TILE_SIZE / 2},
  // };

  // uint32_t count_masks[4] = {
  //     (u32)1 << 0 | (u32)1 << 1 | (u32)1 << 3 | (u32)1 << 4,
  //     (u32)1 << 1 | (u32)1 << 2 | (u32)1 << 4 | (u32)1 << 5,
  //     (u32)1 << 3 | (u32)1 << 4 | (u32)1 << 6 | (u32)1 << 7,
  //     (u32)1 << 4 | (u32)1 << 5 | (u32)1 << 7 | (u32)1 << 8,
  // };

  Arduboy2 arduboy;
  uint8_t fps;

  uint8_t lm_idx = 0;
  uint32_t level = 13466;
  uint32_t solution = 0;
  uint8_t player_pos = 0;
};