#include <Arduboy2.h>
Arduboy2 test;
#define FRUIT_COUNT 4
#define SIZE 4
#define SPEED 2

enum GameState{ START, RUN, GAMEOVER };

enum Dir{ UP, DOWN, LEFT, RIGHT};

struct Segments{
  int x, y;
};
 
struct Snake{
  Segments body[50];
  int count;
};

Dir dir;
GameState state = START;
Snake snake;
Segments fruits[FRUIT_COUNT];

bool contact(int x1, int y1, int x2, int y2){
  if(x2 >= x1 && x2 <= x1 + SIZE && y2 >= y1 && y2 <= y1 + SIZE ) return true;
  else return false; 
}

bool contact(int coord1, int coord2, int coord3){
  if(coord1 == coord2 || coord1+SIZE == coord3) return true;
  return false; 
} 

void drawTitleScreen(){
  test.clear();
  test.setCursor(WIDTH/4, HEIGHT/2 - 5);
  test.setTextSize(2);
  test.print("SNAKE");
  test.setTextSize(1);
  test.setCursor(WIDTH/4, 3*HEIGHT/4);
  test.print("Press start");
  test.display();
  if(test.pressed(A_BUTTON) || test.pressed(B_BUTTON)){
    start();
    state = RUN;
    test.delayShort(150);
  }
}

void drawGameOver(){
  test.clear();
  test.setCursor(WIDTH/4 + 5, HEIGHT/2 - 5);
  test.print("GAME OVER");
  test.setCursor(WIDTH/4 + 5, 3*HEIGHT/4);
  test.print("Score: ");
  test.print(snake.count-1);
  test.display();
  if(test.pressed(A_BUTTON) || test.pressed(B_BUTTON)){
    state = START;
    test.delayShort(150);
  }
}

void start(){
  dir = RIGHT;
  snake.count = 1;
  snake.body[0].x = WIDTH/2;
  snake.body[0].y = HEIGHT/2;
  for(int i = 0; i < FRUIT_COUNT; i++){
    fruits[i].x = rand()%(WIDTH - SIZE);
    fruits[i].y = rand()%(HEIGHT - SIZE);  
  }  
}

void game_run(){
  test.clear(); 
  test.setCursor(WIDTH-15, 0);
  test.print(snake.count - 1);
  for(int i = 0; i < FRUIT_COUNT; i++){
    if(test.collide({snake.body[0].x, snake.body[0].y, SIZE, SIZE}, {fruits[i].x, fruits[i].y, SIZE, SIZE})){
      snake.count++;
      fruits[i].x = rand()%(WIDTH - SIZE);
      fruits[i].y = rand()%(HEIGHT - SIZE);    
    }
  }
  
  if(test.pressed(UP_BUTTON)) dir = UP;
  if(test.pressed(DOWN_BUTTON)) dir = DOWN;
  if(test.pressed(LEFT_BUTTON)) dir = LEFT;
  if(test.pressed(RIGHT_BUTTON)) dir = RIGHT; 

  for(int i = snake.count; i > 0; i--){
    snake.body[i].x = snake.body[i-1].x;
    snake.body[i].y = snake.body[i-1].y;
  }
  switch(dir){
    case UP:
      snake.body[0].y-=SPEED;
      break;
    case DOWN:
      snake.body[0].y+=SPEED;
      break;
    case LEFT:
      snake.body[0].x-=SPEED;
      break;
    case RIGHT:
      snake.body[0].x+=SPEED;
      break;       
  }

  for(int i = 1; i < snake.count; i++){
    if(snake.body[0].x == snake.body[i].x && snake.body[0].y == snake.body[i].y){
      state = GAMEOVER;
      break;
    }
  }

  if(contact(snake.body[0].x, 0, WIDTH) || contact(snake.body[0].y, HEIGHT, 0)){
    state = GAMEOVER;  
  }
    
  for(int i = 0; i < FRUIT_COUNT; i++){
    test.fillRect(fruits[i].x, fruits[i].y, SIZE, SIZE, WHITE);
    test.fillRect(fruits[i].x + SIZE/4, fruits[i].y + SIZE/4, SIZE/2, SIZE/2, BLACK);
  }
  
  for(int i = 0; i < snake.count; i++){
    test.fillRect(snake.body[i].x, snake.body[i].y, SIZE, SIZE, WHITE);
  }
  test.display();
  test.delayShort(75); 
}

void setup(){
  test.begin();
  srand(101);
  test.setFrameRate(60);
}

void loop() {
  if(!test.nextFrame()) {
    return;
  }
 
  switch(state){
    case START:
      drawTitleScreen();
      break;
    case RUN:
      game_run();
      break;
    case GAMEOVER:
      drawGameOver();
      break;      
  }
}


