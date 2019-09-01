#include <Arduboy2.h>
#include "bitmaps.h"

Arduboy2 test;

struct Bullet{
  int16_t _size;
  int16_t x;
  int16_t y;
  float vx;
  float vy; 
}bullet;

struct Entity{
  const uint8_t* sprite;
  int16_t _size;
  float x;
  float y;
  float vx;
  float vy; 
};

enum Game_state{START, RUN, WIN, GAMEOVER}game_state;

bool is_enemy_alive[map_size];

uint8_t cur_sector;
bool able_shoot = true;
Entity player = {PlayerRight, SPRITE_16, WIDTH-SPRITE_16, SPRITE_16+4, 0, 0};
uint8_t dir;
uint8_t enemy_hp = 3;
Entity enemy;


void drawPlayer(){
  //test.fillRect(player.x, player.y, player._size, player._size, BLACK);
  test.drawBitmap(player.x, player.y, player.sprite, player._size, player._size, WHITE);  
}

void drawEnemies(){
  //test.fillRect(enemy.x, enemy.y, enemy._size, enemy._size, BLACK); 
  if(is_enemy_alive[cur_sector]){
    test.drawBitmap(enemy.x, enemy.y, enemy.sprite, enemy._size, enemy._size, WHITE);              
    test.drawBitmap(enemy.x, enemy.y-SPRITE_8/2, Hp[3-enemy_hp], SPRITE_16, SPRITE_8, WHITE);
  }
}

void drawBullets(){
  if(!able_shoot) test.fillRect(bullet.x, bullet.y, bullet._size, bullet._size/2, WHITE);                   
}

void createEnemy(){
  enemy.x = random(0, (WIDTH-SPRITE_16-SPRITE_8-4) + 8);
  enemy.y = random(0, (HEIGHT-SPRITE_16-SPRITE_8-4) + 8); 
  enemy_hp = 3;
  enemy.vx = 0.5;
  enemy.vy = 0.5;
}

void start(){
  cur_sector = map_size/2;
  for(int i = 0; i < 9; i++)
    is_enemy_alive[i] = true;
  enemy.sprite = EnemyLeft;
  enemy._size = SPRITE_16;
  createEnemy();
  enemy.x = 0;
  enemy.y = HEIGHT - SPRITE_16;
  dir = 0;
  player.sprite = PlayerLeft;      
  player.x = WIDTH-SPRITE_16;
  player.y = SPRITE_16+4;
  player.vx = 0;
  player.vy = 0;
}

void drawTitleScreen(){
  test.clear();
  test.setCursor(WIDTH/4, 2);
  test.setTextSize(2);
  test.print("SPACE");
  test.setCursor(WIDTH/16, HEIGHT/2 - 10);
  test.print("ADVENTURE");
  test.setTextSize(1);
  test.setCursor(WIDTH/4, 3*HEIGHT/4);
  test.print("Press start");
  test.display();
  if(test.pressed(A_BUTTON) || test.pressed(B_BUTTON)){
    start();
    game_state = RUN;
    test.delayShort(150);
  }
}

void drawGameOverScreen(){
  test.clear();
  test.setCursor(WIDTH/4-20, HEIGHT/2-16);
  test.setTextSize(2);
  test.print("GAME OVER");
  test.display();
  if(test.pressed(A_BUTTON) || test.pressed(B_BUTTON)){
    game_state = START;
    test.delayShort(150);
  }
}

void setup() {
  test.begin();
  Serial.begin(9600);
  test.initRandomSeed();
  test.setFrameRate(60);
  game_state = START;
}

void game_loop(){
  if(test.pressed(UP_BUTTON)){
    player.vy-=0.5; 
  }
  if(test.pressed(DOWN_BUTTON)){
    player.vy+=0.5;  
  }
  if(test.pressed(LEFT_BUTTON)){
    player.vx-=0.5;
    dir = 0;
    player.sprite = PlayerLeft; 
  }
  if(test.pressed(RIGHT_BUTTON)){
    player.vx+=0.5;
    dir = 1;
    player.sprite = PlayerRight; 
  }

  if(test.pressed(A_BUTTON) || test.pressed(B_BUTTON)){
    if(able_shoot){
      bullet.vy = 0;
      bullet._size = 4;
      bullet.y = player.y+SPRITE_16/2-2;
      if(dir > 0){
        bullet.x = player.x+SPRITE_16/2;
        bullet.vx = BULLET_SPEED;
      } else {
        bullet.x = player.x+SPRITE_16/2 - bullet._size;
        bullet.vx = -BULLET_SPEED;  
      }
      able_shoot = false;
    }
  }  
   
  player.x += player.vx, player.y += player.vy;
  float deltax = abs(player.x - enemy.x)/(player.x - enemy.x);
  float deltay = abs(player.y - enemy.y)/(player.y - enemy.y);
  if(deltax > 0) enemy.sprite = EnemyRight;
  else enemy.sprite = EnemyLeft;
  if(player.x !=  enemy.x) enemy.x += enemy.vx*deltax;
  if(player.y !=  enemy.y) enemy.y += enemy.vy*deltay;
  bullet.x += bullet.vx;
  
  if(player.x + player._size <= 0){
    if(cur_sector%3 != 0){
      player.x = WIDTH;
      (cur_sector--)%map_size;
      if(is_enemy_alive[cur_sector]){
        createEnemy();
      }
    } else {
      player.vx = -player.vx;
      dir = 1;
      player.sprite = PlayerRight;
    }
  }
  if(player.x > WIDTH){
    if((cur_sector+1)%3 != 0) {
      player.x = 0;
      (cur_sector++)%map_size;
      if(is_enemy_alive[cur_sector]){
        createEnemy();
      }
    } else {
      dir = 0;
      player.vx = -player.vx;
      player.sprite = PlayerLeft;
    }
  }
  if(player.y + player._size <= 0){
    if(cur_sector > 2){
      player.y = HEIGHT;
      (cur_sector-=3)%map_size;
      if(is_enemy_alive[cur_sector]){
        createEnemy();
      }
    } else {
      player.vy = -player.vy;
    }
  }
  if(player.y > HEIGHT){
    if(cur_sector < 6){
      player.y = 0;
      (cur_sector+=3)%map_size;
      if(is_enemy_alive[cur_sector]){
        createEnemy();
      }
    } else {
      player.vy = -player.vy;
    }
  }
  
  if(bullet.x+bullet._size <= 0 || bullet.x >= WIDTH - 1){
    able_shoot = true;  
  }

  if(test.collide({bullet.x, bullet.y, bullet._size, bullet._size},
                  {enemy.x, enemy.y, enemy._size, enemy._size})){
    enemy_hp--;
    able_shoot = true;
    if(enemy_hp == 0){
      is_enemy_alive[cur_sector] = false;
    }
  }

  if(is_enemy_alive[cur_sector] && test.collide({player.x+3, player.y+3, 10, 10},
                  {enemy.x+3, enemy.y+3, 10, 10})){
    game_state = GAMEOVER;
    return;                  
  }
  
  test.clear();
  test.drawBitmap(0, 0, BackGr[cur_sector], WIDTH, HEIGHT, WHITE);
  test.drawBitmap(2, 2, Map[cur_sector], SPRITE_8,  SPRITE_8, WHITE);
  drawEnemies();
  drawPlayer();
  drawBullets();
  test.display();
  test.delayShort(30);    
}

void loop() {
  if(!test.nextFrame()) {
    return;
  }
  switch(game_state){
    case START:
      drawTitleScreen();
      break;
    case RUN:
      game_loop();
      break;
    case GAMEOVER:
      drawGameOverScreen();
      break;      
  }
}
