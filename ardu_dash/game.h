#include <Arduboy2.h>

#include "debug_helper.h"
#include "bitmaps.h"
#include "player.h"
#include "obstacles.h"
#include "map.h"


static constexpr uint8_t GROUND_Y = 1;


class Game {
  static constexpr uint8_t DEFAULT_FPS = 24;
  static constexpr uint8_t STARS_CNT = 55;
  static constexpr uint8_t STARS_SPEED = 1;
  static constexpr uint8_t OBJECT_SIZE = 16;
  static constexpr uint8_t PLAYER_X = 32 - OBJECT_SIZE;
  static constexpr uint8_t OBSTACLES_POOL_CAP = 32;
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

    // do level load/generation

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
    // create player
    player = Player(
      PLAYER_X,
      HEIGHT - (12 + GROUND_Y),
      12,
      12
    );

    // create special obstacle representing floor
    floor = Obstacle(
      0, HEIGHT - GROUND_Y, WIDTH, 0,
      ObstacleType::FLOOR,
      true
    );

    // reset starts
    for (auto& s : stars) {
      s.x = random(0, arduboy.width());
      s.y = random(0, arduboy.height());
    }

    // reset obstacle pool
    for (auto& o : obstacle_pool) {
      o.enabled = false;
    }

    // initialize obstacles

    Obstacle tmp_obstacles[10];

    uint16_t init_offset = WIDTH;
    int8_t added_obstacle_idx;

    for(int8_t s_id = 2; s_id <= 4; s_id++) {
      uint8_t created = parse_segment_map_to_obstacles(tmp_obstacles, segments[s_id].map);

      for (uint8_t i = 0; i < created; i++)  {
        // translate to world y
        tmp_obstacles[i].to_world_y(HEIGHT);

        added_obstacle_idx = obstacle_add_to_pool(
          tmp_obstacles[i]
        );
        
        obstacle_pool[added_obstacle_idx].set_x_offset(init_offset);
      }
      init_offset += SEGMENT_W * OBJECT_SIZE;
    }
  }

  void update_and_draw_obstacles() {
    for (uint8_t obstacle_index = 0; obstacle_index < OBSTACLES_POOL_CAP; obstacle_index++) {
      if (obstacle_pool[obstacle_index].enabled) {
        obstacle_pool[obstacle_index].update();
        obstacle_pool[obstacle_index].interact(this->player, arduboy);
        obstacle_pool[obstacle_index].draw(this->arduboy);

        Rect& o_bounds = obstacle_pool[obstacle_index].bounds;
        if (o_bounds.x + o_bounds.width <= 0) {
          obstacle_remove_from_pool(obstacle_index);
        }
      }
    }
  }
  
  // void launch_obstacle(uint8_t obstacle_idx) {
  //   obstacle_pool[obstacle_idx].enabled = true;
  //   obstacle_pool[obstacle_idx].bounds.x = arduboy.width() - 1;
  // }

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

  int8_t obstacle_add_to_pool(const Obstacle& o) {
    for (uint8_t i = 0; i < OBSTACLES_POOL_CAP; i++) {
        if (!obstacle_pool[i].enabled) {
            obstacle_pool[i] = o;
            obstacle_pool[i].enabled = true;
            return i;
        }
    }
    return -1; // failed to add to pool
  }

  bool obstacle_remove_from_pool(int8_t obstacle_index) {
    if (obstacle_index >= OBSTACLES_POOL_CAP || obstacle_index < 0) return false;
    obstacle_pool[obstacle_index].enabled = false;
    return true;
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

  Obstacle obstacle_pool[OBSTACLES_POOL_CAP];
    // BlockObstacle(
    //   WIDTH,
    //   HEIGHT - (GROUND_Y + OBJECT_SIZE - 1),
    //   OBJECT_SIZE * 2,
    //   OBJECT_SIZE
    // ),
    // BlockObstacle(
    //   WIDTH,
    //   HEIGHT - (GROUND_Y + 2 * OBJECT_SIZE - 1),
    //   OBJECT_SIZE * 2,
    //   OBJECT_SIZE
    // ),
    // BlockObstacle(
    //   WIDTH,
    //   HEIGHT - (GROUND_Y + 3 * OBJECT_SIZE - 1),
    //   OBJECT_SIZE * 2,
    //   OBJECT_SIZE
    // )
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