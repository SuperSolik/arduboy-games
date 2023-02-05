#include <Arduboy2.h>

#include "bitmaps.h"
#include "rects_collision.h"

#define DEBUG

#ifdef DEBUG
  #define DO_DEBUG(code) code
#else
  #define DO_DEBUG(code) do {} while(0)
#endif


enum class ObstacleType : uint8_t {
  PLATFORM = 0,
  BLOCK = 1,
  SPIKE = 2,
  COUNT = 3,
  FLOOR = 4, // special technical obstacle
};

constexpr static uint8_t PLAYER_JUMP_LENGTH = 16;
constexpr static int8_t PLAYER_JUMP_Y[PLAYER_JUMP_LENGTH] = {
    -8, -4, -4, -3, -3, -2, -2, -1, 1, 2, 2, 3, 3, 4, 4, 8
};
static constexpr uint8_t GROUND_Y = 1;

struct Player {
  static constexpr uint8_t FALL_SPEED = 5;

  Player() = default;

  constexpr Player(int16_t x, int16_t y, uint8_t width, uint8_t height): 
      x(x), y(y), width(width), height(height), 
      is_jumping(false),
      is_dead(false),
      is_grounded(true),
      jump_idx(0),
      animation_idx(0) {}

  constexpr Player(const Rect& bounds): 
    x(bounds.x), y(bounds.y), width(bounds.width), height(bounds.height), 
    is_jumping(false),
    is_dead(false),
    is_grounded(true),
    jump_idx(0),
    animation_idx(0) {}

  constexpr Rect rect() {
    return Rect(
      this->x,
      this->y,
      this->width,
      this->height
    );
  }

  void draw(const Arduboy2& a) {
    DO_DEBUG(
      a.print(is_grounded ? "g" : "n");
    );
    Sprites::drawSelfMasked(x-2, y-2, cube_sprite, animation_idx);
  }

  void start_jump() {
    if (is_grounded) {
      this->reset_jump();
      is_jumping = true;
      is_grounded = false;
    }
  }

  void ground() {
    if (is_grounded) return;
    is_grounded = true;
    is_jumping = false;
    this->reset_jump();
  }

  void update() {
    if (this->is_jumping && jump_idx <= PLAYER_JUMP_LENGTH) {
      this->y += PLAYER_JUMP_Y[jump_idx++];
      animation_idx = (animation_idx + 1) % 6;
    } else {
      is_grounded = false;
      y += FALL_SPEED;
    }
  }

  void reset_jump() {
    animation_idx = 0;
    jump_idx = 0;
  }

  void dead() {
    is_dead = true;
  }

  int16_t x;
  int16_t y;
  uint8_t width;
  uint8_t height;

  uint8_t jump_idx;
  uint8_t animation_idx;

  bool is_jumping;
  bool is_grounded;
  bool is_dead;
};

struct Obstacle {
    static constexpr uint8_t MOVE_SPEED = 3;

    Obstacle() = default;

    constexpr Obstacle(const Rect& bounds, ObstacleType _type, bool enabled = false): 
      bounds(bounds),
      _type(_type),
      enabled(enabled) {}

    constexpr Obstacle(
      int16_t x, int16_t y, uint8_t width, uint8_t height, 
      ObstacleType _type, 
      bool enabled = false
    ):
      Obstacle(Rect(x, y, width, height), _type, enabled) {}
    
    void update() {
      bounds.x -= MOVE_SPEED;
      if (bounds.x + bounds.width < 0) enabled = false;
    }

    void block_interact(Player& player, const Arduboy2& a) {
      uint8_t w = 0, h = 0;

      switch (collide_rects(player.rect(), bounds)) {
          case CollisionType::TOP:
          case CollisionType::TOP_LEFT:
              w = player.width;
              h = bounds.y + bounds.height - player.y;
              player.y = bounds.y + bounds.height;
              break;
          case CollisionType::BOTTOM:
          case CollisionType::BOTTOM_LEFT:
              player.ground();
              player.y = bounds.y - player.height;
              break;
          case CollisionType::LEFT:
              h = player.height;
              w = bounds.x + bounds.width - player.x;

              player.x = bounds.x + bounds.width;
              break;
          case CollisionType::RIGHT:
              player.dead();
              h = player.height;
              w = player.x + player.width - bounds.x;
              player.x = bounds.x - player.width;
              // DO_DEBUG(
              //   a.setCursor(0, 8);
              //   a.print("R\n");
              //   a.print(w);
              //   a.print(" ");
              //   a.print(h);
              // );
              break;
          case CollisionType::TOP_RIGHT:
              w = player.x + player.width - bounds.x;
              h = bounds.y + bounds.height - player.y;
              if (h > player.height / 3) {
                  player.x = bounds.x - player.width;
                  player.dead();
                  // DO_DEBUG(
                  //   a.setCursor(0, 8);
                  //   a.print("TR\n");
                  //   a.print(w);
                  //   a.print(" ");
                  //   a.print(h);
                  // );
              }
              break;
          case CollisionType::BOTTOM_RIGHT:
              w = player.x + player.width - bounds.x;
              h = player.y + player.height - bounds.y;
              if (h > player.height / 3) {
                  player.x = bounds.x - player.width;
                  player.dead();
                  // DO_DEBUG(
                  //   a.setCursor(0, 8);
                  //   a.print("BR\n");
                  //   a.print(w);
                  //   a.print(" ");
                  //   a.print(h);
                  // );
              }
              break;
          default:
              break;
        }
    }

    void floor_interact(Player& player, const Arduboy2& a) {
      if (player.y + player.height >= this->bounds.y) {
        player.ground();
        player.y = this->bounds.y - player.height;
      }
    }

    void interact(Player& player, const Arduboy2& a) {
      switch (this->_type) {
        case ObstacleType::BLOCK:
          this->block_interact(player, a);
          break;
        case ObstacleType::FLOOR:
          this->floor_interact(player, a);
          break;
        default: 
          break;
      }
    }

    void draw(const Arduboy2& a) {
        switch (this->_type) {
          case ObstacleType::BLOCK:
            a.drawRect(bounds.x, bounds.y, bounds.width, bounds.height, WHITE);
            break;
          case ObstacleType::FLOOR:
            a.drawFastHLine(bounds.x, bounds.y, bounds.width, WHITE);
          break;
          default:
            break;
        }
    }

    Rect bounds;
    ObstacleType _type;
    bool enabled;
};


struct BlockObstacle: Obstacle {
  constexpr BlockObstacle(int16_t x, int16_t y, uint8_t width, uint8_t height) :
    Obstacle(x, y, width, height, ObstacleType::BLOCK, false) {}
};

class Game {
  static constexpr uint8_t DEFAULT_FPS = 24;
  static constexpr uint8_t STARS_CNT = 55;
  static constexpr uint8_t STARS_SPEED = 1;
  static constexpr uint8_t OBJECT_SIZE = 16;
  static constexpr uint8_t PLAYER_X = 32 - OBJECT_SIZE;
  static constexpr uint8_t OBSTACLES_CNT = 3;
  static constexpr uint8_t LAUNCH_START = 20;
  static constexpr uint8_t LAUNCH_END = 20;

 public:
  Game(uint8_t fps = DEFAULT_FPS) {
    this->fps = fps;
  }

  void init() {
    this->arduboy.begin();
    this->arduboy.setFrameRate(fps);
    this->arduboy.initRandomSeed();
    this->reset();
  }

  void run() {
    if (!arduboy.nextFrame()) return;

    arduboy.pollButtons();

    if (arduboy.anyPressed(A_BUTTON | B_BUTTON | UP_BUTTON)) {
      player.start_jump();
    }

    if (player.is_dead) {
      this->arduboy.delayShort(1000);
      this->reset();
      return;
    }

    this->arduboy.clear();

    player.update();

    floor.interact(player, arduboy);
    

    this->update_and_draw_backgroud();
    this->update_and_draw_obstacles();

    floor.draw(this->arduboy);  
    player.draw(this->arduboy);

    this->arduboy.display();
  }

 private:
  void reset() {
    player = Player(
      PLAYER_X,
      HEIGHT - (12 + GROUND_Y),
      12,
      12
    );

    floor = Obstacle(
      0, HEIGHT - GROUND_Y, WIDTH, 0,
      ObstacleType::FLOOR,
      true
    );

    for (auto& s : stars) {
      s.x = random(0, arduboy.width());
      s.y = random(0, arduboy.height());
    }

    for (auto& o : obstacles) {
      o.enabled = false;
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

    for (uint8_t i = 0; i < OBSTACLES_CNT; i++) {
      if (obstacles[i].enabled) {
        obstacles[i].update();
        obstacles[i].interact(this->player, arduboy);
        obstacles[i].draw(this->arduboy);
      }
    }
  }
  
  void launch_obstacle(uint8_t obstacle_idx) {
    obstacles[obstacle_idx].enabled = true;
    obstacles[obstacle_idx].bounds.x = arduboy.width() - 1;
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

  Point stars[STARS_CNT];
  
  int16_t launch_timer = 0;

  Player player;

  int8_t obstacle_under_idx;

  Obstacle floor;

  Obstacle obstacles[OBSTACLES_CNT] = {
    BlockObstacle(
      WIDTH,
      HEIGHT - (GROUND_Y + OBJECT_SIZE - 1),
      OBJECT_SIZE * 2,
      OBJECT_SIZE
    ),
    BlockObstacle(
      WIDTH,
      HEIGHT - (GROUND_Y + 2 * OBJECT_SIZE - 1),
      OBJECT_SIZE * 2,
      OBJECT_SIZE
    ),
    BlockObstacle(
      WIDTH,
      HEIGHT - (GROUND_Y + 3 * OBJECT_SIZE - 1),
      OBJECT_SIZE * 2,
      OBJECT_SIZE
    )
      // {
      //     .type = ObstacleType::BLOCK,
      //     .x = WIDTH,
      //     .y = HEIGHT - (GROUND_Y + OBJECT_SIZE),
      //     .w = OBJECT_SIZE * 10,
      //     .h = OBJECT_SIZE,
      //     .enabled = false,
      // },
      // // {
      //     .type = ObstacleType::BLOCK,
      //     .x = WIDTH,
      //     .y = HEIGHT - (GROUND_Y + OBJECT_SIZE),
      //     .w = OBJECT_SIZE,
      //     .h = OBJECT_SIZE,
      //     .enabled = false,
      // },
      // {
      //     .type = ObstacleType::BLOCK,
      //     .x = WIDTH,
      //     .y = HEIGHT - (GROUND_Y + OBJECT_SIZE),
      //     .w = OBJECT_SIZE,
      //     .h = OBJECT_SIZE,
      //     .enabled = false,
      // },
      // {
      //     .type = ObstacleType::BLOCK,
      //     .x = WIDTH,
      //     .y = HEIGHT - (GROUND_Y + OBJECT_SIZE),
      //     .w = OBJECT_SIZE,
      //     .h = OBJECT_SIZE,
      //     .enabled = false,
      // },
      // {
      //     .type = ObstacleType::BLOCK,
      //     .x = WIDTH,
      //     .y = HEIGHT - (GROUND_Y + OBJECT_SIZE),
      //     .w = OBJECT_SIZE,
      //     .h = OBJECT_SIZE,
      //     .enabled = false,
      // }
    };
};