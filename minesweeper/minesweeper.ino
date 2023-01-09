#include <Arduboy2.h>

#define FPS 24
#define TILE_SIZE 8
#define MINES_CNT 14

#define NOT_OPEN_TILE 9
#define MINE_TILE 10
#define FLAG_TILE 11

#define MINE_BIT_OFFSET 0
#define FLAGGED_BIT_OFFSET 1
#define REVEALED_BIT_OFFSET 2
#define SPRITED_INDEX_BIT_OFFSET 4
#define IS_MINE_MASK (1<<MINE_BIT_OFFSET)
#define IS_FLAGGED_MASK (1<<FLAGGED_BIT_OFFSET)
#define IS_REVEALED_MASK (1<<REVEALED_BIT_OFFSET)
#define SPRITE_INDEX_MASK (15<<SPRITED_INDEX_BIT_OFFSET) // bits 4-7 are 1s

// game state
#define GAME_START 0
#define GAME_RUNNING 1
#define GAME_GAMEOVER_1 2
#define GAME_GAMEOVER_2 3


Arduboy2 arduboy;


const uint8_t minesweeper_sprites[] PROGMEM = {
  8, 8,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // empty tile
  0x00, 0x00, 0x00, 0x08, 0x7c, 0x00, 0x00, 0x00, // 1
  0x00, 0x00, 0x74, 0x54, 0x54, 0x5c, 0x00, 0x00, // 2
  0x00, 0x00, 0x54, 0x54, 0x54, 0x7c, 0x00, 0x00, // 3
  0x00, 0x00, 0x1c, 0x10, 0x10, 0x7c, 0x00, 0x00, // 4
  0x00, 0x00, 0x5c, 0x54, 0x54, 0x74, 0x00, 0x00, // 5
  0x00, 0x00, 0x7c, 0x54, 0x54, 0x74, 0x00, 0x00, // 6
  0x00, 0x00, 0x04, 0x04, 0x04, 0x7c, 0x00, 0x00, // 7
  0x00, 0x00, 0x7c, 0x54, 0x54, 0x7c, 0x00, 0x00, // 8
  0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, // not open tile
  0x00, 0x5a, 0x3c, 0x7e, 0x7e, 0x3c, 0x5a, 0x00, // mine tile
  0x00, 0x00, 0x40, 0x7e, 0x6e, 0x46, 0x02, 0x00, // flag tile
};

const int FIELD_W = WIDTH / TILE_SIZE;
const int FIELD_H = HEIGHT / TILE_SIZE;


/* 
bytes 0-7
0 - is_mine
1 - is_flagged
2 - is_revealed
3 - empty
4-6 - sprite_index

*/
uint8_t field[FIELD_H][FIELD_W] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

uint8_t mines_positions[MINES_CNT][2];

int16_t player_x = 0;
int16_t player_y = 0;

uint8_t first_move;
uint8_t game_state;


uint8_t set_sprite_index(uint8_t value, uint8_t index) {
  value = value & ~SPRITE_INDEX_MASK; // clear sprite bits
  return value | (index << 4);
}

uint8_t get_sprite_index(uint8_t value) {
  return (value & SPRITE_INDEX_MASK) >> SPRITED_INDEX_BIT_OFFSET;
}

uint8_t set_is_mine(uint8_t value) {
  return value | IS_MINE_MASK;
}

uint8_t set_is_revealed(uint8_t value) {
  return value | IS_REVEALED_MASK;
}

uint8_t toggle_is_flagged(uint8_t value) {
  return value ^ IS_FLAGGED_MASK;
}

uint8_t get_is_mine(uint8_t value) {
  return (value & IS_MINE_MASK) >> MINE_BIT_OFFSET;
}

uint8_t get_is_flagged(uint8_t value) {
  return (value & IS_FLAGGED_MASK) >> FLAGGED_BIT_OFFSET;
}

uint8_t get_is_revealed(uint8_t value) {
  return (value & IS_REVEALED_MASK) >> REVEALED_BIT_OFFSET;
}


void create_mines(uint8_t init_x, uint8_t init_y) {
  uint8_t mines_created = 0;
  uint8_t m_x = 0;
  uint8_t m_y = 0;
  uint8_t try_again = 0;

  while (mines_created < MINES_CNT) {
    try_again = 0;
    m_x = (uint8_t)random(0, FIELD_W);
    m_y = (uint8_t)random(0, FIELD_H);

    if (abs(m_x - init_x) <= 1 && abs(m_y - init_y) <= 1) {
      continue;
    }

    for(uint8_t i = 0; i <= mines_created; i++) {
      if ((mines_positions[i][0] == m_x && mines_positions[i][1] == m_y)) {
          try_again = 1;
          break;
      }
    }

    if (try_again == 1) continue;
    mines_positions[mines_created][0] = m_x;
    mines_positions[mines_created][1] = m_y;
    mines_created += 1;
  }
}

void update_field(){
  uint8_t m_x, m_y;
  uint8_t neighbor;

  for(uint8_t i = 0; i < MINES_CNT; i++) {
    m_x = mines_positions[i][0];
    m_y = mines_positions[i][1];

    field[m_y][m_x] = set_is_mine(field[m_y][m_x]);
    field[m_y][m_x] = set_sprite_index(field[m_y][m_x], MINE_TILE);

    for (int8_t y = -1; y <= 1; y++) {
      for (int8_t x = -1; x <= 1; x++) {
        if (x == 0 && y == 0) continue; // we don't need to look into current cell
        if (m_x + x >= 0 && m_x + x < FIELD_W && m_y + y >= 0 && m_y + y < FIELD_H) {
          neighbor = field[m_y + y][m_x + x];
          if (get_is_mine(neighbor)) continue;
          field[m_y + y][m_x + x] = set_sprite_index(neighbor, min(get_sprite_index(neighbor) + 1, 8));
        }
      }
    }
  } 
}

void generate_field(uint8_t init_x, uint8_t init_y) {
  create_mines(init_x, init_y);
  update_field();
}


void toggle_flag_tile(int16_t x, int16_t y) {
  uint8_t is_flagged = get_is_flagged(field[y][x]);
  if (!get_is_revealed(field[y][x])) {
    field[y][x] = toggle_is_flagged(field[y][x]);
  }
}

void reveal_tile(int16_t t_x, int16_t t_y) {
  uint8_t field_value = field[t_y][t_x];

  if (get_is_revealed(field_value) || get_is_flagged(field_value)) return;

  field[t_y][t_x] = set_is_revealed(field_value);

  if (get_is_mine(field_value) == 1) {
    game_state = GAME_GAMEOVER_1;
    return;
  }

  if (get_sprite_index(field_value) != 0) return;

  // if it's 0 reval all neighbors as well
  for (int8_t y = -1; y <= 1; y++) {
      for (int8_t x = -1; x <= 1; x++) {
        if (t_x + x >= 0 && t_x + x < FIELD_W && t_y + y >= 0 && t_y + y < FIELD_H) {
          reveal_tile(t_x + x, t_y + y);
        }
      }
    }
}

void gameover_reveal() {
  uint8_t m_x, m_y;
  for(uint8_t i = 0; i < MINES_CNT; i++) {
    m_x = mines_positions[i][0];
    m_y = mines_positions[i][1];
    field[m_y][m_x] = set_is_revealed(field[m_y][m_x]);
  }
}

void draw_player() {
  arduboy.drawRect(player_x * TILE_SIZE, player_y * TILE_SIZE, TILE_SIZE, TILE_SIZE, WHITE);
}

void draw_field() {
  uint8_t value;
  for (uint8_t y = 0; y < FIELD_H; y ++) {
    for (uint8_t x = 0; x < FIELD_W; x ++) {
      value = field[y][x];

      Sprites::drawOverwrite(
        x * TILE_SIZE, 
        y * TILE_SIZE, 
        minesweeper_sprites, 
        get_is_revealed(value) == 1 ? get_sprite_index(value) : (get_is_flagged(value) == 1 ? FLAG_TILE : NOT_OPEN_TILE));
    }
  }
}


void handle_input(){
  if (arduboy.justPressed(DOWN_BUTTON)) {
    if (player_y + 1 < FIELD_H) {
      player_y += 1;
    } 
  }
  if (arduboy.justPressed(UP_BUTTON)) {
    if (player_y - 1 >= 0) {
      player_y -= 1;
    }
  }
  if (arduboy.justPressed(RIGHT_BUTTON)) {
    if (player_x + 1 < FIELD_W) {
      player_x += 1;
    }
  }
  if (arduboy.justPressed(LEFT_BUTTON)) {
    if (player_x - 1 >= 0) {
      player_x -= 1;
    }
  }
  if (arduboy.justPressed(A_BUTTON)) {
    if (first_move == 1) {
      generate_field(player_x, player_y);
      first_move = 0;
    }
    reveal_tile(player_x, player_y);
  }
  if (arduboy.justPressed(B_BUTTON)) {
    toggle_flag_tile(player_x, player_y);
  }
}

// void draw_game_over_screen()
// {
//   // arduboy.clear();
//   arduboy.setCursor(WIDTH / 4 - 20, HEIGHT / 2 - 10);
//   arduboy.setTextSize(2);
//   arduboy.print("GAME OVER");
//   arduboy.display();
//   if (arduboy.pressed(A_BUTTON) || arduboy.pressed(B_BUTTON))
//   {
//     game_state = START;
//     arduboy.delayShort(250);
//   }
// }

void init_field() {
  for (uint8_t y = 0; y < FIELD_H; y++) {
    for (uint8_t x= 0; x < FIELD_W; x++) {
      field[y][x] = 0;
    }
  }
}

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(FPS);
  arduboy.initRandomSeed();
  game_state = GAME_START;
}

void game_loop() {
  handle_input();

  arduboy.clear();
  draw_field();
  draw_player();
  arduboy.display();
}

void draw_title_screen() {
  arduboy.clear();
  arduboy.setCursor(2, 2);
  arduboy.setTextSize(2);
  arduboy.print("MINES");
  arduboy.setTextSize(1);
  arduboy.setCursor(2, 3 * HEIGHT / 4);
  arduboy.print("Press A to play");
  arduboy.display();
  if (arduboy.justPressed(A_BUTTON)) {
    init_field();
    first_move = 1;
    game_state = GAME_RUNNING;
    arduboy.delayShort(100);
  }
}


void draw_gameover_state() {
  arduboy.clear();
  gameover_reveal();
  draw_field();
  arduboy.setCursor(2, 2);
  arduboy.setTextSize(2);
  arduboy.print("GAME OVER");
  arduboy.display();
  if (arduboy.justPressed(A_BUTTON)) {
    game_state = GAME_START;
    arduboy.delayShort(100);
  }
}

/*
TODO: winning condition && screen
*/
void loop() {
  if (!arduboy.nextFrame()) return;
  arduboy.pollButtons();

  switch (game_state) {
    case GAME_START:
      draw_title_screen();
      break;
    case GAME_RUNNING:
      game_loop();
      break;
    case GAME_GAMEOVER_1:
      draw_gameover_state();
      break;
  }
}