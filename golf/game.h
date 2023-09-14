#include <Arduboy2Core.h>
#include <Arduboy2.h>

constexpr int SUBPIXELS = 16;
constexpr int SUB_WIDTH = WIDTH * SUBPIXELS;
constexpr int SUB_HEIGHT = HEIGHT * SUBPIXELS;
constexpr int VEL_DECREASE_FACTOR = 1;
constexpr int INIT_VEL = 40;

inline int to_world(int x) {
  return (int)(x >> 4);
}

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
  float angle;
  bool is_moving;
  bool filled;

  void draw(Arduboy2& a) {
    if (filled) {
      a.fillCircle(to_world(x), to_world(y), to_world(this->radius), WHITE);
    } else {
      a.drawCircle(to_world(x), to_world(y), to_world(this->radius), WHITE);
    }
  }

  void draw_stick(Arduboy2& a, int dist_to_ball, int length) {
    float cos_angle = cosf(this->angle);
    float sin_angle = sinf(this->angle);
    int dist = dist_to_ball + to_world(this->radius);
    int x1 = (int)((dist) * cos_angle) + to_world(this->x);
    int y1 = (int)((dist) * sin_angle) + to_world(this->y);
    int x2 = (int)((dist + length) * cos_angle) + to_world(this->x);
    int y2 = (int)((dist + length) * sin_angle) + to_world(this->y);
    a.drawLine(x1, y1, x2, y2, WHITE);
  }

  void set_angle(float angle) {
    this->angle = angle;
  }

  void update() {
      this->x -= (int)(this->vel_x * cos(angle));
      this->y -= (int)(this->vel_y * sin(angle));
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

struct Line {
  int start_x;
  int start_y;
  int end_x;
  int end_y;

  float length() {
    return sqrt((float)(end_x - start_x) * (float)(end_x - start_x) + (float)(end_y - start_y) * (float)(end_y - start_y)); 
  }

  bool collide(Ball& ball){
    // todo: optimize ball to line collision
    Line first = {
      this->start_x,
      this->start_y,
      ball.x,
      ball.y,
    }; 

    float t = ((float)(first.end_x - first.start_x) * (float)(this->end_x - this->start_x) + (float)(first.end_y - first.start_y) * (float)(this->end_y - this->start_y)) / this->length();

    float cos_xs = (this->end_x - this->start_x) /  this->length();
    float sin_xs = sqrt(1 - cos_xs * cos_xs);

    Line normal = {
      ball.x,
      ball.y,
      this->start_x + t * cos_xs,
      this->start_y + t * sin_xs,
    };

    float normal_length = normal.length();

    if (this->start_x - ball.radius <= normal.end_x && this->end_x + ball.radius >= normal.end_x && 
        this->start_y - ball.radius <= normal.end_y && this->end_y + ball.radius >= normal.end_y 
      ) {
        int diff = normal_length - ball.radius;
        if (diff <= 0) {
          // TODO: push ball back + bounce
          // float normal_angle_cos = ((normal.end_x - normal.start_x) /  normal_length);
          // float bounce_angle = M_PI_2 + (abs(acos(normal_angle_cos) - acos(cos_xs)));

          // ball.x += cos(bounce_angle) * abs(diff);
          // ball.y += sin(bounce_angle) * abs(diff);

          // ball.set_angle(bounce_angle);
          return true;
        }
        return false;
      }
    return false;
  }

  void draw(const Arduboy2& a) {
    a.drawLine(to_world(start_x), to_world(start_y), to_world(end_x), to_world(end_y), WHITE);
  }
};

struct Stick {
  int length;
  int dist_to_ball;

  void draw(Arduboy2& a, Ball& ball) {
    float cos_angle = cosf(ball.angle);
    float sin_angle = sinf(ball.angle);
    int dist = dist_to_ball + to_world(ball.radius);
    int x1 = (int)((dist) * cos_angle) + to_world(ball.x);
    int y1 = (int)((dist) * sin_angle) + to_world(ball.y);
    int x2 = (int)((dist + length) * cos_angle) + to_world(ball.x);
    int y2 = (int)((dist + length) * sin_angle) + to_world(ball.y);
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
      this->ball.angle += M_PI / 90;
      if (this->ball.angle >= 2 * M_PI) {
        this->ball.angle = 0;
      }
    }

    if (this->arduboy.pressed(RIGHT_BUTTON)) {
      this->ball.angle -= M_PI / 90;
      if (this->ball.angle < 0) {
        this->ball.angle = 2 * M_PI;
      }
    }

    if (this->arduboy.justPressed(A_BUTTON) && !this->ball.is_moving) {
      this->ball.vel_x = INIT_VEL;
      this->ball.vel_y = INIT_VEL;
    }

    this->ball.filled = false;

    this->ball.update();

    for (uint8_t i = 0; i < 4; i++) {
      if (this->obstacles[i].collide(this->ball)) {
        this->ball.filled = true;
      }

      this->obstacles[i].draw(this->arduboy);
    }

    // draw
    this->arduboy.print(to_world(ball.x));
    this->arduboy.print("\n");
    this->arduboy.print(to_world(ball.y));
    this->arduboy.print("\n");
    this->arduboy.print(this->ball.angle * 180 / M_PI);

    this->ball.draw(this->arduboy);
    this->ball.draw_stick(this->arduboy, 5, 20);
  }

  void reset() {
    // init ball
    this->ball.filled = false;
    this->ball.x = SUB_WIDTH / 2;
    this->ball.y = SUB_HEIGHT / 2;
    this->ball.radius = SUBPIXELS * 4;
    this->ball.vel_x = 0;
    this->ball.vel_y = 0;
    this->ball.is_moving = false;
    this->ball.angle = 0;

    this->obstacles[0].start_x = SUB_WIDTH / 4;
    this->obstacles[0].start_y = SUB_HEIGHT / 4;
    this->obstacles[0].end_x = SUB_WIDTH  / 4;
    this->obstacles[0].end_y = SUB_HEIGHT * 3 / 4;

    this->obstacles[1].start_x = SUB_WIDTH * 3 / 4;
    this->obstacles[1].start_y = SUB_HEIGHT / 4;
    this->obstacles[1].end_x = SUB_WIDTH * 3 / 4;
    this->obstacles[1].end_y = SUB_HEIGHT * 3 / 4;

    this->obstacles[2].start_x = SUB_WIDTH / 4;
    this->obstacles[2].start_y = SUB_HEIGHT / 4;
    this->obstacles[2].end_x = SUB_WIDTH * 3 / 4;
    this->obstacles[2].end_y = SUB_HEIGHT / 4;

    this->obstacles[3].start_x = SUB_WIDTH / 4;
    this->obstacles[3].start_y = SUB_HEIGHT * 3 / 4;
    this->obstacles[3].end_x = SUB_WIDTH * 3 / 4;
    this->obstacles[3].end_y = SUB_HEIGHT * 3 / 4;

    this->paused = false;
    this->state = GameState::RUNNING;
  }

  Arduboy2 arduboy;
  GameState state;

  uint8_t fps;
  bool paused;

  Ball ball;
  Line obstacles[4] = {}; 
};