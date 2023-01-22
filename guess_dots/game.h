#pragma once

#include <Arduboy2.h>

// #include "point.h"
#include "content.h"


class Game {
  static constexpr uint8_t DEFAULT_FPS = 24;
  static constexpr uint8_t DOT_RADIUS = 2;

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
  uint8_t count_set_bits(uint32_t value) {
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

    this->set_level(static_cast<uint8_t>(lvl::DifficultyLevel::HARD));
  }

  bool check_win() {
    uint32_t cnt_mask = 0;
    for (uint8_t i = 0; i < lvl::lm[lm_idx].numbers_cnt; i++) {
      cnt_mask = lvl::lm[lm_idx].cnt_masks[i];
      if (count_set_bits(this->level & cnt_mask) != count_set_bits(this->solution & cnt_mask)) {
        return false;
      }
    }
    return true;
  }

  void set_level(uint8_t index) {
    if (index < 0 || index > 2) return;
    this->lm_idx = index;
    this->level = random(1, 2147483647);

    if (lvl::lm[lm_idx].use_offsets) {
      offset_x = -64 + lvl::lm[lm_idx].dots[lvl::lm[lm_idx].dots_cnt - 1].x;
      offset_y = -32 + lvl::lm[lm_idx].dots[lvl::lm[lm_idx].dots_cnt - 1].y;
    }
  }

  void player_input() {
    if (this->arduboy.justPressed(LEFT_BUTTON) && this->player_pos - 1 >= 0) this->player_pos -= 1;
    if (this->arduboy.justPressed(RIGHT_BUTTON) && this->player_pos + 1 < lvl::lm[lm_idx].dots_cnt) this->player_pos += 1;
    if (this->arduboy.justPressed(DOWN_BUTTON) && this->player_pos + lvl::lm[lm_idx].tile_size < lvl::lm[lm_idx].dots_cnt) this->player_pos += lvl::lm[lm_idx].tile_size;
    if (this->arduboy.justPressed(UP_BUTTON) && this->player_pos - lvl::lm[lm_idx].tile_size >= 0) this->player_pos -= lvl::lm[lm_idx].tile_size;

    if (this->arduboy.justPressed(A_BUTTON)) {
      this->solution = this->solution ^ ((uint32_t)1 << this->player_pos);
    }

    new_offset_x = -64 + lvl::lm[lm_idx].dots[this->player_pos].x;
    new_offset_y = -32 + lvl::lm[lm_idx].dots[this->player_pos].y;
    adjust_offset = lvl::lm[lm_idx].use_offsets;
  }

  void render() {
    make_graduate_offset();

    for (uint8_t i = 0; i < lvl::lm[lm_idx].dots_cnt; i++) {
      if ((this->solution & ((uint32_t)1 << i)) >> i == 1) {
        this->arduboy.fillCircle(
          lvl::lm[lm_idx].dots[i].x - offset_x,
          lvl::lm[lm_idx].dots[i].y - offset_y,
          DOT_RADIUS
        );
      } else {
        this->arduboy.drawCircle(
          lvl::lm[lm_idx].dots[i].x - offset_x,
          lvl::lm[lm_idx].dots[i].y - offset_y, 
          DOT_RADIUS
        );
      }
    }

    uint8_t tile_size = lvl::lm[lm_idx].tile_size, w, h;


    for (uint8_t i = 0; i < tile_size - 1; i++) {
      for (uint8_t j = 0; j < tile_size; j ++) {
        w = lvl::lm[lm_idx].dots[i + 1 + j * tile_size].x - lvl::lm[lm_idx].dots[i + j * tile_size].x - 2 * (DOT_RADIUS + 4);
        h = lvl::lm[lm_idx].dots[(i + 1) * tile_size].y - lvl::lm[lm_idx].dots[i * tile_size].y - 2 * (DOT_RADIUS + 4);

        this->arduboy.drawFastHLine(
          lvl::lm[lm_idx].dots[i + j * tile_size].x + DOT_RADIUS + 4 - offset_x,
          lvl::lm[lm_idx].dots[i + j * tile_size].y - offset_y,
          w,
          WHITE
        );

        this->arduboy.drawFastVLine(
          lvl::lm[lm_idx].dots[j + i * tile_size].x - offset_x,
          lvl::lm[lm_idx].dots[j + i * tile_size].y + DOT_RADIUS + 4 - offset_y,
          h,
          WHITE
        );
      }
    }

    this->arduboy.drawRoundRect(
      lvl::lm[lm_idx].dots[this->player_pos].x - offset_x - 4, 
      lvl::lm[lm_idx].dots[this->player_pos].y - offset_y - 4, 
      9, 
      9, 
      3
    );

    for (uint8_t i = 0; i < lvl::lm[lm_idx].numbers_cnt; i++) {
      this->arduboy.drawChar(
          lvl::lm[lm_idx].numbers[i].x - offset_x - 2,
          lvl::lm[lm_idx].numbers[i].y - offset_y - 3,
          // '0',
          count_set_bits(this->level & lvl::lm[lm_idx].cnt_masks[i]) + 48,
          WHITE,
          BLACK,
          1);
    }
  }

  void make_graduate_offset(){
    if (!adjust_offset) return;

    int8_t step_x = new_offset_x < offset_x ? - 3 : 3;
    int8_t step_y = new_offset_y < offset_y ? - 2 : 2; 

    if (level_start) {
      step_x /= 3;
      step_y /= 2;
    }

    if(offset_x != new_offset_x) {
      offset_x += step_x;
    }

    if(offset_y != new_offset_y) {
      offset_y += step_y;
    }

    if (offset_x == new_offset_x && offset_y == new_offset_y){
      adjust_offset = false;
      level_start = false;
    }
  }

  Arduboy2 arduboy;
  uint8_t fps;

  uint8_t lm_idx = 0;
  uint32_t level = 13466;
  uint32_t solution = 0;
  uint8_t player_pos = 0;

  int16_t offset_x = 0;
  int16_t offset_y = 0;

  int16_t new_offset_x = 0;
  int16_t new_offset_y = 0; 

  bool adjust_offset = false;
  bool level_start = true;
};