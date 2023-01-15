#include <Arduboy2.h>

#include "bitmaps.h"

#define NOT_OPEN_TILE 11
#define MINE_TILE 12
#define FLAG_TILE 13

#define MINE_BIT_OFFSET 0
#define FLAGGED_BIT_OFFSET 1
#define REVEALED_BIT_OFFSET 2
#define SPRITED_INDEX_BIT_OFFSET 4
#define IS_MINE_MASK (1 << MINE_BIT_OFFSET)
#define IS_FLAGGED_MASK (1 << FLAGGED_BIT_OFFSET)
#define IS_REVEALED_MASK (1 << REVEALED_BIT_OFFSET)
#define SPRITE_INDEX_MASK (15 << SPRITED_INDEX_BIT_OFFSET)  // bits 4-7 are 1s

#define SET_SPRITE_INDEX(x, index) ((x & ~SPRITE_INDEX_MASK) | (index << 4))
#define SPRITE_INDEX(x) ((x & SPRITE_INDEX_MASK) >> SPRITED_INDEX_BIT_OFFSET)
#define MINE(x) (x | IS_MINE_MASK)
#define REVEAL(x) (x | IS_REVEALED_MASK)
#define TOGGLE_IS_FLAGGED(x) (x ^ IS_FLAGGED_MASK)
#define IS_MINE(x) ((x & IS_MINE_MASK) >> MINE_BIT_OFFSET)
#define IS_FLAGGED(x) ((x & IS_FLAGGED_MASK) >> FLAGGED_BIT_OFFSET)
#define IS_REVEALED(x) ((x & IS_REVEALED_MASK) >> REVEALED_BIT_OFFSET)

enum class GameState : uint8_t {
  START,
  PICK_MINES,
  RUNNING,
  WIN,
  LOSE
};

template <uint8_t W, uint8_t H>
class Game {
 public:
  static constexpr uint8_t MAX_MINES_CNT = 20;
  static constexpr uint8_t DEFAULT_FPS = 24;
  static constexpr uint8_t DEFAULT_MINES_CNT = 14;
  static constexpr uint8_t TILE_SIZE = 8;
  static constexpr uint8_t FIELD_W = W / TILE_SIZE;
  static constexpr uint8_t FIELD_H = H / TILE_SIZE;

  Game(Arduboy2 arduboy, uint8_t fps = DEFAULT_FPS) {
    this->arduboy = arduboy;
    this->fps = fps;
  }

  void init() {
    this->arduboy.begin();
    this->arduboy.setFrameRate(this->fps);
    this->arduboy.initRandomSeed();
    this->title_screen_blink = true;
    this->reset();
  }

  void reset() {
    this->player_x = 0;
    this->player_y = 0;
    this->mines_cnt = DEFAULT_MINES_CNT;
    this->revealed_cnt = 0;
    this->state = GameState::START;
    this->is_first_move = true;

    this->arduboy.digitalWriteRGB(RED_LED, RGB_OFF);
    this->arduboy.digitalWriteRGB(GREEN_LED, RGB_OFF);

    this->init_field();
  }

  void run() {
    if (!this->arduboy.nextFrame()) return;
    this->arduboy.pollButtons();
    this->arduboy.clear();

    switch (this->state) {
      case GameState::START:
        this->title_screen();
        break;
      case GameState::PICK_MINES:
        this->pick_mines();
        break;
      case GameState::RUNNING:
        this->loop();
        break;
      case GameState::WIN:
      case GameState::LOSE:
        this->end_screen();
        break;
    }

    this->arduboy.display();
  }

 private:
  void title_screen() {
    this->reset();
    this->arduboy.drawCompressed(0, 0, CMP_LOGO, WHITE);

    if (this->arduboy.everyXFrames(20)) {
      this->title_screen_blink = !this->title_screen_blink;
    }

    if (this->title_screen_blink) {
      this->arduboy.drawCompressed(
          WIDTH / 2 - 32,
          HEIGHT / 2 + 10,
          CMP_PRESS_A_TO_PLAY,
          WHITE);
    }

    if (arduboy.justPressed(A_BUTTON | B_BUTTON)) {
      this->state = GameState::PICK_MINES;
      this->arduboy.delayShort(150);
    }
  }

  void pick_mines() {
    uint8_t a = this->mines_cnt / 10;
    uint8_t b = this->mines_cnt % 10;

    uint8_t x1 = (FIELD_W / 2 - 1) * TILE_SIZE + TILE_SIZE / 3;
    uint8_t x2 = (FIELD_W / 2 + 1) * TILE_SIZE - TILE_SIZE / 3;
    uint8_t y1 = (FIELD_H / 2) * TILE_SIZE - TILE_SIZE / 2;
    uint8_t y2 = (FIELD_H / 2 + 1) * TILE_SIZE + TILE_SIZE / 2;

    this->arduboy.fillTriangle(
        x1, y1,
        (x2 + x1) / 2, y1 - 6,
        x2, y1,
        WHITE);

    Sprites::drawSelfMasked((FIELD_W / 2 - 1) * TILE_SIZE, (FIELD_H / 2) * TILE_SIZE, minesweeper_sprites, a == 0 ? 10 : a);
    Sprites::drawSelfMasked((FIELD_W / 2) * TILE_SIZE, (FIELD_H / 2) * TILE_SIZE, minesweeper_sprites, b == 0 ? 10 : b);

    this->arduboy.fillTriangle(
        x1, y2,
        (x2 + x1) / 2, y2 + 6,
        x2, y2,
        WHITE);

    this->arduboy.setCursor(x1 - 25, y1 + 2 * TILE_SIZE / 3);
    this->arduboy.print("USE");

    this->arduboy.setCursor(x2 + 10, y1 + 2 * TILE_SIZE / 3);
    this->arduboy.print("MINES");

    if (this->arduboy.justPressed(DOWN_BUTTON | LEFT_BUTTON)) {
      if (this->mines_cnt - 1 > 0) this->mines_cnt -= 1;
    }

    if (this->arduboy.justPressed(UP_BUTTON | RIGHT_BUTTON)) {
      if (this->mines_cnt + 1 <= MAX_MINES_CNT) this->mines_cnt += 1;
    }

    if (this->arduboy.justPressed(A_BUTTON | B_BUTTON)) {
      state = GameState::RUNNING;
    }
  }

  void player_input() {
    if (arduboy.justPressed(DOWN_BUTTON) && (player_y + 1 < FIELD_H)) this->player_y += 1;
    if (arduboy.justPressed(UP_BUTTON) && (player_y - 1 >= 0)) this->player_y -= 1;
    if (arduboy.justPressed(RIGHT_BUTTON) && (player_x + 1 < FIELD_W)) this->player_x += 1;
    if (arduboy.justPressed(LEFT_BUTTON) && (player_x - 1 >= 0)) this->player_x -= 1;

    if (arduboy.justPressed(A_BUTTON)) {
      if (this->is_first_move) {
        this->generate_field();
        this->is_first_move = false;
      }

      if (IS_REVEALED(field[this->player_y][this->player_x])) {
        this->reveal_current_neighbors();
      } else {
        this->reveal_tile(this->player_x, this->player_y);
      }
    }
    if (arduboy.justPressed(B_BUTTON)) {
      this->toggle_flag();
    }
  }

  void reveal_current_neighbors() {
    for (int8_t y = -1; y <= 1; y++) {
      for (int8_t x = -1; x <= 1; x++) {
        this->reveal_tile(this->player_x + x, this->player_y + y);
      }
    }
  }

  void reveal_tile(uint8_t t_x, uint8_t t_y) {
    if (t_x < 0 || t_x >= FIELD_W || t_y < 0 || t_y >= FIELD_H) return;

    uint8_t field_value = field[t_y][t_x];

    if (IS_REVEALED(field_value) || IS_FLAGGED(field_value)) return;

    if (IS_MINE(field_value)) {
      this->state = GameState::LOSE;
      return;
    }

    field[t_y][t_x] = REVEAL(field_value);

    this->revealed_cnt += 1;

    if (SPRITE_INDEX(field_value) != 0) return;

    for (int8_t y = -1; y <= 1; y++) {
      for (int8_t x = -1; x <= 1; x++) {
        this->reveal_tile(t_x + x, t_y + y);
      }
    }
  }

  void toggle_flag() {
    if (!IS_REVEALED(field[this->player_y][this->player_x])) {
      field[this->player_y][this->player_x] = TOGGLE_IS_FLAGGED(field[this->player_y][this->player_x]);
    }
  }

  void generate_field() {
    uint8_t mines_created = 0, try_again, neighbor;
    int16_t m_x = 0, m_y = 0;

    while (mines_created < this->mines_cnt) {
      try_again = false;
      m_x = (uint8_t)random(0, FIELD_W);
      m_y = (uint8_t)random(0, FIELD_H);

      if (abs(m_x - this->player_x) <= 1 && abs(m_y - this->player_y) <= 1) {
        continue;
      }

      for (uint8_t i = 0; i <= mines_created; i++) {
        if ((mines_positions[i][0] == m_x && mines_positions[i][1] == m_y)) {
          try_again = true;
          break;
        }
      }

      if (try_again) continue;
      mines_positions[mines_created][0] = m_x;
      mines_positions[mines_created][1] = m_y;
      mines_created += 1;
    }

    for (uint8_t i = 0; i < this->mines_cnt; i++) {
      m_x = mines_positions[i][0];
      m_y = mines_positions[i][1];

      field[m_y][m_x] = MINE(field[m_y][m_x]);
      field[m_y][m_x] = SET_SPRITE_INDEX(field[m_y][m_x], MINE_TILE);

      for (int8_t y = -1; y <= 1; y++) {
        for (int8_t x = -1; x <= 1; x++) {
          if (x == 0 && y == 0) continue;  // we don't need to look into current cell
          if (m_x + x >= 0 && m_x + x < FIELD_W && m_y + y >= 0 && m_y + y < FIELD_H) {
            neighbor = field[m_y + y][m_x + x];
            if (IS_MINE(neighbor)) continue;
            field[m_y + y][m_x + x] = SET_SPRITE_INDEX(neighbor, min(SPRITE_INDEX(neighbor) + 1, 8));
          }
        }
      }
    }
  }

  void gameover_reveal() {
    uint8_t m_x, m_y;
    for (uint8_t i = 0; i < this->mines_cnt; i++) {
      m_x = mines_positions[i][0];
      m_y = mines_positions[i][1];
      field[m_y][m_x] = REVEAL(field[m_y][m_x]);
    }
  }

  void draw_player() {
    arduboy.drawRect(this->player_x * TILE_SIZE, this->player_y * TILE_SIZE, TILE_SIZE, TILE_SIZE, WHITE);
  }

  void draw_field() {
    uint8_t value;

    for (uint8_t y = 0; y < FIELD_H; y++) {
      for (uint8_t x = 0; x < FIELD_W; x++) {
        value = field[y][x];

        Sprites::drawOverwrite(
            x * TILE_SIZE,
            y * TILE_SIZE,
            minesweeper_sprites,
            IS_REVEALED(value) == 1 ? SPRITE_INDEX(value) : (IS_FLAGGED(value) == 1 ? FLAG_TILE : NOT_OPEN_TILE));
      }
    }
  }

  void loop() {
    this->player_input();

    this->draw_field();
    this->draw_player();

    if (this->check_winning_condition()) state = GameState::WIN;
  }

  void end_screen() {
    this->gameover_reveal();
    this->draw_field();

    this->arduboy.digitalWriteRGB(state == GameState::WIN ? GREEN_LED : RED_LED, RGB_ON);

    if (arduboy.justPressed(A_BUTTON | B_BUTTON)) {
      this->state = GameState::START;
      arduboy.delayShort(100);
    }
  }

  void init_field() {
    memset(field, 0, sizeof(field[0]) * FIELD_H);
    memset(mines_positions, 0, sizeof(mines_positions[0]) * MAX_MINES_CNT);
  };

  bool check_winning_condition() {
    return FIELD_W * FIELD_H - revealed_cnt == mines_cnt;
  }

  Arduboy2 arduboy;
  uint8_t fps;

  uint8_t field[FIELD_H][FIELD_W];
  uint8_t mines_positions[MAX_MINES_CNT][2];

  int16_t player_x, player_y;

  uint8_t mines_cnt;
  uint8_t revealed_cnt;
  

  GameState state;

  bool is_first_move;
  bool title_screen_blink;
};