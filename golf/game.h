#include "Arduboy2Core.h"
#include <Arduboy2.h>

constexpr int SUBPIXELS = 16;
constexpr int SUB_WIDTH = WIDTH * SUBPIXELS;
constexpr int SUB_HEIGHT = HEIGHT * SUBPIXELS;
constexpr int VEL_DECREASE_FACTOR = 2;
constexpr int INIT_VEL = 45;

#define TO_WORLD(x) (int)(x >> 4)


enum class GameState: uint8_t {
  INIT_SCREEN,
  RUNNING,
  GAMEOVER,
};

struct Ball {
  int x;
  int y;
  int radius;

  int vel_x;
  int vel_y;
  bool is_moving;

  void init() {
    x = SUB_WIDTH / 2;
    y = SUB_HEIGHT / 2;
    radius = 4;
    vel_x = 0;
    vel_y = 0;
    is_moving = false;
  }

  void draw(Arduboy2& a) {
    a.fillCircle(TO_WORLD(x), TO_WORLD(y), this->radius, WHITE);
  }

  void update(const float& angle) {
      this->x -= (int)(this->vel_x * cosf(angle));
      this->y -= (int)(this->vel_y * sinf(angle));
      // todo: decrease velocity based in current animation frame
      this->vel_x -= VEL_DECREASE_FACTOR;
      if (this->vel_x <= 0) {
        this->vel_x = 0;
      }

      this->vel_y -= VEL_DECREASE_FACTOR;
      if (this->vel_y <= 0) {
        this->vel_y = 0;
      } 
      
      this->is_moving = (this->vel_x != 0) || (this->vel_y != 0);
  }
};

struct Stick {
  float angle;
  int length;
  int dist_to_ball;

  void init() {
    length = 20;
    angle = 0;
    dist_to_ball = 5;
  }

  void draw(Arduboy2& a, Ball& ball) {
    float cos_angle = cosf(angle);
    float sin_angle = sinf(angle);
    int dist = dist_to_ball + ball.radius;
    int x1 = (int)((dist) * cos_angle) + TO_WORLD(ball.x);
    int y1 = (int)((dist) * sin_angle) + TO_WORLD(ball.y);
    int x2 = (int)((dist + length) * cos_angle) + TO_WORLD(ball.x);
    int y2 = (int)((dist + length) * sin_angle) + TO_WORLD(ball.y);
    a.drawLine(x1, y1, x2, y2, WHITE);
  }
};

class Game {
  static constexpr uint8_t DEFAULT_FPS = 60;

 public:
  Game(uint8_t fps = DEFAULT_FPS) {
    this->fps = fps;
  }

  void init(){
    this->arduboy.begin();
    this->arduboy.setFrameRate(fps);
    this->arduboy.initRandomSeed();
    this->reset();
  }

  void run() {
    if (!this->arduboy.nextFrame()) return;

    this->arduboy.clear();
    this->arduboy.pollButtons();
    
    switch (state) {
      case GameState::RUNNING:
        this->game_loop();
        break;
      case GameState::GAMEOVER:
      default:
        break;
    }
    
    this->arduboy.display();
  }

 private:
  void init_screen() {

    this->arduboy.print("init screen");

  }

  void game_loop() {
    // draw
    if (this->arduboy.pressed(LEFT_BUTTON)) {
      this->stick.angle += M_PI / 90;
      if (this->stick.angle >= 2 * M_PI) {
        this->stick.angle = 0;
      }
    }

    if (this->arduboy.pressed(RIGHT_BUTTON)) {
      this->stick.angle -= M_PI / 90;
      if (this->stick.angle < 0) {
        this->stick.angle = 2 * M_PI;
      }
    }

    if (this->arduboy.pressed(A_BUTTON) && !this->ball.is_moving) {
      this->ball.vel_x = INIT_VEL;
      this->ball.vel_y = INIT_VEL;
    }

    this->ball.update(this->stick.angle);


    // draw
    this->arduboy.print(TO_WORLD(ball.x));
    this->arduboy.print("\n");
    this->arduboy.print(TO_WORLD(ball.y));
    this->arduboy.print("\n");
    this->arduboy.print(this->ball.is_moving);

    this->ball.draw(this->arduboy);
    this->stick.draw(this->arduboy, this->ball);
  }

  void reset() {
    this->ball.init();
    this->stick.init();
    this->state = GameState::RUNNING;
    this->paused = false;
  }

  Arduboy2 arduboy;
  GameState state;

  uint8_t fps;
  bool paused;

  Ball ball;
  Stick stick;
};