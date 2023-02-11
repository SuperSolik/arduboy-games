#pragma once

#include "obstacles.h"
#include "game_global_constants.h"
#include "map_segments.h"

ObstacleType map_char_to_obstacle_type(char c) {
    switch (c) {
        case 'x':
            return ObstacleType::BLOCK;
        case '^':
            return ObstacleType::SPIKE;
        case '-':
            return ObstacleType::PLATFORM;
        default:
            return ObstacleType::UNKNOWN;
    }
    // unreachable
    return ObstacleType::UNKNOWN;
}

void create_obstacle(
    Obstacle* o, 
    int16_t start_x, int16_t start_y, uint8_t w, uint8_t h, 
    char obstacle_char,
    uint8_t* created_cnt
){
    ObstacleType type_to_create = map_char_to_obstacle_type(obstacle_char);

    if (type_to_create == ObstacleType::UNKNOWN) return;
    o->_type = type_to_create;
    o->bounds.x = start_x * OBJECT_SIZE;
    o->bounds.y = start_y * OBJECT_SIZE;
    o->bounds.width = w * OBJECT_SIZE;
    o->bounds.height = h * OBJECT_SIZE;
    (*created_cnt)++;
}

int8_t parse_vertical_segment_map_to_obstacles(
    Obstacle obstacles[SEGMENT_W], 
    const char segment_map[SEGMENT_H][SEGMENT_W]
) {
    // int8_t start = 0;
    // int8_t height;
    // uint8_t created_cnt = 0;

    // int8_t obstacle_start_y = 0;
    int8_t obstacle_height = 0;
    uint8_t created_cnt = 0;

    char tile;
    char prev_tile;

    for (int8_t x = 0; x < SEGMENT_W; x++) {
        obstacle_height = 1;
        prev_tile = ' ';
        
        for (int8_t y = SEGMENT_H - 1; y >= 0; y--) {
            tile = (char)pgm_read_byte(&segment_map[y][x]);

            if (prev_tile == tile) {
                obstacle_height++;
            } else {
                create_obstacle(
                    &obstacles[created_cnt], 
                    x, -y + SEGMENT_H - 1, 1, obstacle_height,
                    prev_tile,
                    &created_cnt
                );
                obstacle_height = 1;
            }

            prev_tile = tile;
        }

        // add last obstacle
        create_obstacle(
            &obstacles[created_cnt], 
            x, SEGMENT_H, 1, obstacle_height,
            prev_tile,
            &created_cnt
        );
    }

    return created_cnt;
}

int8_t parse_horizontal_segment_map_to_obstacles(
    Obstacle obstacles[SEGMENT_W], 
    const char segment_map[SEGMENT_H][SEGMENT_W]
){
    int8_t obstacle_start_x = 0;
    int8_t obstacle_width = 0;
    uint8_t created_cnt = 0;

    char tile;
    char prev_tile;

    for (int8_t y = SEGMENT_H - 1; y >= 0; y--) {
        obstacle_start_x = 0;
        obstacle_width = 0;
        prev_tile = ' ';

        for(int8_t x = 0; x < SEGMENT_W; x++) {
            tile = (char)pgm_read_byte(&segment_map[y][x]);

            if (prev_tile == tile) {
                obstacle_width++;
            } else {
                create_obstacle(
                    &obstacles[created_cnt], 
                    obstacle_start_x, -y + SEGMENT_H, obstacle_width, 1, 
                    prev_tile, 
                    &created_cnt
                );
                obstacle_start_x = x;
                obstacle_width = 1;
            }

            prev_tile = tile;
        }
        // create last obstacle
        // create_obstacle(
        //     &obstacles[created_cnt], 
        //     obstacle_start_x, y, obstacle_width, 1, 
        //     prev_tile, 
        //     &created_cnt
        // );
    }

    return created_cnt;
}

int8_t parse_segment_to_obstacles(Obstacle obstacles[SEGMENT_W], const Segment& seg) {
    if (seg.load_orientation == SegmentLoadOrientation::HORIZONTAL){
        return parse_horizontal_segment_map_to_obstacles(obstacles, SEGMENT_MAPS[seg.map_idx]);
    } else {
        return parse_vertical_segment_map_to_obstacles(obstacles, SEGMENT_MAPS[seg.map_idx]);
    }
}

uint8_t segment_from_height(uint8_t height) {
    uint8_t picked_idx = random(0, SEGMENT_CNT);
    return picked_idx;
}