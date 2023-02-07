#pragma once

#include <Arduboy2.h>

#include "bitmaps.h"
#include "game_global_constants.h"
#include "player.h"
#include "rects_collision.h"

enum class ObstacleType : uint8_t {
  PLATFORM = 0,
  BLOCK = 1,
  SPIKE = 2,
  COUNT = 3,
  FLOOR = 4,  // special technical obstacle
};

struct Obstacle {
  static constexpr uint8_t MOVE_SPEED = 3;

  Obstacle() {
    this->bounds = Rect(0, 0, 0, 0);
    this->_type = ObstacleType::BLOCK;
    this->enabled = false;
  }

  Obstacle(
      const Rect& bounds,
      ObstacleType _type,
      int16_t init_offset = 0,
      bool enabled = false) : bounds(bounds),
                              _type(_type),
                              enabled(enabled) {
    this->set_x_offset(init_offset);
  }

  Obstacle(
      int16_t x, int16_t y, uint8_t width, uint8_t height,
      ObstacleType _type,
      int16_t init_offset = 0,
      bool enabled = false) : Obstacle(Rect(x, y, width, height), _type, init_offset, enabled) {}

  void update() {
    bounds.x -= MOVE_SPEED;
  }

  void set_x_offset(int16_t x_offset = 0) {
    bounds.x += x_offset;
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
        // h = player.height;
        // w = player.x + player.width - bounds.x;
        // player.x = bounds.x - player.width;

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
          player.dead();
          // player.x = bounds.x - player.width;
          
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
          player.dead();
          // player.x = bounds.x - player.width;
          
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

  void to_world_y(int16_t y_offset) {
    this->bounds.y = y_offset - this->bounds.y;
  }

  void draw(const Arduboy2& a) {
    switch (this->_type) {
      case ObstacleType::FLOOR:
        a.drawFastHLine(bounds.x, bounds.y, bounds.width, WHITE);
        break;
      default:
        for (int16_t y = bounds.y; y < bounds.y + bounds.height; y += OBJECT_SIZE) {
          for (int16_t x = bounds.x; x < bounds.x + bounds.width; x += OBJECT_SIZE) {
            Sprites::drawSelfMasked(x, y, obstacles_sprites, static_cast<uint8_t>(_type));
          }
        }
        break;
    }
  }

  Rect bounds;
  ObstacleType _type;
  bool enabled;
};

struct BlockObstacle : Obstacle {
  BlockObstacle(int16_t x, int16_t y, uint8_t width, uint8_t height, uint8_t init_offset = 0) : Obstacle(x, y, width, height, ObstacleType::BLOCK, init_offset) {}
};