#include <Arduboy2.h>

#include "bitmaps.h"
#include "point.h"

enum class ObstacleType : uint8_t {
  PLATFORM = 0,
  BLOCK = 1,
  SPIKE = 2,
  COUNT = 3,
};

struct Obstacle {
  ObstacleType type;
  int16_t x;
  int16_t y;
  uint8_t w;
  uint8_t h;
  bool enabled;

  constexpr Rect ardu_rect() {
    return Rect(
      this->x,
      this->y,
      this->w,
      this->h
    );
  }
};

struct Player {
  int16_t x;
  int16_t y;
  uint8_t w;
  uint8_t h;
  bool is_jumping;

  constexpr Rect ardu_rect() {
    return Rect(
      this->x,
      this->y,
      this->w,
      this->h
    );
  }
};

class Game {
  static constexpr uint8_t DEFAULT_FPS = 24;
  static constexpr uint8_t STARS_CNT = 55;
  static constexpr uint8_t STARS_SPEED = 1;
  static constexpr uint8_t GROUND_Y = 1;
  static constexpr uint8_t OBJECT_SIZE = 16;
  static constexpr uint8_t PLAYER_X = 32 - OBJECT_SIZE;
  static constexpr uint8_t JUMP_LENGTH = 16;
  static constexpr uint8_t OBSTACLES_CNT = 5;
  static constexpr uint8_t LAUNCH_START = 10;
  static constexpr uint8_t LAUNCH_END = 45;

 public:
  Game(uint8_t fps = DEFAULT_FPS) {
    this->fps = fps;
  }

  void init() {
    this->arduboy.begin();
    this->arduboy.setFrameRate(fps);
    this->arduboy.initRandomSeed();
    reset();
  }

  void run() {
    if (!arduboy.nextFrame()) return;

    arduboy.pollButtons();

    if (arduboy.anyPressed(A_BUTTON | B_BUTTON | UP_BUTTON)) {
      if (!player.is_jumping) player.is_jumping = true;
    }

    this->arduboy.clear();

    this->update_and_draw_backgroud();
    this->update_and_draw_obstacles();

    this->arduboy.drawFastHLine(0, this->arduboy.height() - GROUND_Y, arduboy.width(), WHITE);

    this->update_player();
    this->draw_player();
    this->arduboy.display();
  }

 private:
  void reset() {
    player.is_jumping = false;
    animation_idx = 0;
    for (auto& s : stars) {
      s.x = random(0, arduboy.width());
      s.y = random(0, arduboy.height());
    }

    reset_launch_time();
  }

  void reset_launch_time() {
    launch_timer = random(LAUNCH_START, LAUNCH_END);
  }

  void update_and_draw_obstacles() {
    --launch_timer;
    
    if (launch_timer <= 0) {
      for (uint8_t i = 0; i < OBSTACLES_CNT; i++) {
        if (!obstacles[i].enabled) {
          launch_obstacle(i);
          break; 
        }
      }
      reset_launch_time();
    }

    this->obstacle_under_idx = -1;

    int16_t player_mid_x = player.x + player.w / 2;

    for (uint8_t i = 0; i < OBSTACLES_CNT; i++) {
      if (obstacles[i].enabled) {
        
        if (player.x + player.w >= obstacles[i].x && player.x <= obstacles[i].x + obstacles[i].w) {
          this->obstacle_under_idx = i;
        }

        obstacles[i].x -= 3;
        Sprites::drawSelfMasked(obstacles[i].x, obstacles[i].y, obstacles_sprites, 0);
      }
      if (obstacles[i].x <= -OBJECT_SIZE) obstacles[i].enabled = false;
    }
  }
  
  void launch_obstacle(uint8_t obstacle_idx) {
    obstacles[obstacle_idx].enabled = true;
    obstacles[obstacle_idx].x = arduboy.width() - 1;
  }

  void reset_jump() {
    animation_idx = 0;
    jump_idx = 0;
    player.is_jumping = false;
  }

  void update_player() {
    // TODO: reimplement into `is_touching_ground` flag

    if (player.is_jumping) {
      player.y += JUMP_Y[jump_idx++];
      animation_idx = (animation_idx + 1) % 6;

      if (jump_idx >= JUMP_LENGTH) this->reset_jump();
    }

    // if (this->obstacle_under_idx != -1) {
    //   Obstacle &o = obstacles[this->obstacle_under_idx];

    //   if (
    //     arduboy.collide(
    //       player.ardu_rect(),
    //       o.ardu_rect()
    //     ) && player.y + player.h < o.y + o.h / 2
    //   ) {
    //     player.y = o.y - player.h - GROUND_Y - 1;
    //     this->reset_jump();
    //   }
    // }

    // if (player.is_jumping) return;
    // player.y += 4;

    // if (player.y >= HEIGHT - GROUND_Y - player.h) {
    //   player.y = HEIGHT - GROUND_Y - player.h;
    // }
  }

  void draw_player() {
    Sprites::drawSelfMasked(player.x, player.y, cube_sprite, this->animation_idx);
  }

  void update_and_draw_backgroud() {
    for (uint8_t i = 0; i < STARS_CNT; i++) {
      if (stars[i].x < 0) {
        stars[i].x = arduboy.width();
        stars[i].y = random(0, arduboy.height());
      }
      if (i % 2 == 0) {
        stars[i].x -= STARS_SPEED;
      } else if (i % 3 == 0) {
        stars[i].x -= STARS_SPEED * 2;
      } else {
        stars[i].x -= STARS_SPEED * 3;
      }

      arduboy.drawPixel(stars[i].x, stars[i].y, WHITE);
    }
  }

  Arduboy2 arduboy;
  uint8_t fps;
  uint8_t animation_idx;

  bool is_jumping = false;
  uint8_t jump_idx = 0;

  PointS16 stars[STARS_CNT];
  int8_t JUMP_Y[JUMP_LENGTH] = {
      // 1, 6, 12, 18, 21, 27, 29, 34, 29, 27, 21, 18, 12, 6, 1
       -1, -5, -6, -6, -3, -6, -2, -5, 5, 2, 6, 3, 6, 6, 5, 1
  };

  int16_t launch_timer = 0;

  Player player = {
    .x = PLAYER_X,
    .y = HEIGHT - (OBJECT_SIZE + GROUND_Y),
    .w = OBJECT_SIZE,
    .h = OBJECT_SIZE,
    .is_jumping = false,
  };

  int8_t obstacle_under_idx;

  Obstacle obstacles[OBSTACLES_CNT] = {
      {
          .type = ObstacleType::BLOCK,
          .x = WIDTH,
          .y = HEIGHT - (GROUND_Y + OBJECT_SIZE),
          .w = OBJECT_SIZE,
          .h = OBJECT_SIZE,
          .enabled = false,
      },
      {
          .type = ObstacleType::BLOCK,
          .x = WIDTH,
          .y = HEIGHT - (GROUND_Y + OBJECT_SIZE),
          .w = OBJECT_SIZE,
          .h = OBJECT_SIZE,
          .enabled = false,
      },
      {
          .type = ObstacleType::BLOCK,
          .x = WIDTH,
          .y = HEIGHT - (GROUND_Y + OBJECT_SIZE),
          .w = OBJECT_SIZE,
          .h = OBJECT_SIZE,
          .enabled = false,
      },
      {
          .type = ObstacleType::BLOCK,
          .x = WIDTH,
          .y = HEIGHT - (GROUND_Y + OBJECT_SIZE),
          .w = OBJECT_SIZE,
          .h = OBJECT_SIZE,
          .enabled = false,
      },
      {
          .type = ObstacleType::BLOCK,
          .x = WIDTH,
          .y = HEIGHT - (GROUND_Y + OBJECT_SIZE),
          .w = OBJECT_SIZE,
          .h = OBJECT_SIZE,
          .enabled = false,
      }
    };
};