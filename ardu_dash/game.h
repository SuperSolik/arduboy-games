#include <Arduboy2.h>

#include "debug_helper.h"
#include "bitmaps.h"
#include "player.h"
#include "obstacles.h"
#include "map_utils.h"
#include "game_global_constants.h"


static constexpr uint8_t GROUND_Y = 1;


class Game {
  static constexpr uint8_t DEFAULT_FPS = 24;
  static constexpr uint8_t STARS_CNT = 55;
  static constexpr uint8_t STARS_SPEED = 1;
  static constexpr uint8_t PLAYER_X = 32 - OBJECT_SIZE;
  static constexpr uint8_t OBSTACLES_POOL_CAP = 48;

 public:
  Game(uint8_t fps = DEFAULT_FPS) {
    this->fps = fps;
  }

  void init() {
    this->arduboy.begin();
    this->arduboy.setFrameRate(fps);
    this->arduboy.initRandomSeed();
    this->reset();
    arduboy.display();
  }

  void run() {
    if (!arduboy.nextFrame()) return;
    
    // do level generation
    if (load_cnt > 0) {
      load_cnt = preload_obstacle_segments(load_cnt);
    } else if (do_preload) {
      load_cnt = preload_obstacle_segments(1);
      do_preload = false;
    }

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

    player_distance += Obstacle::MOVE_SPEED;
    new_level_cnt = player_distance / 160;
    if (level_cnt != new_level_cnt) {
      do_preload = true;
      init_offset -= SEGMENT_W * OBJECT_SIZE;
    }

    level_cnt = new_level_cnt;
  
    // print segment distance passed counter
    DO_DEBUG(
      arduboy.setCursor(0, 16);
      arduboy.print(new_level_cnt, 10);
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

    player_distance = 0;

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
    // last_segment_height = 0;
    this->init_offset = WIDTH;

    load_cnt = preload_obstacle_segments(3);
    do_preload = false;
    level_cnt = 0;
  }

  int8_t preload_obstacle_segments(int8_t segments_cnt) {
    // int16_t added_obstacle_idx;
    uint8_t parsed_obstacles_cnt;
    uint8_t segment_idx;
    int8_t i = 0;

    Obstacle tmp_obstacles[SEGMENT_W];

    while(i < segments_cnt) {
      // TODO: rename to smth like get_next_segment(last_added_segment_idx)
      segment_idx = segment_from_height(0);

      if (obstacle_pool_free_size < SEGMENTS_METADATA[segment_idx].obstacle_cnt) {
        // return number of segments left to add
        return segments_cnt - i;
      }

      // last_segment_height = SEGMENTS_METADATA[index].end_height;
      parsed_obstacles_cnt = parse_segment_to_obstacles(tmp_obstacles, SEGMENTS_METADATA[segment_idx]);

      for (uint8_t j = 0; j < parsed_obstacles_cnt; j++)  {
        // translate to world y && x offset
        tmp_obstacles[j].to_world_y(HEIGHT);
        tmp_obstacles[j].set_x_offset(init_offset);

        // add to pool
        // TODO: refactor to avoid data copy from temp obstacles
        //       to obstacle pool (use pointers or/and copy constructors)
        obstacle_add_to_pool(tmp_obstacles[j]);
      }

      // update the initial offset
      init_offset += SEGMENT_W * OBJECT_SIZE;
      last_added_segment_idx = segment_idx;
      i += 1;
    }

    return segments_cnt - i;
  }

  void update_and_draw_obstacles() {
    
    for (uint8_t obstacle_index = 0; obstacle_index < OBSTACLES_POOL_CAP; obstacle_index++) {
      Obstacle& o = obstacle_pool[obstacle_index];

      if (o.enabled) {
        // move obstacle && interact with player
        o.update();
        // TODO: interact only with close obstacles
        //       (relative to the player pos, += 4 * OBJECT_SIZE should do?)
        if (o.bounds.x + o.bounds.width >= player.x) {
          // if obstacle is not behind
          obstacle_pool[obstacle_index].interact(this->player, arduboy);
        }
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

  // uint8_t last_segment_height;
  uint8_t last_added_segment_idx;
  int16_t init_offset;

  int8_t obstacle_pool_free_size;
  int16_t player_distance;
  int8_t load_cnt;
  uint16_t level_cnt;
  uint16_t new_level_cnt;
  bool do_preload;

  Obstacle obstacle_pool[OBSTACLES_POOL_CAP];
};