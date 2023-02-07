#include <Arduboy2.h>

#include "debug_helper.h"
#include "bitmaps.h"
#include "player.h"
#include "obstacles.h"
#include "map.h"
#include "game_global_constants.h"


static constexpr uint8_t GROUND_Y = 1;


class Game {
  static constexpr uint8_t DEFAULT_FPS = 24;
  static constexpr uint8_t STARS_CNT = 55;
  static constexpr uint8_t STARS_SPEED = 1;
  static constexpr uint8_t PLAYER_X = 32 - OBJECT_SIZE;
  static constexpr uint8_t OBSTACLES_POOL_CAP = 32;

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


    DO_DEBUG(
      arduboy.setCursor(0, 0);
      arduboy.print(player.is_grounded ? "g" : "n");
    );

    player.update();
    // TODO: use in level load
    // we need to keep track of distance for segments preload
    // when the player passed 2 * SEGMENT_W * OBJECT_SIZE pixels,
    // it means that the segment is completely gone,
    // (also the start offset of the new segment 
    //  just got reduced by SEGMENT_W * OBJECT_SIZE)
    // and we need to load a new one
    player_distance += Obstacle::MOVE_SPEED;    
    int8_t level_passed_cnt = player_distance / 160;

    // print segment distance passed counter
    DO_DEBUG(
      arduboy.setCursor(0, 16);
      arduboy.print(level_passed_cnt, 10);
    );

    
    
    // print obstacle count
    DO_DEBUG(
      arduboy.setCursor(0, 32);
      arduboy.print(obstacle_pool_free_size);
    );

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

    player_distance = -160;

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

    obstacle_pool_free_size = OBSTACLES_POOL_CAP;

    // initialize obstacles, hardcoded for now
    // TODO: add continuous generation (add new obstacles offscreen)
    //       when some portion of present obstacles have passed
    //       figure out how to change start offsets

    Obstacle tmp_obstacles[10];

    uint16_t init_offset = WIDTH;
    int8_t added_obstacle_idx;

    // hardcoded segments for now
    for(int8_t s_id = 4; s_id <= 7; s_id++) {
      Segment& next_seg = segments[s_id];
      if (obstacle_pool_free_size < next_seg.obstacle_cnt) {
        break;
      }
      uint8_t parsed_obstacles_cnt = parse_segment_to_obstacles(tmp_obstacles, next_seg);

      for (uint8_t i = 0; i < parsed_obstacles_cnt; i++)  {
        // translate to world y
        tmp_obstacles[i].to_world_y(HEIGHT);

        // add to pool
        // TODO: refactor to avoid data copy from temp obstacles
        //       to obstacle pool (use pointers or/and copy constructors)
        added_obstacle_idx = obstacle_add_to_pool(
          tmp_obstacles[i]
        );
        
        obstacle_pool[added_obstacle_idx].set_x_offset(init_offset);
      }

      // update the initial offset
      init_offset += SEGMENT_W * OBJECT_SIZE;
    }
  }

  void update_and_draw_obstacles() {
    for (uint8_t obstacle_index = 0; obstacle_index < OBSTACLES_POOL_CAP; obstacle_index++) {
      if (obstacle_pool[obstacle_index].enabled) {
        // move obstacle && interact with player
        obstacle_pool[obstacle_index].update();
        // TODO: interact only with close obstacles
        //       (relative to the player pos, += 4 * OBJECT_SIZE should do?)
        obstacle_pool[obstacle_index].interact(this->player, arduboy);
        obstacle_pool[obstacle_index].draw(this->arduboy);

        // if obstacle out of screen -> remove
        Rect& o_bounds = obstacle_pool[obstacle_index].bounds;
        if (o_bounds.x + o_bounds.width <= 0) {
          obstacle_remove_from_pool(obstacle_index);
        }
      }
    }
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

  int8_t obstacle_add_to_pool(const Obstacle& o) {
    for (uint8_t i = 0; i < OBSTACLES_POOL_CAP; i++) {
        if (!obstacle_pool[i].enabled) {
            obstacle_pool[i] = o;
            obstacle_pool[i].enabled = true;
            --obstacle_pool_free_size;
            return i;
        }
    }
    return -1; // failed to add to pool
  }

  bool obstacle_remove_from_pool(int8_t obstacle_index) {
    if (obstacle_index >= OBSTACLES_POOL_CAP || obstacle_index < 0) return false;
    obstacle_pool[obstacle_index].enabled = false;
    obstacle_pool_free_size++;
    return true;
  }

  Arduboy2 arduboy;
  uint8_t fps;

  Point stars[STARS_CNT];

  Player player;

  int8_t obstacle_under_idx;

  Obstacle floor;

  Obstacle obstacle_pool[OBSTACLES_POOL_CAP];
  int16_t obstacle_pool_free_size;

  int16_t player_distance;
};