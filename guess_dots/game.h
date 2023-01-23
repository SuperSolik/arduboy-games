#pragma once

#include <Arduboy2.h>

#include "bitmaps.h"
#include "content.h"

enum class GameState : uint8_t {
  TITLE_SCREEN,
  DIFF_CHOICE,
  LEVEL_INIT,
  LEVEL_PLAY,
  LEVEL_COMPLETED,
};

struct SaveData {
  uint8_t player_pos;
  uint16_t overall_score;
  uint16_t levels_solved[lvl::DIFFICULTIES_CNT];  // counter for each difficulty
  uint32_t level_in_progress;

  constexpr static SaveData empty_save_data() {
    return {
      .player_pos = 0, 
      .overall_score = 0, 
      .levels_solved = {
        0, 0, 0
      },
      .level_in_progress = lvl::NULL_LEVEL
    };
  }
};

// constexpr SaveData empty_save_data = {
//   .player_pos = 0, 
//   .overall_score = 0, 
//   .levels_solved = {
//     0, 0, 0
//   },
//   .level_in_progress = lvl::NULL_LEVEL
// };

class Game {
  static constexpr uint8_t DEFAULT_FPS = 24;
  static constexpr uint8_t DOT_RADIUS = 2;
  static constexpr uint8_t EEPROM_START_C1 = EEPROM_STORAGE_SPACE_START;
  static constexpr uint8_t EEPROM_START_C2 =  EEPROM_START_C1 + 1;
  static constexpr uint8_t EEPROM_START_C3 =  EEPROM_START_C1 + 2;
  static constexpr uint8_t EEPROM_SAVE_DATA =  EEPROM_START_C1 + 3;

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

    switch (this->state) {
      case GameState::TITLE_SCREEN:
        this->title_screen();
        break;
      case GameState::DIFF_CHOICE:
        this->choose_difficulty();
        break;
      case GameState::LEVEL_INIT:
        this->set_level();
        break;
      case GameState::LEVEL_PLAY:
        this->play_level();
        break;
      case GameState::LEVEL_COMPLETED:
        this->completed_level_screen();
        break;
      default:
        break;
    }
  }

 private:
  void load_save_from_eeprom() {
    uint8_t c1 = EEPROM.read(EEPROM_START_C1);
    uint8_t c2 = EEPROM.read(EEPROM_START_C2);
    uint8_t c3 = EEPROM.read(EEPROM_START_C3);

    if (c1 != 'D' || c2 != '0' || c3 != 'T') { 
      EEPROM.update(EEPROM_START_C1, 'D');
      EEPROM.update(EEPROM_START_C2, '0');
      EEPROM.update(EEPROM_START_C3, 'T');
      this->save_data = SaveData::empty_save_data();
      EEPROM.put(EEPROM_SAVE_DATA, this->save_data);
    } else {
      EEPROM.get(EEPROM_SAVE_DATA, this->save_data);
    }
  }

  void save_to_eeprom() {
    EEPROM.put(EEPROM_SAVE_DATA, this->save_data);
  }

  void title_screen() {
    static bool flash_press_a_to_play = true;

    this->arduboy.clear();

    if (arduboy.everyXFrames(2 * this->fps / 3)) {
      flash_press_a_to_play = !flash_press_a_to_play;
    }

    this->arduboy.drawCompressed(35, 10, DOTS_LOGO);

    if (flash_press_a_to_play) this->arduboy.drawCompressed(35, 32 + 20, PRESS_A_TO_PLAY);

    if (this->arduboy.justPressed(A_BUTTON | B_BUTTON)) {
      this->state = GameState::DIFF_CHOICE;
    }

    this->arduboy.display();
  }

  void choose_difficulty() {
    if (this->chosen_difficulty == lvl::Difficulty::NONE) {
      this->chosen_difficulty = lvl::Difficulty::EASY;
    }

    this->arduboy.clear();

    this->arduboy.print(F("CHOOSE DIFFICULTY:\n\n"));

    this->arduboy.print(F("  EASY (2x2)\n\n"));
    this->arduboy.print(F("  MEDIUM (3x3)\n\n"));
    this->arduboy.print(F("  HARD (4x4)\n\n"));

    uint8_t diff_idx = static_cast<uint8_t>(this->chosen_difficulty);

    this->arduboy.drawChar(4, 16 + diff_idx * 16, '*', WHITE, BLACK, 1);

    if (this->arduboy.justPressed(UP_BUTTON) && this->chosen_difficulty > lvl::Difficulty::EASY) {
      this->chosen_difficulty = static_cast<lvl::Difficulty>(diff_idx - 1);
    }

    if (this->arduboy.justPressed(DOWN_BUTTON) && this->chosen_difficulty < lvl::Difficulty::HARD) {
      this->chosen_difficulty = static_cast<lvl::Difficulty>(diff_idx + 1);
    }

    if (this->arduboy.justPressed(A_BUTTON)) {
      this->state = GameState::LEVEL_INIT;
    } else if (this->arduboy.justPressed(B_BUTTON)) {
      this->state = GameState::TITLE_SCREEN;
    }

    this->arduboy.display();
  }

  void set_level() {
    if (this->chosen_difficulty == lvl::Difficulty::NONE) {
      this->state = GameState::DIFF_CHOICE;
      return;
    }

    this->lm_idx = static_cast<uint8_t>(this->chosen_difficulty);

    this->level = random(1, lvl::lm[lm_idx].lvl_limit);
    this->player_pos = 0;

    if (lvl::lm[lm_idx].use_offsets) {
      offset_x = -64 + lvl::lm[lm_idx].dots[lvl::lm[lm_idx].dots_cnt - 1].x;
      offset_y = -32 + lvl::lm[lm_idx].dots[lvl::lm[lm_idx].dots_cnt - 1].y;
    }

    this->state = GameState::LEVEL_PLAY;
    this->score_saved = false;
  }

  void completed_level_screen() {
    this->arduboy.clear();

    this->save_data.levels_solved[lm_idx] += 1;
    this->save_data.overall_score += lvl::lm[lm_idx].score_value;

    this->arduboy.print(F("LEVEL\n"));
    this->arduboy.print(F("COMPLETED!\n\n"));

    this->arduboy.print(F("SCORE:\n"));
    this->arduboy.print(this->save_data.overall_score);

    if (!this->score_saved) {
      this->save_to_eeprom();
      this->score_saved = true;
    }

    new_offset_x = -64 + lvl::lm[lm_idx].dots[0].x;
    new_offset_y = -32 + lvl::lm[lm_idx].dots[0].y;

    this->render_level();

    this->arduboy.display();
  }

  void play_level() {
    this->player_input();

    if (this->check_win()) {
      this->arduboy.digitalWriteRGB(GREEN_LED, RGB_ON);
      this->state = GameState::LEVEL_COMPLETED;
    }

    this->arduboy.clear();
    this->render_level();
    this->arduboy.display();
  }

  void reset() {
    this->arduboy.digitalWriteRGB(RED_LED, RGB_OFF);
    this->arduboy.digitalWriteRGB(GREEN_LED, RGB_OFF);

    this->state = GameState::TITLE_SCREEN;
    this->chosen_difficulty = lvl::Difficulty::NONE;
    this->load_save_from_eeprom();
    this->score_saved = false;
  }

  uint8_t count_set_bits(uint32_t value) {
    uint8_t count = 0;
    while (value) {
      count += value & 1;
      value >>= 1;
    }
    return count;
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

  void render_level() {
    make_graduate_offset();

    for (uint8_t i = 0; i < lvl::lm[lm_idx].dots_cnt; i++) {
      if ((this->solution & ((uint32_t)1 << i)) >> i == 1) {
        this->arduboy.fillCircle(
            lvl::lm[lm_idx].dots[i].x - offset_x,
            lvl::lm[lm_idx].dots[i].y - offset_y,
            DOT_RADIUS);
      } else {
        this->arduboy.drawCircle(
            lvl::lm[lm_idx].dots[i].x - offset_x,
            lvl::lm[lm_idx].dots[i].y - offset_y,
            DOT_RADIUS);
      }
    }

    uint8_t tile_size = lvl::lm[lm_idx].tile_size, w, h;

    for (uint8_t i = 0; i < tile_size - 1; i++) {
      for (uint8_t j = 0; j < tile_size; j++) {
        w = lvl::lm[lm_idx].dots[i + 1 + j * tile_size].x - lvl::lm[lm_idx].dots[i + j * tile_size].x - 2 * (DOT_RADIUS + 4);
        h = lvl::lm[lm_idx].dots[(i + 1) * tile_size].y - lvl::lm[lm_idx].dots[i * tile_size].y - 2 * (DOT_RADIUS + 4);

        this->arduboy.drawFastHLine(
            lvl::lm[lm_idx].dots[i + j * tile_size].x + DOT_RADIUS + 4 - offset_x,
            lvl::lm[lm_idx].dots[i + j * tile_size].y - offset_y,
            w,
            WHITE);

        this->arduboy.drawFastVLine(
            lvl::lm[lm_idx].dots[j + i * tile_size].x - offset_x,
            lvl::lm[lm_idx].dots[j + i * tile_size].y + DOT_RADIUS + 4 - offset_y,
            h,
            WHITE);
      }
    }

    this->arduboy.drawRoundRect(
        lvl::lm[lm_idx].dots[this->player_pos].x - offset_x - 4,
        lvl::lm[lm_idx].dots[this->player_pos].y - offset_y - 4,
        9,
        9,
        3);

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

  void make_graduate_offset() {
    if (!adjust_offset) return;

    int8_t step_x = new_offset_x < offset_x ? -3 : 3;
    int8_t step_y = new_offset_y < offset_y ? -2 : 2;

    if (level_start) {
      step_x /= 3;
      step_y /= 2;
    }

    if (offset_x != new_offset_x) {
      offset_x += step_x;
    }

    if (offset_y != new_offset_y) {
      offset_y += step_y;
    }

    if (offset_x == new_offset_x && offset_y == new_offset_y) {
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

  lvl::Difficulty chosen_difficulty;

  bool adjust_offset = false;
  bool level_start = true;

  SaveData save_data;
  bool score_saved;

  GameState state;
};