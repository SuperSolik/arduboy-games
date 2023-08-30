#include "Arduboy2Core.h"
#include <Arduboy2.h>

constexpr int SUBPIXELS = 16;
constexpr int SUB_WIDTH = WIDTH * SUBPIXELS;
constexpr int SUB_HEIGHT = HEIGHT * SUBPIXELS;

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

  void init() {
    x = SUB_WIDTH / 2;
    y = SUB_HEIGHT / 2;
    radius = 4;
    vel_x = 16;
    vel_y = 16;
  }

  void draw(Arduboy2& a) {
    a.fillCircle(TO_WORLD(x), TO_WORLD(y), this->radius, WHITE);
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

    if (this->arduboy.pressed(A_BUTTON)) {
      this->ball.x -= (int)(this->ball.vel_x * cosf(this->stick.angle));
      this->ball.y -= (int)(this->ball.vel_y * sinf(this->stick.angle));
    }

    this->arduboy.print(TO_WORLD(ball.x));
    this->arduboy.print("\n");
    this->arduboy.print(TO_WORLD(ball.y));
    this->arduboy.print("\n");
    this->arduboy.print(stick.angle * 180 / M_PI);

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