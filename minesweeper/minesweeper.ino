#include <Arduboy2.h>

#define FPS 24

#include "ard_periodic_events.h"

Arduboy2 arduboy;

#define ARDBITMAP_SBUF arduboy.getBuffer()
#include <ArdBitmap.h>
ArdBitmap<WIDTH, HEIGHT> ardbitmap;



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

#define SET_SPRITE_INDEX(x, index) ((x & ~SPRITE_INDEX_MASK) | (index << 4))
#define SPRITE_INDEX(x) ((x & SPRITE_INDEX_MASK) >> SPRITED_INDEX_BIT_OFFSET)
#define MINE(x) (x | IS_MINE_MASK)
#define REVEAL(x) (x | IS_REVEALED_MASK)
#define TOGGLE_IS_FLAGGED(x) (x ^ IS_FLAGGED_MASK)
#define IS_MINE(x) ((x & IS_MINE_MASK) >> MINE_BIT_OFFSET)
#define IS_FLAGGED(x) ((x & IS_FLAGGED_MASK) >> FLAGGED_BIT_OFFSET)
#define IS_REVEALED(x) ((x & IS_REVEALED_MASK) >> REVEALED_BIT_OFFSET)

// game state
#define GAME_START 0
#define GAME_RUNNING 1
#define GAME_OVER 2
#define GAME_WIN 3


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

const unsigned char PROGMEM CMP_LOGO[] ={
  0x7f, 0xdf, 0x7f, 0x1f, 0xae, 0x5e, 0xbd, 0x7a, 0xf5, 0xca, 0x7f, 0x4f, 0x91, 0xc3, 0x3a, 0xec,
  0xc3, 0x3a, 0x20, 0xef, 0xef, 0xd3, 0x79, 0x9f, 0xce, 0xfb, 0x46, 0x1a, 0x55, 0x53, 0x64, 0x8f,
  0x35, 0xe2, 0x32, 0xb9, 0xe4, 0x3e, 0xd6, 0xd2, 0xa5, 0xea, 0xab, 0xcb, 0x52, 0x75, 0xa9, 0xfa,
  0xea, 0xf2, 0xea, 0xf2, 0xea, 0xf2, 0xea, 0xb2, 0x54, 0x5d, 0xaa, 0xbe, 0xba, 0x2c, 0x55, 0x97,
  0xaa, 0xaf, 0x2e, 0xcb, 0xe5, 0xba, 0xbc, 0xba, 0x2c, 0x55, 0x97, 0xaa, 0x4f, 0xc7, 0x1a, 0x66,
  0xb9, 0x5c, 0xbc, 0xc7, 0x5e, 0xe6, 0x9e, 0x97, 0xca, 0xef, 0xc7, 0x5e, 0xe6, 0x9e, 0x97, 0xca,
  0x67, 0x2e, 0xaf, 0x2e, 0xaf, 0x2e, 0xaf, 0x2e, 0xaf, 0x2e, 0xaf, 0x2e, 0xaf, 0x2e, 0x4b, 0xd5,
  0xa5, 0xdc, 0xd1, 0xa5, 0xea, 0xd2, 0xb5, 0xc6, 0x7d, 0xac, 0x11, 0x97, 0x19, 0x6b, 0xc4, 0x65,
  0xc6, 0x1a, 0xf7, 0xb1, 0x96, 0x2e, 0x55, 0x97, 0x72, 0x1f, 0x6b, 0xe9, 0x52, 0x75, 0x29, 0xf7,
  0xe9, 0xc4, 0x8b, 0xe4, 0x92, 0xfb, 0x58, 0x4b, 0x97, 0xaa, 0x4b, 0x79, 0xe2, 0xba, 0x2c, 0x3f,
  0x89, 0x1c, 0xd6, 0x61, 0x1f, 0xd6, 0x01, 0xf9, 0xdf, 0x0d,
};

const uint8_t PROGMEM CMP_PRESS_A_TO_PLAY[] ={
  0x3f, 0x07, 0x5f, 0x5a, 0x22, 0xed, 0x6d, 0x84, 0xf3, 0x08, 0x00, 0x01, 0xe0, 0x14, 0x08, 0x00,
  0x81, 0x9c, 0x02, 0x01, 0x20, 0x90, 0x4f, 0x0c, 0x91, 0x22, 0x45, 0xc6, 0x78, 0xca, 0x1c, 0xe1,
  0x9e, 0x12, 0x9a, 0xb7, 0x4b, 0xa4, 0xbd, 0x8d, 0xc9, 0xe4, 0xd6, 0x00, 0xab, 0xa7, 0xc0, 0x00,
  0x32, 0x3e, 0x00,
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
int16_t player_x = 0, player_y = 0;
uint8_t revealed_cnt, first_move, game_state;


void create_mines(uint8_t init_x, uint8_t init_y) {
  uint8_t mines_created =0, try_again = 0;
  uint8_t m_x = 0, m_y = 0;

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
  uint8_t m_x, m_y, neighbor;

  for(uint8_t i = 0; i < MINES_CNT; i++) {
    m_x = mines_positions[i][0];
    m_y = mines_positions[i][1];

    field[m_y][m_x] = MINE(field[m_y][m_x]);
    field[m_y][m_x] = SET_SPRITE_INDEX(field[m_y][m_x], MINE_TILE);

    for (int8_t y = -1; y <= 1; y++) {
      for (int8_t x = -1; x <= 1; x++) {
        if (x == 0 && y == 0) continue; // we don't need to look into current cell
        if (m_x + x >= 0 && m_x + x < FIELD_W && m_y + y >= 0 && m_y + y < FIELD_H) {
          neighbor = field[m_y + y][m_x + x];
          if (IS_MINE(neighbor)) continue;
          field[m_y + y][m_x + x] = SET_SPRITE_INDEX(neighbor, min(SPRITE_INDEX(neighbor) + 1, 8));
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
  if (!IS_REVEALED(field[y][x])) {
    field[y][x] = TOGGLE_IS_FLAGGED(field[y][x]);
  }
}

void reveal_tile(int16_t t_x, int16_t t_y) {
  if (t_x < 0 || t_x >= FIELD_W || t_y < 0 || t_y >= FIELD_H) return;

  uint8_t field_value = field[t_y][t_x];

  if (IS_REVEALED(field_value) || IS_FLAGGED(field_value)) return;

   if (IS_MINE(field_value)) {
    game_state = GAME_OVER;
    return;
  }

  field[t_y][t_x] = REVEAL(field_value);

  revealed_cnt += 1;

  if (SPRITE_INDEX(field_value) != 0) return;

  for (int8_t y = -1; y <= 1; y++) {
    for (int8_t x = -1; x <= 1; x++) {
      reveal_tile(t_x + x, t_y + y);
    }
  }
}

void reveal_revealed_neighbors(int16_t t_x, int16_t t_y) {
  for (int8_t y = -1; y <= 1; y++) {
    for (int8_t x = -1; x <= 1; x++) {
      reveal_tile(t_x + x, t_y + y);
    }
  }
}

void gameover_reveal() {
  uint8_t m_x, m_y;
  for(uint8_t i = 0; i < MINES_CNT; i++) {
    m_x = mines_positions[i][0];
    m_y = mines_positions[i][1];
    field[m_y][m_x] = REVEAL(field[m_y][m_x]);
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
        IS_REVEALED(value) == 1 ? SPRITE_INDEX(value) : (IS_FLAGGED(value) == 1 ? FLAG_TILE : NOT_OPEN_TILE));
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

    if(IS_REVEALED(field[player_y][player_x])) {
      reveal_revealed_neighbors(player_x, player_y);
    } else {
      reveal_tile(player_x, player_y);
    }
  }
  if (arduboy.justPressed(B_BUTTON)) {
    toggle_flag_tile(player_x, player_y);
  }
}

void init_field() {
  for (uint8_t y = 0; y < FIELD_H; y++) {
    for (uint8_t x= 0; x < FIELD_W; x++) {
      field[y][x] = 0;
    }
  }
  revealed_cnt = 0;
}

uint8_t check_winning_condition() {
  return FIELD_W * FIELD_H - revealed_cnt == MINES_CNT;
}

void game_loop() {
  handle_input();

  arduboy.clear();
  draw_field();
  draw_player();
  
  if (check_winning_condition() == 1) game_state = GAME_WIN;

  arduboy.display();
}

uint16_t flashing_press_a_to_play_id = register_periodic_event();

void draw_press_play() {
  ardbitmap.drawCompressed(WIDTH / 2, HEIGHT / 2 + 10, CMP_PRESS_A_TO_PLAY, WHITE, ALIGN_CENTER, MIRROR_NONE);  
}

void draw_title_screen() {
  arduboy.clear();

  ardbitmap.drawCompressed(0, 0, CMP_LOGO, WHITE, ALIGN_NONE, MIRROR_NONE);
  PERIODIC_EVENT(draw_press_play(), flashing_press_a_to_play_id, arduboy, MILLIS_TO_FRAMES(850));
  
  arduboy.display();
  arduboy.digitalWriteRGB(RED_LED, RGB_OFF);
  arduboy.digitalWriteRGB(GREEN_LED, RGB_OFF);

  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) {
    init_field();
    first_move = 1;
    game_state = GAME_RUNNING;
    arduboy.delayShort(150);
  }
}


void draw_end_screen() {
  arduboy.clear();
  gameover_reveal();
  draw_field();
  arduboy.display();
  arduboy.digitalWriteRGB(game_state == GAME_WIN ? GREEN_LED : RED_LED, RGB_ON);
  if (arduboy.justPressed(A_BUTTON | B_BUTTON)) {
    game_state = GAME_START;
    arduboy.delayShort(100);
  }
}

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(FPS);
  arduboy.initRandomSeed();
  game_state = GAME_START;
}

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
    case GAME_OVER:
    case GAME_WIN:
      draw_end_screen();
      break;
  }
}