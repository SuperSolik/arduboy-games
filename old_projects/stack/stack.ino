#include <Arduboy2.h>
#include "bitmaps.h"
Arduboy2 test;

struct Block {
  uint8_t x;
  uint8_t y;
};

uint8_t field[8][16];
Block platform[3];
uint8_t level = 0;
uint8_t platform_size = 3;
uint8_t dir;

enum Game_state {START, RUN, WIN, GAMEOVER} game_state;

void draw_platform() {
  for (uint8_t i = 0; i < platform_size; i++) {
    test.drawBitmap(platform[i].x, platform[i].y, Block_Sprite, SPRITE_8, SPRITE_8, WHITE);
  }
}

void draw_field() {
  int k = 0;
  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 16; j++) {
      if (field[i][j]) test.drawBitmap(j * 8 + 1, i * 8, Block_Sprite, SPRITE_8, SPRITE_8, WHITE);
    }
  }
}

void move_platform() {
  if (platform[0].y + SPRITE_8 * platform_size >= HEIGHT || platform[0].y <= 0) {
    dir = -dir;
  }
  for (uint8_t i = 0; i < platform_size; i++) {
    platform[i].y += dir * SPRITE_8;
  }
}

void start() {
  test.clear();
  test.print("START");
  test.display();
  if (test.pressed(A_BUTTON) || test.pressed(B_BUTTON)) {
    platform_size = 3;
    dir = 1;
    level = 0;
    for (uint8_t i = 0; i < platform_size; i++) {
      platform[i].x = 1;
      platform[i].y = (i * 8) + 8;
    }
    for (uint8_t i = 0; i < 8; i++) {
      for (uint8_t j = 0; j < 16; j++) {
        field[i][j] = 0;
      }
    }
    game_state = RUN;
    test.delayShort(150);
  }
}

void gameover() {
  test.clear();
  test.print("GAME OVER");
  test.display();
  if (test.pressed(A_BUTTON) || test.pressed(B_BUTTON)){
    game_state = START;
    test.delayShort(150);
  }
}

void win_screen() {
  test.clear();
  test.print("YOU WON");
  test.display();
  if (test.pressed(A_BUTTON) || test.pressed(B_BUTTON)){
    game_state = START;
    test.delayShort(150);
  }
}

void game_loop() {
  int life_count = 0;
  test.clear();
  test.pollButtons();
  if (test.pressed(A_BUTTON) || test.pressed(B_BUTTON)) {
    if (level > 0) {
      for (uint8_t i = 0; i < platform_size; i++) {
        if (field[platform[i].y / 8][level - 1] == 1) {
          field[platform[i].y / 8][level] = 1;
          life_count++;
        }
      }
      if (life_count == 0){
        game_state = GAMEOVER;
        return;
      }
      platform_size = life_count;
      for (uint8_t i = 0; i < platform_size; i++) {
        platform[i].x += 8;
      }
    } else {
      for (uint8_t i = 0; i < 3; i++) {
        field[platform[i].y / 8][level] = 1;
        platform[i].x += 8;
      }
    }
    level++;
    if (level >= 16){
      game_state = WIN;
      return;
    }
  }
  move_platform();
  draw_platform();
  if (level > 0) draw_field();
  test.display();
  test.delayShort(75);
}

void setup() {
  test.begin();
  test.setFrameRate(60);
  game_state = START;
}

void loop() {
  if (!test.nextFrame()) {
    return;
  }
  switch (game_state) {
    case START:
      start();
      break;
    case RUN:
      game_loop();
      break;
    case WIN:
      win_screen();
      break;
    case GAMEOVER:
      gameover();
      break;
  }
}
